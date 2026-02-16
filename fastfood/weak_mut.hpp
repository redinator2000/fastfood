#ifndef FF_WEAK_MUT_HPP
#define FF_WEAK_MUT_HPP

#include "unique_flat.hpp"

namespace ff
{

template <typename Chef, typename Data>
class Weak_mut_food : public impl::Container_Parent_mut<Chef, Data>
{
private:
    Data * data;
public:

    Weak_mut_food(Data * n_data) :
        data(n_data)
        {}
    const Data * get_data() const override
        { return data; }
    Data * get_data_mut() override
        { return data; }
};
template <typename Chef, typename Data>
Weak_mut_food<Chef, Data> as_interface(Unique_food<Chef, Data> * uf)
{
    return Weak_mut_food<Chef, Data>(flat_cast<Data>(uf));
}
template <typename Chef, typename Data>
Weak_mut_food<Chef, Data> as_interface(Unique_flat<Chef> * uf)
{
    assert(uf->template has_alternative<Data>());
    return Weak_mut_food<Chef, Data>(flat_cast<Data>(uf));
}

};

#endif // FF_WEAK_MUT_HPP
