#ifndef FIFO_HPP
#define FIFO_HPP

#include <iostream>
#include <string>
#include "../models/buffer.hpp"


int evictFIFO(Buffer &buffer) {
    int slotEvict = buffer.posFila;
    buffer.posFila = (buffer.posFila + 1) % 5;

    checaModificao(buffer, slotEvict);

    buffer.paginasCarregadas[slotEvict].livre = true;

    return slotEvict;
}

#endif