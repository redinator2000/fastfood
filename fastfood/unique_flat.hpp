#ifndef FF_UNIQUE_FLAT_HPP
#define FF_UNIQUE_FLAT_HPP

#include "unique.hpp"

namespace ff
{

template <typename Chef>
class Unique_flat;

namespace impl
{
    struct Unique_food_Vtable { virtual void dont_instantiate() = 0; };
    template <typename Chef, typename Data>
    const Unique_food_Vtable * unique_food_vtable_ripper()
    {
        return static_cast<Unique_food_Vtable *>(vtable_ripper<Unique_food<Chef, Data>>());
    }

    template <typename Chef>
    Chef_Base_own<typename Chef::Interface_mut> * reference_Unique_flat_to_Interface_own(Unique_flat<Chef> * tf)
    {
        return reinterpret_cast<Chef_Base_own<typename Chef::Interface_mut> *>(tf);
    }
}; //namespace impl

template <typename Chef>
class Unique_flat
{
private:
    const impl::Unique_food_Vtable * unique_food_vtable = nullptr;
    Data_Alias data_alias = 0;
    Unique_flat(const impl::Unique_food_Vtable * n_vtable, Data_Alias n_data) :
        unique_food_vtable(n_vtable),
        data_alias(n_data)
        {}
public:
    template <Data_Empty Data>
    explicit Unique_flat(Unique_food<Chef, Data> &&) :
        Unique_flat(impl::unique_food_vtable_ripper<Chef, Data>(), 0)
        {}
    template <Data_Trivial Data>
    explicit Unique_flat(Unique_food<Chef, Data> && ts) :
        Unique_flat(impl::unique_food_vtable_ripper<Chef, Data>(), ts.data_alias)
        {}
    template <Data_Dynamic Data>
    explicit Unique_flat(Unique_food<Chef, Data> && ts) :
        Unique_flat(impl::unique_food_vtable_ripper<Chef, Data>(), reinterpret_cast<Data_Alias>(ts.data.release()))
        {}

    bool has_value() const
        { return unique_food_vtable && as_interface(this)->has_value(); }
    template <typename Data>
    bool has_alternative() const
        { return unique_food_vtable == impl::unique_food_vtable_ripper<Chef, Data>(); }

    ~Unique_flat()
    {
        if(data_alias)
        {
            impl::reference_Unique_flat_to_Interface_own<Chef>(this)->reset();
            assert(data_alias == 0);
        }
    }
    Unique_flat(const Unique_flat & other) = delete;
    Unique_flat & operator=(const Unique_flat & other) = delete;
    Unique_flat(Unique_flat && other) noexcept :
        Unique_flat(other.unique_food_vtable, other.data_alias)
    {
        other.unique_food_vtable = nullptr;
        other.data_alias = 0;
    }
    Unique_flat& operator=(Unique_flat && other) noexcept
    {
        if(data_alias)
        {
            impl::reference_Unique_flat_to_Interface_own<Chef>(this)->reset();
            assert(data_alias == 0);
        }
        this->unique_food_vtable = other->unique_food_vtable;
        this->data_alias = other->data_alias;
        other.unique_food_vtable = nullptr;
        other.data_alias = 0;
    }

    template <Data_Empty Data, typename fChef>
    friend Data * flat_cast(Unique_flat<fChef> *) noexcept;
    template <Data_Empty Data, typename fChef>
    friend Data & flat_cast(Unique_flat<fChef> &);
    template <Data_Trivial Data, typename fChef>
    friend Data * flat_cast(Unique_flat<fChef> *) noexcept;
    template <Data_Trivial Data, typename fChef>
    friend Data & flat_cast(Unique_flat<fChef> &);
    template <Data_Dynamic Data, typename fChef>
    friend Data * flat_cast(Unique_flat<fChef> *) noexcept;
    template <Data_Dynamic Data, typename fChef>
    friend Data & flat_cast(Unique_flat<fChef> &);

