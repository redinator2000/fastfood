#include "fastfood/unique_flat.hpp"
#include "fastfood/weak_const.hpp"
#include "fastfood/weak_mut.hpp"
#include <string>
#include <vector>

using Tile_ID = uint64_t;
constexpr Tile_ID tile_id_uninitialized = UINT64_MAX;
template <typename Chef, typename Data>
Tile_ID tile_id = tile_id_uninitialized;

Tile_ID register_tile_id_highest_used = 0;
template <typename Chef, typename Data>
void register_tile_id()
{
    assert((tile_id<Chef, Data> == tile_id_uninitialized));
    tile_id<Chef, Data> = register_tile_id_highest_used++;
}

struct Tracker {
    int id;

    Tracker(int id_) : id(id_) {
        std::printf("ctor        id=%d\n", id);
    }
    Tracker(const Tracker& other) : id(other.id) {
        std::printf("copy ctor   id=%d\n", id);
    }
    Tracker(Tracker&& other) noexcept : id(other.id) {
        std::printf("move ctor   id=%d\n", id);
    }
    Tracker& operator=(const Tracker& other) {
        std::printf("copy assign id=%d <- %d\n", id, other.id);
        id = other.id;
        return *this;
    }
    Tracker& operator=(Tracker&& other) noexcept {
        std::printf("move assign id=%d <- %d\n", id, other.id);
        id = other.id;
        return *this;
    }
    ~Tracker() {
        std::printf("dtor        id=%d\n", id);
    }

    bool operator==(const Tracker &) const = default;
};


#include <array>
namespace tiles
{
    struct Water
    {
        static const char * name() { return "water"; }
        bool operator==(const Water &) const = default;
    };
    struct Stone
    {
        static const char * name(){ return "stone"; }
        bool operator==(const Stone &) const = default;

        bool moss[8] = {0};
    };
    struct Switch
    {
        static const char * name() { return "switch"; }
        bool operator==(const Switch &) const = default;

        bool on = false;
    };
    struct Piano
    {
        static const char * name() { return "piano"; }
        bool operator==(const Piano &) const = default;

        std::array<float, 20> keys = {0};
    };
    struct Sign
    {
        static const char * name() { return "sign"; }
        bool operator==(const Sign &) const = default;

        std::string text;
    };
    struct Tracker_Toy
    {
        static const char * name() { return "tracker_toy"; }
        bool operator==(const Tracker_Toy &) const = default;

        Tracker tracker;
    };
}

struct My_Chef
{
    struct Interface_const : public ff::Chef_Base_const<My_Chef>
    {
        virtual Tile_ID id_get() const = 0;
        virtual const char * name() const = 0;
        virtual void print_data() const = 0;
    };
    struct Interface_mut : public ff::Chef_Base_mut<Interface_const>
    {
        virtual void manipulate() = 0;
    };
    template <typename Has_get_data, typename Data>
    struct Implement_const : public Has_get_data
    {
        Tile_ID id_get() const override
        {
            assert((tile_id<My_Chef, Data> != tile_id_uninitialized));
            return tile_id<My_Chef, Data>;
        }
        const char * name() const override
        {
            return Data::name();
        }
        void print_data() const override
        {
            if constexpr (ff::Data_Empty<Data>)
            {
                printf("empty data\n");
                return;
            }
            const Data * data = this->get_data();
            if(!data)
            {
                printf("null data\n");
                return;
            }
            if constexpr (std::is_same_v<Data, tiles::Stone>)
            {
                printf("moss:%d %d %d %d %d %d %d %d\n", data->moss[0], data->moss[1], data->moss[2], data->moss[3], data->moss[4], data->moss[5], data->moss[6], data->moss[7]);
            }
            else if constexpr (std::is_same_v<Data, tiles::Switch>)
            {
                printf("on:%d\n", data->on);
            }
            else if constexpr (std::is_same_v<Data, tiles::Piano>)
            {
                printf("key:%f\n", data->keys[0]);
            }
            else if constexpr (std::is_same_v<Data, tiles::Sign>)
            {
                printf("text:%s\n", data->text.c_str());
            }
            else
            {
                printf("print_data not implemented for %s\n", name());
            }
        }
    };
    template <typename Has_get_data, typename Data>
    struct Implement_mut : public Implement_const<Has_get_data, Data>
    {
        void manipulate() override
        {
            Data * data = this->get_data_mut();
            if(!data)
                return;
            if constexpr (std::is_same_v<Data, tiles::Switch>)
            {
                data->on = !data->on;
            }
            else if constexpr (std::is_same_v<Data, tiles::Stone>)
            {
                data->moss[0] = !data->moss[0];
            }
            else if constexpr (std::is_same_v<Data, tiles::Piano>)
            {
                data->keys[0] += 0.125f;
            }
            else if constexpr (std::is_same_v<Data, tiles::Sign>)
            {
                data->text += "-manipluated";
            }
        }
    };
};

