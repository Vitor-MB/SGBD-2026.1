#ifndef MRU_HPP
#define MRU_HPP


#include <iostream>
#include <string>
#include "../models/buffer.hpp"
#include "printEvict.hpp"


int evictMRU(Buffer &buffer){
    int slotEvict = 0;

    for(int i = 1; i < 5; i++){
        if (buffer.tempo[i] > buffer.tempo[slotEvict]){
            slotEvict = i;
        }
    }

    checaModificao(buffer, slotEvict);

    buffer.paginasCarregadas[slotEvict].livre = true;
    return slotEvict;



}

#endif