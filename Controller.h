#include "Device.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

// the number of CPUs
constexpr static int N = 4;

class Controller
{
public:
    Memory memory;
    Cache caches[N];
    std::ifstream traces[N];

    uint64 writeBack(uint32 addr);
    uint64 readFromMemory(uint32 addr);
    void cacheRead(unsigned int id, uint32 addr);
    void cacheWrite(unsigned int id, uint32 addr, uint64 data);
    void printState(uint32 addr);
    void mainLoop();
    Controller();
    ~Controller();
};

uint64 Controller::writeBack(uint32 addr)
{
    for (int i = 0; i < N;i++){
        auto b = caches[i].blocks.find(addr);
        if(b->second.state == State::M){
            memory.write(addr, b->second.data);
            return b->second.data;
        }
    }
    return 0;
}

uint64 Controller::readFromMemory(uint32 addr)
{
    auto b = memory.blocks.find(addr);

    // memory missed
    if(b == memory.blocks.end()){
        memory.addBlock(addr, 1, State::S);
        return 1;
    }
    //the data in memory is invalid
    if(b->second.state == State::I){
        uint64 data = writeBack(addr);
        b->second.state = State::S;
        return data;
    }
    return b->second.data;
}

void Controller::cacheRead(unsigned int id, uint32 addr)
{
    // RI
    auto b = caches[id].blocks.find(addr);
    // cache missed
    if(b == caches[id].blocks.end()){
        uint64 data = readFromMemory(addr);
        caches[id].addBlock(addr, data, State::S);
    }
    else{
        if(b->second.state == State::S || b->second.state == State::M){
        }
        else{
            uint64 data = readFromMemory(addr);
            b->second.data = data;
            b->second.state = State::S;
        }
    }

    // RR
    for (int i = 0; i < N;i++){
        if(i != id){
            auto c = caches[i].blocks.find(addr);
            if(c == caches->blocks.end())
                continue;
            if(c->second.state == State::M)
                c->second.state = State::S;
        }
    }
}

void Controller::cacheWrite(unsigned int id, uint32 addr, uint64 data)
{
    for (int i = 0; i < N;i++){
        // WI
        if(i == id){
            auto b = caches[i].blocks.find(addr);
            if(b == caches[i].blocks.end()){
                caches[i].addBlock(addr, data, State::M);
                continue;
            }
            b->second.data = data;
            b->second.state = State::M;
        }
        // WR
        else {
            auto b = caches[i].blocks.find(addr);
            if(b == caches[i].blocks.end()){
                continue;
            }
            b->second.state = State::I;
        }
    }
    auto b = memory.blocks.find(addr);
    if(b == memory.blocks.end()){
        return;
    }
    b->second.state = State::I;
}

void Controller::printState(uint32 addr)
{
    std::cout << "       ";
    for (int i = 0; i < N; i++){
        std::cout << std::setfill(' ') << std::setw(5) << "C" + std::to_string(i);
    }
    std::cout << std::setfill(' ') << std::setw(5) << "M" << std::endl;

    std::cout << "state  ";
    for (int i = 0; i < N;i++){
        auto b = caches[i].blocks.find(addr);
        if(b == caches[i].blocks.end())
            std::cout << std::setfill(' ') << std::setw(5) << "/";
        else 
            std::cout << std::setfill(' ') << std::setw(5) << getStateChar(b->second.state);
    }
    auto b = memory.blocks.find(addr);
    if(b == memory.blocks.end())
        std::cout << std::setfill(' ') << std::setw(5) << "/";
    else
        std::cout << std::setfill(' ') << std::setw(5) << getStateChar(b->second.state) << std::endl;
}

static std::string MODSTR[2] = {"READ", "WRITE"};
void Controller::mainLoop()
{
    int id;
    while(1){
        std::cout << "cache: ";
        std::cin >> id;

        if(traces[id].peek() == EOF){
            std::cout << "trace" + std::to_string(id) + ".txt end" << std::endl;
            continue;
        }
        unsigned int mod, addr;
        traces[id] >> mod >> std::hex >> addr;
        std::cout << "[Action] " << "cache" <<id << " " << MODSTR[mod] << " " 
                  << std::setfill('0') << std::setw(8) << std::hex << addr << std::endl;
        
        // read
        if(mod == 0){
            cacheRead(id, addr);
        }
        // write
        else if(mod == 1){
            cacheWrite(id, addr, 2);
        }
        
        printState(addr);
    }
}

Controller::Controller()
{
    for (int i = 0; i < N;i++){
        caches[i].id = i;

        traces[i].open("./trace" + std::to_string(i) + ".txt");
        if(!traces[i].is_open()){
            std::cout << "trace" + std::to_string(i) + ".txt no found" << std::endl;
            return;
        }
    }
}

Controller::~Controller()
{
}
