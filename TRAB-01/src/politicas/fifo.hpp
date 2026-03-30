#ifndef FIFO_HPP
#define FIFO_HPP

#include <iostream>
#include <string>
#include "../models/buffer.hpp"
#include "printEvict.hpp"


int evictFIFO(Buffer &buffer) {
    int slotEvict = buffer.posFila;

    //Transforma em algo circular, que vai de 0 -> 4 
    buffer.posFila = (buffer.posFila + 1) % 5;

    checaModificao(buffer, slotEvict);

    buffer.paginasCarregadas[slotEvict].livre = true;

    return slotEvict;
}

#endif