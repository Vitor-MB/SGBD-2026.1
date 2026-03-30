#ifndef FIFO_HPP
#define FIFO_HPP

#include <iostream>
#include <string>
#include "../models/buffer.hpp"
#include "printEvict.hpp"


int evictFIFO(Buffer &buffer) {
    int slotEvict = buffer.indices.front();

    //Transforma em algo circular, que vai de 0 -> 4 
    buffer.indices.pop();

    checaModificao(buffer, slotEvict);

    buffer.paginasCarregadas[slotEvict].livre = true;

    return slotEvict;
}

#endif