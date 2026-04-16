#pragma once

#include <string>

class Emulator {
private:
    std::string buff;
    int counter;

public:
    Emulator();
    ~Emulator();
    void init();
    void write(std::string buff);
    std::string read();
    void close();
};