template <typename Chef, typename Data, typename... Args>
bool blender_test(Args&&... args)
{
    printf("testing id:%ld name:%s\n", tile_id<Chef, Data>, Data::name());
    ff::Unique_food<Chef, Data> sign_strong = ff::make_Unique_food<Chef, Data>(std::forward<Args>(args)...);
    ff::Unique_food<Chef, Data> sign_moved = std::move(sign_strong);
    ff::Weak_const_food<Chef, Data> sign_weak = sign_moved.as_Weak_const_food();
    ff::Unique_flat<Chef> flat = ff::Unique_flat<Chef>(std::move(sign_moved));
    const My_Chef::Interface_const * itw = ff::as_interface(&flat);
    [[maybe_unused]] Data * casted_data_p = ff::flat_cast<Data>(&flat);
    [[maybe_unused]] Data & casted_data_r = ff::flat_cast<Data>(flat);
    printf("flat  id:%ld name:%s\n", itw->id_get(), itw->name());
    printf("original   data: ");
    itw->print_data();
    My_Chef::Interface_mut * itm = ff::as_interface(&flat);
    printf("manipluating through Interface_mut\n");
    itm->manipulate();
    printf("new        data: ");
    itm->print_data();
    printf("Weak_const_food: "); // Weak_const_food might implement data as a copy OR a pointer to the original data, so it may or may have not changed. It is not safe to manipulate the food Weak_const_food is referencing
    sign_weak.print_data();
    ff::Weak_mut_food<Chef, Data> wmf = ff::as_interface<Chef, Data>(&flat);
    printf("manipulating through Weak_mut_food\n");
    wmf.manipulate();
    printf("mainipulated   : ");
    wmf.print_data();
    printf("\n");
    return true;
}

bool vector_test()
{
    printf("vector test\n");
    std::vector<ff::Unique_flat<My_Chef>> vec = {};
    vec.emplace_back(ff::make_Unique_food<My_Chef, tiles::Water>());
    vec.emplace_back(ff::make_Unique_food<My_Chef, tiles::Stone>());
    vec.emplace_back(ff::make_Unique_food<My_Chef, tiles::Switch>(true));
    vec.emplace_back(ff::make_Unique_food<My_Chef, tiles::Piano>());
    vec.emplace_back(ff::make_Unique_food<My_Chef, tiles::Sign>("hello vec"));
    vec.emplace_back(ff::make_Unique_food<My_Chef, tiles::Tracker_Toy>(5));

    for(auto & e : vec)
    {
        My_Chef::Interface_mut * itm = ff::as_interface(&e);
        itm->print_data();
        itm->manipulate();
        itm->print_data();
    }
    for(auto & e : vec)
    {
        My_Chef::Interface_const * itm = ff::as_interface(&e);
        ff::Unique_flat<My_Chef> c = itm->clone();
        My_Chef::Interface_mut * ci = ff::as_interface(&c);
        assert(itm->equals_food(ci));
        if(c.has_alternative<tiles::Switch>())
        {
            ff::flat_cast<tiles::Switch>(c).on = !ff::flat_cast<tiles::Switch>(c).on;
            assert(!itm->equals_food(ci));
        }
    }

    return true;
}

void example()
{
    printf("starting\n");

    register_tile_id<My_Chef, tiles::Water>();
    register_tile_id<My_Chef, tiles::Stone>();
    register_tile_id<My_Chef, tiles::Switch>();
    register_tile_id<My_Chef, tiles::Piano>();
    register_tile_id<My_Chef, tiles::Sign>();
    register_tile_id<My_Chef, tiles::Tracker_Toy>();

    assert((blender_test<My_Chef, tiles::Water>()));
    assert((blender_test<My_Chef, tiles::Stone>()));
    assert((blender_test<My_Chef, tiles::Switch>(true)));
    assert((blender_test<My_Chef, tiles::Piano>()));
    assert((blender_test<My_Chef, tiles::Sign>("hello blender")));
    assert((blender_test<My_Chef, tiles::Tracker_Toy>(4)));

    assert(vector_test());

    printf("ok!\n");
}

#ifdef FASTFOOD_RUN_EXAMPLE
int main()
{
    example();
}
#endif // FASTFOOD_RUN_EXAMPLE

