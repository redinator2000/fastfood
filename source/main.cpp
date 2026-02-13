#include "unique_flat.hpp"
#include "weak.hpp"

// chunk/id stuff

template <typename Chef>
struct Chunk
{
private:
    std::vector<uint32_t> aliases; //could be a variant with every type of unsigned

    std::vector<ff::Unique_Flat<Chef>> flattened; //these tweaks are owning

    uint64_t find_or_insert_alias(ff::Unique_Flat<Chef> tsf)
    {
        const Chef * itw = ff::reference_Unique_Flat_to_Food_Interface(&tsf);
        for(uint64_t a = 0; a < flattened.size(); a++)
            if(itw->equals_Tinterface(ff::reference_Unique_Flat_to_Food_Interface(&flattened[a])))
                return a;
        flattened.push_back(std::move(tsf));
        return flattened.size() - 1;
    }
public:
    void resize(size_t s)
    {
        aliases.resize(s);
    }

    const Chef * get_IWeak_food(size_t index) const // resizing will invalidate these
    {
        return reference_Unique_Flat_to_Chef(&flattened[aliases[index]]);
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
template <typename Data>
void register_tile_id()
{
    assert((tile_id<Chef, Data> == tile_id_uninitialized));
    tile_id<Chef, Data> = highest_used++;
}

// userspace i guess

template <typename Data>
struct My_Chef_Implement;

struct My_Chef : public ff::Food_Interface
{
    virtual Tile_ID id_get() const = 0;

    //below must be user implemented
    virtual const char * name() const = 0;

    template <typename Data>
    using Implement = My_Chef_Implement<Data>;
};

template <typename Chef, typename Data>
struct Chef_Implment : public Chef
{
    virtual const Data * get_data() const = 0;
    virtual Data * get_data() = 0;
}

template <typename Data>
struct My_Chef_Implement : public My_Chef
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
};

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

template <typename Chef, typename Data, typename... Args>
bool blender_test(Args&&... args)
{
    printf("testing id:%ld name:%s\n", tile_id<Chef, Data>, Data::name());
    ff::Unique_food<Chef, Data> sign_strong = ff::make_Unique_food<Chef, Data>(std::forward<Args>(args)...);
    ff::Unique_food<Chef, Data> sign_moved = std::move(sign_strong);
    ff::Weak_food<Chef, Data> sign_weak = sign_moved.as_Weak_food();
    ff::Unique_Flat<Chef> flat = ff::Unique_Flat<Chef>(std::move(sign_moved));
    const IUser * itw = ff::reference_Unique_Flat_to_Food_Interface(&flat);
    printf("flat  id:%ld name:%s\n", itw->id_get(), itw->name());
    return true;
}

int main()
{
    ID_map<IUser> id_map;

    id_map.register_tile_id<tiles::Water>();
    id_map.register_tile_id<tiles::Stone>();
    id_map.register_tile_id<tiles::Switch>();
    id_map.register_tile_id<tiles::Piano>();
    id_map.register_tile_id<tiles::Sign>();
    id_map.register_tile_id<tiles::Tracker_Toy>();

    assert((blender_test<IUser, tiles::Water>()));
    assert((blender_test<IUser, tiles::Stone>()));
    assert((blender_test<IUser, tiles::Switch>()));
    assert((blender_test<IUser, tiles::Piano>()));
    assert((blender_test<IUser, tiles::Sign>()));
    assert((blender_test<IUser, tiles::Tracker_Toy>(4)));

    printf("ok!\n");
}
