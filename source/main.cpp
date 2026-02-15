#include "unique_flat.hpp"
#include "weak_const.hpp"
#include "weak_mut.hpp"
#include <string>
#include <vector>

// chunk/id stuff

template <typename Chef>
struct Chunk
{
private:
    std::vector<uint32_t> aliases; //could be a variant with every type of unsigned

    std::vector<ff::Unique_Flat<Chef>> flattened; //these tweaks are owning

    uint64_t find_or_insert_alias(ff::Unique_Flat<Chef> tsf)
    {
        const typename Chef::Interface_const * itw = ff::reference_Unique_Flat_to_Interface_const(&tsf);
        for(uint64_t a = 0; a < flattened.size(); a++)
            if(itw->equals_Tinterface(ff::reference_Unique_Flat_to_Interface_const(&flattened[a])))
                return a;
        flattened.push_back(std::move(tsf));
        return flattened.size() - 1;
    }
public:
    void resize(size_t s)
    {
        aliases.resize(s);
    }

    const typename Chef::Interface_const * get_IWeak_food(size_t index) const // resizing will invalidate these
    {
        return reference_Unique_Flat_to_Interface_const(&flattened[aliases[index]]);
    }
    void garbage_collect()
    {
        std::vector<uint32_t> usage_count{};
        usage_count.resize(flattened.size());
        for(const auto & a : aliases)
            usage_count[a]++;
        std::vector<uint32_t> alias_renames{}; //microoptimization: this could probably be the reused array of usage_count
        alias_renames.resize(flattened.size());
        uint32_t lowest_unused_new_a = 0;
        for(uint32_t old_a = 0; old_a < flattened.size(); old_a++)
        {
            if(usage_count[old_a])
            {
                uint32_t new_a = lowest_unused_new_a++;
                alias_renames[old_a] = new_a;
                assert(new_a <= old_a);
                flattened[new_a] = flattened[old_a];
            }
        }
        for(auto & a : aliases)
            a = alias_renames[a];
    }
};

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

// userspace i guess

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

template <typename Data>
struct My_Chef_Implement_Const;

struct My_Chef
{
    struct Interface_const : public ff::Chef_Base_const
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
            if constexpr (std::is_same_v<Data, tiles::Stone>)
            {
                data->moss[0] = !data->moss[0];
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
    [[maybe_unused]] ff::Weak_const_food<Chef, Data> sign_weak = sign_moved.as_Weak_const_food();
    ff::Unique_Flat<Chef> flat = ff::Unique_Flat<Chef>(std::move(sign_moved));
    const My_Chef::Interface_const * itw = ff::reference_Unique_Flat_to_Interface_const(&flat);
    printf("flat  id:%ld name:%s\n", itw->id_get(), itw->name());
    itw->print_data();
    My_Chef::Interface_mut * itwm = ff::reference_Unique_Flat_to_Interface_mut(&flat);
    itwm->manipulate();
    itwm->print_data();
    ff::Weak_mut_food<Chef, Data> wmf = ff::reference_Unique_Flat_to_Weak_mut_food<Chef, Data>(&flat);
    wmf.manipulate();
    wmf.print_data();
    return true;
}

bool vector_test()
{
    std::vector<ff::Unique_Flat<My_Chef>> vec = {};
    vec.emplace_back(ff::make_Unique_food<My_Chef, tiles::Water>());
    vec.emplace_back(ff::make_Unique_food<My_Chef, tiles::Stone>());
    vec.emplace_back(ff::make_Unique_food<My_Chef, tiles::Switch>(true));
    vec.emplace_back(ff::make_Unique_food<My_Chef, tiles::Piano>());
    vec.emplace_back(ff::make_Unique_food<My_Chef, tiles::Sign>("hello vec"));
    vec.emplace_back(ff::make_Unique_food<My_Chef, tiles::Tracker_Toy>(5));

    return true;
}

int main()
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
