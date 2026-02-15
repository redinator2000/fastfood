#ifndef FF_WEAK_CONST_HPP
#define FF_WEAK_CONST_HPP

#include "chef.hpp"

namespace ff
{

template <typename Chef, Data_Empty Data>
struct Weak_const_food<Chef, Data> : public impl::Container_Parent_const<Chef, Data>
{
    const Data * get_data() const override
        { return nullptr; }
};
template <typename Chef, Data_Trivial Data>
struct Weak_const_food<Chef, Data> : public impl::Container_Parent_const<Chef, Data>
{
    union
    {
        const Data data = Data();
        const Trivial_Alias data_alias;
    };
    const Data * get_data() const override
        { return &data; }

    Weak_const_food(Data n_data) : data(n_data)
        {}
};
template <typename Chef, Data_Dynamic Data>
struct Weak_const_food<Chef, Data> : public impl::Container_Parent_const<Chef, Data>
{
    const Data * const data = nullptr;

    const Data * get_data() const override
        { return data; }

    Weak_const_food(const Data * n_data) : data(n_data)
        {}
};

} // namespace ff

#endif // FF_WEAK_CONST_HPP
