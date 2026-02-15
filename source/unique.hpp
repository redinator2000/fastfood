#ifndef FF_UNIQUE_HPP
#define FF_UNIQUE_HPP

#include "chef.hpp"

namespace ff
{

template <typename Chef, typename Data>
struct Unique_food;

template <typename Chef, Data_Empty Data>
struct Unique_food<Chef, Data> : public impl::Container_Parent_own<Chef, Data>
{
    const Data * get_data() const override
        { return nullptr; }
    Data * get_data_mut() override
        { return nullptr; }


    Unique_food()
    {
        static_assert(sizeof(Unique_food<Chef, Data>) == 1 * sizeof(void*));
    }
    Weak_const_food<Chef, Data> as_Weak_const_food()
        { return Weak_const_food<Chef, Data>(); }
    void delete_data() override
        {}
    Unique_food(const Unique_food & other) = delete;
    Unique_food & operator=(const Unique_food & other) = delete;
    Unique_food(Unique_food && other) noexcept = default;
    Unique_food& operator=(Unique_food && other) noexcept = default;
};

template <typename Chef, Data_Trivial Data>
struct Unique_food<Chef, Data> : public impl::Container_Parent_own<Chef, Data>
{
    union
    {
        Data data = Data();
        Trivial_Alias data_alias;
    };
    const Data * get_data() const override
        { return &data; }
    Data * get_data_mut() override
        { return &data; }

    Unique_food() = default;
    explicit Unique_food(Data n_data) :
        data(n_data)
        {}
    explicit Unique_food(Data_Alias da) :
        data_alias(da)
    {
        static_assert(sizeof(Unique_food<Chef, Data>) == 2 * sizeof(void*));
    }
    void delete_data() override
        { data_alias = 0; }

    Weak_const_food<Chef, Data> as_Weak_const_food()
        { return Weak_const_food<Chef, Data>(data); }

    Unique_food(const Unique_food & other) = delete;
    Unique_food & operator=(const Unique_food & other) = delete;
    Unique_food(Unique_food && other) noexcept = default;
    Unique_food& operator=(Unique_food && other) noexcept = default;
};

template <typename Chef, Data_Dynamic Data>
struct Unique_food<Chef, Data> : public impl::Container_Parent_own<Chef, Data>
{
    std::unique_ptr<Data> data = nullptr;
    const Data * get_data() const override
        { return data.get(); }
    Data * get_data_mut() override
        { return data.get(); }

    void delete_data() override
        { data.reset(); }

    Weak_const_food<Chef, Data> as_Weak_const_food()
        { return Weak_const_food<Chef, Data>(this->data.get()); }

    Unique_food() = default;
    Unique_food(std::unique_ptr<Data> && n_data) :
        data(std::move(n_data))
        {}
    Unique_food(const Unique_food & other) = delete;
    Unique_food & operator=(const Unique_food & other) = delete;
    Unique_food(Unique_food && other) noexcept = default;
    Unique_food& operator=(Unique_food && other) noexcept = default;
};

template <typename Chef, Data_Empty Data, typename... Args>
Unique_food<Chef, Data> make_Unique_food(Args&&... args)
{
    return Unique_food<Chef, Data>();
}
template <typename Chef, Data_Trivial Data, typename... Args>
Unique_food<Chef, Data> make_Unique_food(Args&&... args)
{
    return Unique_food<Chef, Data>(
        Data(std::forward<Args>(args)...)
    );
}
template <typename Chef, Data_Dynamic Data, typename... Args>
Unique_food<Chef, Data> make_Unique_food(Args&&... args)
{
    return Unique_food<Chef, Data>(
        std::make_unique<Data>(std::forward<Args>(args)...)
    );
}

}; // namespace ff

#endif // FF_UNIQUE_HPP
