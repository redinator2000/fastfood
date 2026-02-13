#ifndef FF_UNIQUE_FLAT_HPP
#define FF_UNIQUE_FLAT_HPP

#include "unique.hpp"

namespace ff
{

namespace impl
{
    struct Unique_food_Vtable { virtual void dont_instantiate() = 0; };
    template <typename Tgeneral, typename Data>
    const Unique_food_Vtable * unique_food_vtable_ripper()
    {
        return static_cast<Unique_food_Vtable *>(vtable_ripper<Unique_food<Tgeneral, Data>>());
    }
} // namespace impl

template <typename Tgeneral>
class Unique_Flat
{
private:
    const impl::Unique_food_Vtable * unique_food_vtable = nullptr;
    T_Data_Alias data_alias = 0;
    Unique_Flat(const impl::Unique_food_Vtable * n_vtable, T_Data_Alias n_data) :
        unique_food_vtable(n_vtable),
        data_alias(n_data)
        {}
public:
    template <typename Data>
    Unique_Flat(Unique_food<Tgeneral, Data> && ts) :
    Unique_Flat(impl::unique_food_vtable_ripper<Tgeneral, Data>(), ts.release_data())
        {}

    template <typename Data>
    bool has_alternative() const
        { return unique_food_vtable == impl::unique_food_vtable_ripper<Tgeneral, Data>(); }
    template <Data_Empty Data>
    Data * get_data()
    {
        assert(has_alternative<Data>());
        return static_cast<Data *>(&data_alias);
    }
    template <Data_Empty Data>
    const Data * get_data() const
    {
        assert(has_alternative<Data>());
        return static_cast<Data *>(&data_alias);
    }
    template <Data_Trivial Data>
    Data * get_data()
    {
        assert(has_alternative<Data>());
        return static_cast<Data *>(&data_alias);
    }
    template <Data_Trivial Data>
    const Data * get_data() const
    {
        assert(has_alternative<Data>());
        return static_cast<Data *>(&data_alias);
    }
    template <Data_Dynamic Data>
    Data * get_data()
    {
        assert(has_alternative<Data>());
        return static_cast<Data *>(data_alias);
    }
    template <Data_Dynamic Data>
    const Data * get_data() const
    {
        assert(has_alternative<Data>());
        return static_cast<Data *>(data_alias);
    }

    ~Unique_Flat()
    {
        if(data_alias)
        {
            reference_Unique_Flat_to_Tgeneral<Tgeneral>(this)->clear_data();
            assert(data_alias == 0);
        }
    }
    Unique_Flat(const Unique_Flat & other) = delete;
    Unique_Flat & operator=(const Unique_Flat & other) = delete;
    Unique_Flat(Unique_Flat && other) noexcept = default;
    Unique_Flat& operator=(Unique_Flat && other) noexcept = default;
};

template <typename Tgeneral>
Tgeneral * reference_Unique_Flat_to_Food_Interface(Unique_Flat<Tgeneral> * tf)
{
    return reinterpret_cast<Tgeneral *>(tf);
}
template <typename Tgeneral>
const Tgeneral * reference_Unique_Flat_to_Tgeneral(const Unique_Flat<Tgeneral> * tf)
{
    return reinterpret_cast<Tgeneral *>(tf);
}

}; // namespace ff

#endif // FF_UNIQUE_FLAT_HPP
