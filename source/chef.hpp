#ifndef FF_FOOD_HPP
#define FF_FOOD_HPP

#include <memory>
#include <cassert>

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

using Data_Alias = Trivial_Alias;

template <typename Chef>
class Unique_flat;

template <typename Chef>
struct Chef_Base_const
{
    virtual bool equals_food(const Chef_Base_const *) const = 0;
    virtual Unique_flat<Chef> clone() const = 0;
};
template <typename Chef_Interface_const>
struct Chef_Base_mut : public Chef_Interface_const
{
};
namespace impl
{
    template <typename Chef_Interface_mut>
    struct Chef_Base_own : public Chef_Interface_mut
    {
        virtual void delete_data() = 0;
    };

    template <typename Chef, typename Impl_Parent, typename Data>
    struct Chef_Implementer_const : public Impl_Parent
    {
        virtual const Data * get_data() const = 0;

        bool equals_food(const Chef_Base_const<Chef> * other_base) const override
        {
            if(other_base == this)
                return true;
            if(!other_base)
                return false;
            const Chef_Implementer_const * other = dynamic_cast<const Chef_Implementer_const *>(other_base);
            if(!other)
                return false;

            if constexpr (Data_Empty<Data>)
                return true;
            else if constexpr (Data_Trivial<Data>)
                return *this->get_data() == *other->get_data();
            else
            {
                static_assert(Data_Dynamic<Data>);
                const Data * td = this->get_data();
                const Data * od = other->get_data();
                if(td == od)
                    return true;
                if(bool(td) != bool(od))
                    return false;
                return *td == *od;
            }
        }
        Unique_flat<Chef> clone() const override;
    };
    template <typename Chef, typename Impl_Parent, typename Data>
    struct Chef_Implementer_mut : public Chef_Implementer_const<Chef, Impl_Parent, Data>
    {
        virtual Data * get_data_mut() = 0;
    };
    template <typename Chef, typename Impl_Parent, typename Data>
    struct Chef_Implementer_own : public Chef_Implementer_mut<Chef, Impl_Parent, Data>
    {
    };

    template <typename Chef, typename Data>
    using Container_Parent_const = typename Chef::Implement_const<Chef_Implementer_const<Chef, typename Chef::Interface_const, Data>, Data>;
    template <typename Chef, typename Data>
    using Container_Parent_mut = typename Chef::Implement_mut<Chef_Implementer_mut<Chef, typename Chef::Interface_mut, Data>, Data>;
    template <typename Chef, typename Data>
    using Container_Parent_own = typename Chef::Implement_mut<Chef_Implementer_own<Chef, Chef_Base_own<typename Chef::Interface_mut>, Data>, Data>;

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

template <typename Chef, typename Data>
struct Weak_const_food;

}; // namespace ff

#endif // FF_FOOD_HPP
