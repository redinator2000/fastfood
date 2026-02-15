#ifndef FF_UNIQUE_FLAT_HPP
#define FF_UNIQUE_FLAT_HPP

#include "unique.hpp"

namespace ff
{

template <typename Chef>
class Unique_Flat;

namespace impl
{
    struct Unique_food_Vtable { virtual void dont_instantiate() = 0; };
    template <typename Chef, typename Data>
    const Unique_food_Vtable * unique_food_vtable_ripper()
    {
        return static_cast<Unique_food_Vtable *>(vtable_ripper<Unique_food<Chef, Data>>());
    }

    template <typename Chef>
    Chef_Base_own<typename Chef::Interface_mut> * reference_Unique_Flat_to_Interface_own(Unique_Flat<Chef> * tf)
    {
        return reinterpret_cast<Chef_Base_own<typename Chef::Interface_mut> *>(tf);
    }
}; //namespace impl

template <typename Chef>
class Unique_Flat
{
private:
    const impl::Unique_food_Vtable * unique_food_vtable = nullptr;
    Data_Alias data_alias = 0;
    Unique_Flat(const impl::Unique_food_Vtable * n_vtable, Data_Alias n_data) :
        unique_food_vtable(n_vtable),
        data_alias(n_data)
        {}
public:
    template <Data_Empty Data>
    Unique_Flat(Unique_food<Chef, Data> &&) :
        Unique_Flat(impl::unique_food_vtable_ripper<Chef, Data>(), 0)
        {}
    template <Data_Trivial Data>
    Unique_Flat(Unique_food<Chef, Data> && ts) :
        Unique_Flat(impl::unique_food_vtable_ripper<Chef, Data>(), ts.data_alias)
        {}
    template <Data_Dynamic Data>
    Unique_Flat(Unique_food<Chef, Data> && ts) :
        Unique_Flat(impl::unique_food_vtable_ripper<Chef, Data>(), reinterpret_cast<Data_Alias>(ts.data.release()))
        {}

    template <typename Data>
    bool has_alternative() const
        { return unique_food_vtable == impl::unique_food_vtable_ripper<Chef, Data>(); }
    template <Data_Empty Data>
    Data * get_data_mut()
    {
        assert(has_alternative<Data>());
        return reinterpret_cast<Data *>(&data_alias);
    }
    template <Data_Empty Data>
    const Data * get_data() const
    {
        assert(has_alternative<Data>());
        return reinterpret_cast<Data *>(&data_alias);
    }
    template <Data_Trivial Data>
    Data * get_data_mut()
    {
        assert(has_alternative<Data>());
        return reinterpret_cast<Data *>(&data_alias);
    }
    template <Data_Trivial Data>
    const Data * get_data() const
    {
        assert(has_alternative<Data>());
        return reinterpret_cast<Data *>(&data_alias);
    }
    template <Data_Dynamic Data>
    Data * get_data_mut()
    {
        assert(has_alternative<Data>());
        return reinterpret_cast<Data *>(data_alias);
    }
    template <Data_Dynamic Data>
    const Data * get_data() const
    {
        assert(has_alternative<Data>());
        return reinterpret_cast<Data *>(data_alias);
    }

    ~Unique_Flat()
    {
        if(data_alias)
        {
            impl::reference_Unique_Flat_to_Interface_own<Chef>(this)->delete_data();
            assert(data_alias == 0);
        }
    }
    Unique_Flat(const Unique_Flat & other) = delete;
    Unique_Flat & operator=(const Unique_Flat & other) = delete;
    Unique_Flat(Unique_Flat && other) noexcept :
        Unique_Flat(other.unique_food_vtable, other.data_alias)
    {
        other.unique_food_vtable = nullptr;
        other.data_alias = 0;
    }
    Unique_Flat& operator=(Unique_Flat && other) noexcept
    {
        if(data_alias)
        {
            impl::reference_Unique_Flat_to_Interface_own<Chef>(this)->delete_data();
            assert(data_alias == 0);
        }
        this->unique_food_vtable = other->unique_food_vtable;
        this->data_alias = other->data_alias;
        other.unique_food_vtable = nullptr;
        other.data_alias = 0;
    }
};

template <typename Chef>
Chef::Interface_mut * reference_Unique_Flat_to_Interface_mut(Unique_Flat<Chef> * tf)
{
    return reinterpret_cast<Chef::Interface_mut *>(tf);
}
template <typename Chef>
const Chef::Interface_const * reference_Unique_Flat_to_Interface_const(const Unique_Flat<Chef> * tf)
{
    return reinterpret_cast<const Chef::Interface_const *>(tf);
}

namespace impl
{
    template <typename Chef, typename Impl_Parent, typename Data>
    Unique_Flat<Chef> Chef_Implementer_const<Chef, Impl_Parent, Data>::clone() const
    {
        if constexpr (Data_Empty<Data>)
            return Unique_Flat<Chef>(ff::make_Unique_food<Chef, Data>());
        else if constexpr (Data_Trivial<Data>)
            return Unique_Flat<Chef>(ff::make_Unique_food<Chef, Data>(*this->get_data()));
        else
        {
            const Data * td = this->get_data();
            if(td)
                return Unique_Flat<Chef>(ff::make_Unique_food<Chef, Data>(*td));
            else
                return Unique_Flat<Chef>(ff::Unique_food<Chef, Data>(std::unique_ptr<Data>(nullptr)));
        }
    }
} // namespace impl

}; // namespace ff

#endif // FF_UNIQUE_FLAT_HPP
