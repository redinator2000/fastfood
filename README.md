# fastfood

fastfood is a c++ header-only library that creates fat pointers (16 bytes) that store a vtable, and either a data pointer, or the data itself.

## Types

* `Data`

  User implemented types that can hold any data. If trivially copyable and <= 8 bytes, it will be stored in-place instead of being dynamically allocated.

* `Chef`

  User implemented struct that defines the interface for the OOP vtable, and implementations for specific data types.
  
  - `Chef::Interface_const` defines virtual functions that don't mutate data
  
  - `Chef::Interface_mut` defines virtual functions that do mutate data

* `ff::Unique_food<Chef, Data>`

  The owner of the `Data data`. Has similar semantics to `std::unique_ptr`, since for non-trivial data it is just a wrapper for `std::unique_ptr<Data>`. 16 bytes for most types, 8 bytes for `std::is_empty` types.

  Inherits from `Chef::Interface_mut`.

* `ff::Unique_flat<Chef>`

  Type erasure version of `ff::Unique_food<Chef, Data>`. Always 16 bytes.
  
  This allows different `Data` types to all be put into a `std::vector<ff::Unique_flat<Chef>>`.
  
  Has `std::variant` like semantics instead of inheriting from a `Chef::Interface`.

* `ff::Weak_const_food<Chef, Data>`

  Weak pointer to `ff::Unique_food<Chef, Data>`. Alternative to casting `ff::Unique_food<Chef, Data>` to `const Chef::Interface_const *`.

  It may be implemented as a deep or shallow copy, so it is const. 
  
  Inherits from `Chef::Interface_const`.
  
* `ff::Weak_mut_food<Chef, Data>`

  Weak pointer to `ff::Unique_food<Chef, Data>`. In most cases, casting `ff::Unique_food<Chef, Data>` to `Chef::Interface_mut *` is preferable.

  Implemented as a normal 2-pointer fat pointer.
  
  Inherits from `Chef::Interface_mut`.

## Chef

Most fastfood types inherit from a `Chef::Interface` provided by the library user.

```cpp
struct My_Chef // can be called anything
{
    struct Interface_const : public ff::Chef_Base_const<My_Chef>
    {
        // declare all const methods in here
    
        virtual bool is_positive() const = 0;
    };
    struct Interface_mut : public ff::Chef_Base_mut<Interface_const>
    {
        // declare all mutable methods in here
        
        virtual void add_1() = 0;
    };
    template <typename Has_get_data, typename Data>
    struct Implement_const : public Has_get_data
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
    struct Implement_mut : public Implement_const<Has_get_data, Data>
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