    template <Data_Empty Data, typename fChef>
    friend const Data * flat_cast(const Unique_flat<fChef> *) noexcept;
    template <Data_Empty Data, typename fChef>
    friend const Data & flat_cast(const Unique_flat<fChef> &);
    template <Data_Trivial Data, typename fChef>
    friend const Data * flat_cast(const Unique_flat<fChef> *) noexcept;
    template <Data_Trivial Data, typename fChef>
    friend const Data & flat_cast(const Unique_flat<fChef> &);
    template <Data_Dynamic Data, typename fChef>
    friend const Data * flat_cast(const Unique_flat<fChef> *) noexcept;
    template <Data_Dynamic Data, typename fChef>
    friend const Data & flat_cast(const Unique_flat<fChef> &);
};

template <Data_Empty Data, typename Chef>
Data * flat_cast(Unique_flat<Chef> * uf) noexcept
{
    static Data empty_temp;
    if(uf && uf->template has_alternative<Data>())
        return &empty_temp;
    return nullptr;
}
template <Data_Empty Data, typename Chef>
Data & flat_cast(Unique_flat<Chef> &)
{
    static Data empty_temp;
    return empty_temp;
}
template <Data_Trivial Data, typename Chef>
Data * flat_cast(Unique_flat<Chef> * uf) noexcept
{
    if(uf && uf->template has_alternative<Data>())
        return reinterpret_cast<Data *>(&uf->data_alias);
    return nullptr;
}
template <Data_Trivial Data, typename Chef>
Data & flat_cast(Unique_flat<Chef> & uf)
{
    return *reinterpret_cast<Data *>(&uf.data_alias);
}
template <Data_Dynamic Data, typename Chef>
Data * flat_cast(Unique_flat<Chef> * uf) noexcept
{
    if(uf && uf->template has_alternative<Data>() && uf->data_alias != 0)
        return reinterpret_cast<Data *>(uf->data_alias);
    return nullptr;
}
template <Data_Dynamic Data, typename Chef>
Data & flat_cast(Unique_flat<Chef> & uf)
{
    return *reinterpret_cast<Data *>(uf.data_alias);
}

template <Data_Empty Data, typename Chef>
const Data * flat_cast(const Unique_flat<Chef> * uf) noexcept
{
    static Data empty_temp;
    if(uf && uf->template has_alternative<Data>())
        return &empty_temp;
    return nullptr;
}
template <Data_Empty Data, typename Chef>
const Data & flat_cast(const Unique_flat<Chef> &)
{
    static Data empty_temp;
    return empty_temp;
}
template <Data_Trivial Data, typename Chef>
const Data * flat_cast(const Unique_flat<Chef> * uf) noexcept
{
    if(uf && uf->template has_alternative<Data>())
        return reinterpret_cast<Data *>(&uf->data_alias);
    return nullptr;
}
template <Data_Trivial Data, typename Chef>
const Data & flat_cast(const Unique_flat<Chef> & uf)
{
    return reinterpret_cast<Data>(uf.data_alias);
}
template <Data_Dynamic Data, typename Chef>
const Data * flat_cast(const Unique_flat<Chef> * uf) noexcept
{
    if(uf && uf->template has_alternative<Data>() && uf->data_alias != 0)
        return reinterpret_cast<Data *>(uf->data_alias);
    return nullptr;
}
template <Data_Dynamic Data, typename Chef>
const Data & flat_cast(const Unique_flat<Chef> & uf)
{
    return *reinterpret_cast<Data *>(&uf.data_alias);
}

template <typename Chef>
Chef::Interface_mut * as_interface(Unique_flat<Chef> * tf)
{
    return reinterpret_cast<Chef::Interface_mut *>(tf);
}
template <typename Chef>
const Chef::Interface_const * as_interface(const Unique_flat<Chef> * tf)
{
    return reinterpret_cast<const Chef::Interface_const *>(tf);
}

namespace impl
{
    template <typename Chef, typename Impl_Parent, typename Data>
    Unique_flat<Chef> Chef_Implementer_const<Chef, Impl_Parent, Data>::clone() const
    {
        if constexpr (Data_Empty<Data>)
            return Unique_flat<Chef>(ff::make_Unique_food<Chef, Data>());
        else if constexpr (Data_Trivial<Data>)
            return Unique_flat<Chef>(ff::make_Unique_food<Chef, Data>(*this->get_data()));
        else
        {
            const Data * td = this->get_data();
            if(td)
                return Unique_flat<Chef>(ff::make_Unique_food<Chef, Data>(*td));
            else
                return Unique_flat<Chef>(ff::Unique_food<Chef, Data>(std::unique_ptr<Data>(nullptr)));
        }
    }
} // namespace impl

}; // namespace ff

#endif // FF_UNIQUE_FLAT_HPP
