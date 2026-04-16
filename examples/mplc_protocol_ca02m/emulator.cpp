#include "emulator.h"


Emulator::Emulator() {
    counter = 0;
}

Emulator::~Emulator() {

}

void Emulator::init() {
    
}

void Emulator::write(std::string buff) {
    this->buff = buff + " " + std::to_string(counter++);
}

std::string Emulator::read() {
    return buff;
}

void Emulator::close() {

}
