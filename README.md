# fastfood

fastfood is a c++ header-only library that creates fat pointers (16 bytes) that store a vtable, and either a data pointer, or the data itself.

Intended for storing lots of small (<= 8 bytes) types in a contiguous array, with the occasional larger type.

## Types

* `Data`

  User implemented types that can hold any data. If trivially copyable and <= 8 bytes, it will be stored in-place instead of being dynamically allocated.
  
  If you want to check if a `Data` type will take advantage of fastfood small optimizations, use `static_assert( ! ff::Data_Dynamic<my_small_type>)`. Large types will be dynamically allocated `static_assert(ff::Data_Dynamic<my_big_type>)`.
  
  These types cannot have any virtual methods (otherwise there's no point to using fastfood fat pointers).
  
  These types must have `operator==` defined (usually default).

* `Chef`

  User implemented struct that defines the interface for the OOP vtable, and implementations for specific data types.
  
  - `Chef::Interface_const` defines virtual functions that don't mutate data
  
  - `Chef::Interface_mut` defines virtual functions that do mutate data. Inherits (indirectly) from `Chef::Interface_const`

* `ff::Unique_food<Chef, Data>`

  Inherits from `Chef::Interface_mut`.

  The owner of the `Data data`. Has similar semantics to `std::unique_ptr`, since for non-trivial data it is just a wrapper for `std::unique_ptr<Data>`. 16 bytes for most types, 8 bytes for `std::is_empty` types.

* `ff::Unique_flat<Chef>`

  Type erasure version of `ff::Unique_food<Chef, Data>`. Always 16 bytes.
  
  This allows different `Data` types to all be put into a `std::vector<ff::Unique_flat<Chef>>`. Usually `ff::Unique_flat<Chef>` will be the owner of `Data`.
  
  Has `std::variant` like semantics instead of inheriting from a `Chef::Interface`.
  
  This type has the exact same bit layout as `ff::Unique_food<Chef, Data>`. fastfood leverages this in its efficient implementation of type conversions.

* `ff::Weak_const_food<Chef, Data>`

  Inherits from `Chef::Interface_const`.

  Weak pointer to `ff::Unique_food<Chef, Data>`. Alternative to casting `ff::Unique_food<Chef, Data>` to `const Chef::Interface_const *`.

  It may be implemented as a copy or pointer, so it is const. 
  
  WARNING - since this may or may not be implemented as a copy, it is unsafe to modify the underlying `ff::Unique_food<Chef, Data>` during the lifetime of `ff::Weak_const_food<Chef, Data>`.
  
* `ff::Weak_mut_food<Chef, Data>`

  Inherits from `Chef::Interface_mut`.

  Weak pointer to `ff::Unique_food<Chef, Data>`. In most cases, casting `ff::Unique_food<Chef, Data>` to `Chef::Interface_mut *` is preferable.

  Implemented as a normal 2-pointer fat pointer.
  

## Chef

Any `food` type inherits from a `Chef::Interface` provided by the library user.

```cpp
struct My_Chef // can be called anything
{
    struct Interface_const : public ff::Chef_Base_const<My_Chef> // must have this signature
    {
        // declare all const methods in here
    
        virtual bool is_positive() const = 0;
    };
    struct Interface_mut : public ff::Chef_Base_mut<Interface_const> // must have this signature
    {
        // declare all mutable methods in here
        
        virtual void add_1() = 0;
    };
    template <typename Has_get_data, typename Data>
    struct Implement_const : public Has_get_data // must have this signature
    {
        // implement all const methods here
    
        bool is_positive() const override
        {
            const Data * data = this->get_data(); // Implement_const inherits from typename Has_get_data, so it has access to this->get_data()
            if(!data) // if Data is small and trivial, then this will never fail. Otherwise, data is stored inside a `std::unique_ptr`, which can be empty.
                return false;
            if constexpr (std::is_same_v<Data, int>)
                return *data > 0;
            else if constexpr (std::is_same_v<Data, float>)
                return *data > 0.0f;
            else
            {
                static_assert(false);
                return false;
            }
        }
    };
    template <typename Has_get_data, typename Data>
    struct Implement_mut : public Implement_const<Has_get_data, Data> // must have this signature
    {
        // implement all mutable methods here
        
        void add_1() override
        {
            const Data * data = this->get_data_mut(); // Implement_mut gets this->get_data() and this->get_data_mut()
            if(!data) // 
                return;
            if constexpr (std::is_same_v<Data, int>)
                *data += 1;
            else if constexpr (std::is_same_v<Data, float>)
                *data > 1.0f;
            else
            {
                static_assert(false);
            }
        }
    };
    
    // struct My_Chef is never used itself, its just convenient to put all these types into one single template parameter
};
```

## Chef inheritance tree

fastfood has a long inheritance chain for its fundumental types.

Starting from the most great grandparent at the top:

* `ff::Chef_Base_const` - top level virtual const methods

* `My_Chef::Interface_const` - user provided declarations to virtual const methods

* `ff::Chef_Base_mut`

* `My_Chef::Interface_mut` - user provided declarations to virtual non-const methods

* Several internal implementation layers can be in here. These are the ones that declare `get_data()` and `get_data_mut()` member functions.

  Below here, classes have a `typename Data` template parameter, so they can have functions that use `Data` (especially `std::is_same_v<Data, ...>`).

* `My_Chef::Implment_const` - user provided implementations to methods in `My_Chef::Interface_const`

* `My_Chef::Implement_mut` - user provided implementations to methods in `My_Chef::Interface_mut`

* `ff::Unique_food<Chef, Data>`, `ff::Weak_mut_food<Chef, Data>`, `ff::Weak_const_food<Chef, Data>` - these types implement `get_data()` and `get_data_mut()`, so they can be instantiated.

For `ff::Weak_const_food<Chef, Data>`, the mutable steps in the tree aren't present. The need to pluck the mutable steps is why this chain is so granular.

## Methods and Functions

`const Data * this->get_data()` and `Data * this->get_data_mut()` - used inside `My_Chef::Implment` methods

`bool ff::Chef_Base_const::equals_food(const Chef_Base_const *)` - uses `operator==` to compare values of any food type.

`ff::Unique_flat<Chef> ff::Chef_Base_const::clone()` - clones into a type-erased type.

`bool ff::Chef_Base_const::has_value()` and `bool ff::Unique_flat::has_value()` - For large `Data` that is implemented in a `std::unique_ptr`, it can be constructed with `nullptr`. In this case, `has_value()` returns false. Otherwise, it returns true.

`bool ff::Unique_flat::has_alternative<Data>()` - important boolean check before converting a `ff::Unique_flat` to a non-erased type

### Conversion Functions

`Data * ff::flat_cast<Data>(ff::Unique_flat *)` - gets data from a `ff::Unique_flat`. Returns `nullptr` if the unique_flat stores the wrong type, is null, or is empty.

`Data & ff::flat_cast<Data>(ff::Unique_flat &)` - unsafe version that doesn't do checks

`ff::as_interface()` - turns a `ff::Unique_flat<Chef> *` into `Chef::Interface_mut *`, or `const ff::Unique_flat<Chef> *` into `const Chef::Interface_const *`

### Conversion Constructors

`ff::Unique_food<Chef, Data>` and `ff::Unique_flat<Chef>` have move constructors between the two

`ff::Weak_mut_food<Chef, Data>` has constructors for references or pointers to `ff::Unique_food<Chef, Data>` and `ff::Unique_flat<Chef>`

`ff::Weak_const_food<Chef, Data>` has constructors for const ref to `ff::Unique_food<Chef, Data>`, `ff::Unique_flat<Chef>`, and `ff::Weak_mut_food<Chef, Data>`

### Conversion Inheritance

These types inherit from `Chef::Interface_const` and can follow all c++ inheritance conversions.

* `ff::Unique_food<Chef, Data>`
* `ff::Weak_mut_food<Chef, Data>`
* `ff::Weak_const_food<Chef, Data>`

These types inherit from `Chef::Interface_mut`

* `ff::Unique_food<Chef, Data>`
* `ff::Weak_mut_food<Chef, Data>`

