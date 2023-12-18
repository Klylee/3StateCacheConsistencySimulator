#include "State.h"
#include <vector>
#include <map>

typedef unsigned long long uint64;
typedef unsigned int uint32;

class Block
{
public:
    uint32 addr;
    uint64 data;
    State state;

    Block(uint32 addr, uint64 data, State s)
        : addr(addr), data(data), state(s) {}
};

class Device
{
public:
    std::map<uint32, Block> blocks;

    Device() = default;
    void addBlock(uint32 addr, uint64 data, State s)
    {
        blocks.insert(std::pair<uint32, Block>(addr, Block(addr, data, s)));
    }
    void write(uint32 addr, uint64 data) {/*do something*/}
    uint64 read(uint32 addr)
    {
        uint64 data = 0;
        return data;
        // auto b = blocks.find(addr);
        // if(b != blocks.end())
        //     return b->second.data;
    }
};

class Memory : public Device
{
public:
    Memory() = default;
};

class Cache : public Device
{
public:
    unsigned int id;
    Cache(unsigned int id = 0) : Device(), id(id) {}
};
