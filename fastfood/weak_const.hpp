#ifndef FF_WEAK_CONST_HPP
#define FF_WEAK_CONST_HPP

#include "weak_mut.hpp"

namespace ff
{

template <typename Chef, Data_Empty Data>
struct Weak_const_food<Chef, Data> : public impl::Container_Parent_const<Chef, Data>
{
    const Data * get_data() const override
        { return nullptr; }
    Weak_const_food()
        {}
    explicit Weak_const_food(const Unique_food<Chef, Data> &) :
        Weak_const_food()
        {}
    explicit Weak_const_food(const Unique_flat<Chef> & uf) :
        Weak_const_food()
    {
        assert(uf.template has_alternative<Data>());
    }
    explicit Weak_const_food(const Weak_mut_food<Chef, Data> &)
        {}
};
template <typename Chef, Data_Trivial Data>
struct Weak_const_food<Chef, Data> : public impl::Container_Parent_const<Chef, Data>
{
    const Data data = Data();

    const Data * get_data() const override
        { return &data; }

    Weak_const_food(Data n_data) : data(n_data)
        {}
    explicit Weak_const_food(const Unique_food<Chef, Data> & uf) :
        Weak_const_food(uf.data)
        {}
    explicit Weak_const_food(const Unique_flat<Chef> & uf) :
        Weak_const_food(flat_cast<Data>(uf))
        {}
    explicit Weak_const_food(const Weak_mut_food<Chef, Data> & uf) :
        Weak_const_food(*uf.get_data())
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
    explicit Weak_const_food(const Unique_food<Chef, Data> & uf) :
        Weak_const_food(uf.data.get())
        {}
    explicit Weak_const_food(const Unique_flat<Chef> & uf) :
        Weak_const_food(flat_cast<Data>(&uf))
        {}
    explicit Weak_const_food(const Weak_mut_food<Chef, Data> & uf) :
        Weak_const_food(uf.get_data())
        {}
};

} // namespace ff

#endif // FF_WEAK_CONST_HPP
