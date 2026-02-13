#ifndef FF_FOOD_HPP
#define FF_FOOD_HPP

#include <cstdlib>
#include <string>
#include <vector>
#include <variant>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <cassert>
#include <vector>

namespace ff
{

using Trivial_Alias = intptr_t;

template <typename Data>
concept Data_Empty =
    std::is_empty_v<Data>;
template <typename Data>
concept Data_Trivial =
    (!Data_Empty<Data>
    && sizeof(Data) <= sizeof(Trivial_Alias)
    && std::is_trivially_copyable_v<Data>);
template <typename Data>
concept Data_Dynamic =
    (!Data_Empty<Data> && !Data_Trivial<Data>);

using T_Data_Alias = Trivial_Alias;

struct Food_Interface
{
    virtual bool equals_Tinterface(const Food_Interface *) const = 0;

    virtual T_Data_Alias release_data() = 0; // only use on Unique_food children of Tinterface
    virtual void clear_data() = 0; // only use on Unique_food children of Tinterface
};

template <typename Tgeneral, typename Data>
struct Weak_food;

template <typename Tgeneral, typename Inheriting, typename Data>
struct Tinterface_Impl : public Tgeneral::template Implement<Data>
{
    bool equals_Tinterface(const Food_Interface * other_tw) const override
    {
        if(!other_tw)
            return false;
        if(other_tw == this)
            return true;
        if(const Tinterface_Impl<Tgeneral, Inheriting, Data> * other = dynamic_cast<const Tinterface_Impl<Tgeneral, Inheriting, Data> *>(other_tw))
        {
            if constexpr (Data_Empty<Data>)
                return true;
            else if constexpr (Data_Trivial<Data>)
                return dynamic_cast<const Inheriting*>(this)->data_alias == dynamic_cast<const Inheriting*>(other)->data_alias;
            else
            {
                static_assert(Data_Dynamic<Data>);
                const Data * td = dynamic_cast<const Inheriting*>(this)->data_get();
                const Data * od = dynamic_cast<const Inheriting*>(other)->data_get();
                if(td == od)
                    return true;
                if(bool(td) != bool(od))
                    return false;
                return *td == *od;
            }
        }
        else
            return false;
    }
};

namespace impl
{
    template <typename T>
    void * vtable_ripper()
    {
        static void * ripped = nullptr;
        if(!ripped)
        {
            T temp{};
            void ** raw = reinterpret_cast<void**>(&temp);
            ripped = *raw;
        }
        return ripped;
    }
} // namespace ipml

/*
struct Weak_food_Vtable { virtual void dont_instantiate() = 0; };
template <typename Tgeneral, typename Data>
Weak_food_Vtable * tweak_vtable_ripper()
{
    return static_cast<Weak_food_Vtable *>(vtable_ripper<Weak_food<Tgeneral, Data>>());
}
*/

}; // namespace ff

#endif // FF_FOOD_HPP
