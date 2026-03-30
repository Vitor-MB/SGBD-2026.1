
#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <iostream>
#include <string>
#include "slot.hpp"


typedef struct Buffer{
    Slot paginasCarregadas[5];
    int posFila = 0;  // FIFO

    int tempo[5];        // LRU/MRU
    int contadorTempo = 0;

    bool refBit[5];    //CLOCK

}Buffer;



#endif