#ifndef FF_WEAK_HPP
#define FF_WEAK_HPP

#include "food.hpp"

namespace ff
{

template <typename Chef, Data_Empty Data>
struct Weak_food<Chef, Data> : public Tinterface_Impl<Chef, Weak_food<Chef, Data>, Data>
{
    T_Data_Alias release_data() override
        { assert(false); return 0; }
    void clear_data() override
        { assert(false); }
};
template <typename Chef, Data_Trivial Data>
struct Weak_food<Chef, Data> : public Tinterface_Impl<Chef, Weak_food<Chef, Data>, Data>
{
    union
    {
        Data data = Data();
        Trivial_Alias data_alias;
    };

    Weak_food() = default;
    Weak_food(Data n_data) : data(n_data)
        {}
    T_Data_Alias release_data() override
        { assert(false); return 0; }
    void clear_data() override
        { assert(false); }
};
template <typename Chef, Data_Dynamic Data>
struct Weak_food<Chef, Data> : public Tinterface_Impl<Chef, Weak_food<Chef, Data>, Data>
{
    Data * const data = nullptr;
    const Data * data_get() const
        { return data; }

    Weak_food() = default;
    Weak_food(Data * n_data) : data(n_data)
        {}
    T_Data_Alias release_data() override
        { assert(false); return 0; }
    void clear_data() override
        { assert(false); }
};

} // namespace ff

#endif // FF_WEAK_HPP
