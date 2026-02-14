#ifndef FF_WEAK_CONST_HPP
#define FF_WEAK_CONST_HPP

#include "food.hpp"

namespace ff
{

template <typename Chef, Data_Empty Data>
struct Weak_const_food<Chef, Data> : public Tinterface_Impl<Chef, Weak_const_food<Chef, Data>, Data>
{
    T_Data_Alias release_data() override
        { assert(false); return 0; }
    void clear_data() override
        { assert(false); }

    const Data * get_data() const override
        { return nullptr; }
};
template <typename Chef, Data_Trivial Data>
struct Weak_const_food<Chef, Data> : public Tinterface_Impl<Chef, Weak_const_food<Chef, Data>, Data>
{
    union
    {
        const Data data = Data();
        const Trivial_Alias data_alias;
    };
    const Data * get_data() const override
        { return &data; }

    Weak_const_food() = default;
    Weak_const_food(Data n_data) : data(n_data)
        {}
    T_Data_Alias release_data() override
        { assert(false); return 0; }
    void clear_data() override
        { assert(false); }
};
template <typename Chef, Data_Dynamic Data>
struct Weak_const_food<Chef, Data> : public Tinterface_Impl<Chef, Weak_const_food<Chef, Data>, Data>
{
    const Data * const data = nullptr;

    const Data * get_data() const override
        { return data; }

    Weak_const_food() = default;
    Weak_const_food(const Data * n_data) : data(n_data)
        {}
    T_Data_Alias release_data() override
        { assert(false); return 0; }
    void clear_data() override
        { assert(false); }
};

} // namespace ff

#endif // FF_WEAK_CONST_HPP
