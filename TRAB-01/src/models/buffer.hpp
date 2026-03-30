
#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <iostream>
#include <string>
#include "slot.hpp"
#include <queue>


typedef struct Buffer{
    Slot paginasCarregadas[5];
    queue<int> indices; //fifo

    int tempo[5];        // LRU/MRU
    int contadorTempo = 0;

    bool refBit[5];    //CLOCK
    int posFila = 0;


}Buffer;



#endif