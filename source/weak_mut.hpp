#ifndef FF_WEAK_MUT_HPP
#define FF_WEAK_MUT_HPP

#include "unique_flat.hpp"

namespace ff
{

template <typename Chef, typename Data>
class Weak_mut_food : public Chef_Base_Implement<Chef, Weak_mut_food<Chef, Data>, Data>
{
private:
    Data * data;
public:

    Weak_mut_food(Data * n_data) :
        data(n_data)
        {}
    const Data * get_data() const override
        { return &data; }
    Data * get_data() override
        { return &data; }

    T_Data_Alias release_data() override
        { assert(false); return 0; }
    void clear_data() override
        { assert(false); }
};
template <typename Chef, typename Data>
Weak_mut_food<Chef, Data> reference_Unique_food_to_Weak_mut_food(Unique_food<Chef, Data> * uf)
{
    return Weak_mut_food<Chef, Data>(uf->get_data());
}
template <typename Chef, typename Data>
Weak_mut_food<Chef, Data> reference_Unique_Flat_to_Weak_mut_food(Unique_Flat<Chef> * uf)
{
    return Weak_mut_food<Chef, Data>(uf->get_data());
}

};

#endif // FF_WEAK_MUT_HPP
