#ifndef FF_UNIQUE_HPP
#define FF_UNIQUE_HPP

#include "food.hpp"

namespace ff
{

template <typename Tgeneral, typename Data>
struct Unique_food;

template <typename Tgeneral, Data_Empty Data>
struct Unique_food<Tgeneral, Data> : public Tinterface_Impl<Tgeneral, Unique_food<Tgeneral, Data>, Data>
{
    Unique_food() = default;
    explicit Unique_food(T_Data_Alias)
    {
        static_assert(sizeof(Unique_food<Tgeneral, Data>) == 1 * sizeof(void*));
    }
    Weak_food<Tgeneral, Data> as_Weak_food()
    { return Weak_food<Tgeneral, Data>(); }
    T_Data_Alias release_data() override
        { return 0; }
    void clear_data() override
        {}
    Unique_food(const Unique_food & other) = delete;
    Unique_food & operator=(const Unique_food & other) = delete;
    Unique_food(Unique_food && other) noexcept = default;
    Unique_food& operator=(Unique_food && other) noexcept = default;
};
template <typename Tgeneral, Data_Empty Data, typename... Args>
Unique_food<Tgeneral, Data> make_Unique_food(Args&&... args)
{
    return Unique_food<Tgeneral, Data>();
}

template <typename Tgeneral, Data_Trivial Data>
struct Unique_food<Tgeneral, Data> : public Tinterface_Impl<Tgeneral, Unique_food<Tgeneral, Data>, Data>
{
    union
    {
        Data data = Data();
        Trivial_Alias data_alias;
    };
    Unique_food() = default;
    explicit Unique_food(T_Data_Alias da) :
    data_alias(da)
    {
        static_assert(sizeof(Unique_food<Tgeneral, Data>) == 2 * sizeof(void*));
    }
    T_Data_Alias release_data() override
        { return data_alias; }
    void clear_data() override
        { data_alias = 0; }
    Unique_food(const Unique_food & other) = delete;
    Unique_food & operator=(const Unique_food & other) = delete;
    Unique_food(Unique_food && other) noexcept = default;
    Unique_food& operator=(Unique_food && other) noexcept = default;

    Weak_food<Tgeneral, Data> as_Weak_food()
        { return Weak_food<Tgeneral, Data>(data); }
};
template <typename Tgeneral, Data_Trivial Data, typename... Args>
Unique_food<Tgeneral, Data> make_Unique_food(Args&&... args)
{
    Unique_food<Tgeneral, Data> ts;
    ts.data = Data(std::forward<Args>(args)...);
    return ts;
}

template <typename Tgeneral, Data_Dynamic Data>
struct Unique_food<Tgeneral, Data> : public Tinterface_Impl<Tgeneral, Unique_food<Tgeneral, Data>, Data>
{
    std::unique_ptr<Data> data = nullptr;
    const Data * data_get() const
        { return data.get(); }

    T_Data_Alias release_data() override
        { return reinterpret_cast<T_Data_Alias>(data.release()); }
    void clear_data() override
        { data.reset(); }

    Weak_food<Tgeneral, Data> as_Weak_food()
        { return Weak_food<Tgeneral, Data>(this->data.get()); }

    Unique_food() = default;
    explicit Unique_food(Data* ptr) : data(ptr) {}
    explicit Unique_food(T_Data_Alias da) :
    data(reinterpret_cast<Data *>(da))
    {
        static_assert(sizeof(Unique_food<Tgeneral, Data>) == 2 * sizeof(void*));
    }
    Unique_food(std::unique_ptr<Data> && n_data) :
    data(std::move(n_data))
        {}
    Unique_food(const Unique_food & other) = delete;
    Unique_food & operator=(const Unique_food & other) = delete;
    Unique_food(Unique_food && other) noexcept = default;
    Unique_food& operator=(Unique_food && other) noexcept = default;
};
template <typename Tgeneral, Data_Dynamic Data, typename... Args>
Unique_food<Tgeneral, Data> make_Unique_food(Args&&... args)
{
    return Unique_food<Tgeneral, Data>(
        std::make_unique<Data>(std::forward<Args>(args)...)
    );
}
}; // namespace ff

#endif // FF_UNIQUE_HPP
