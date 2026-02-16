#ifndef FF_WEAK_MUT_HPP
#define FF_WEAK_MUT_HPP

#include "unique_flat.hpp"

namespace ff
{

template <typename Chef, non_polymorphic Data>
class Weak_mut_food : public impl::Container_Parent_mut<Chef, Data>
{
private:
    Data * data;
public:

    Weak_mut_food(Data * n_data) :
        data(n_data)
        {}
    Weak_mut_food(Unique_food<Chef, Data> & uf) :
        data(uf.get_data_mut())
        {}
    Weak_mut_food(Unique_food<Chef, Data> * uf) :
        data(uf ? uf->get_data_mut() : nullptr)
        {}
    Weak_mut_food(Unique_flat<Chef> & uf) :
        data(flat_cast<Data>(&uf))
        {}
    Weak_mut_food(Unique_flat<Chef> * uf) :
        data(flat_cast<Data>(uf))
        {}
    const Data * get_data() const override
        { return data; }
    Data * get_data_mut() override
        { return data; }
};

};

#endif // FF_WEAK_MUT_HPP
