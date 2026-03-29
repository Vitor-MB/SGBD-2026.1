#ifndef CLOCK_HPP
#define CLOCK_HPP


#include <iostream>
#include <string>
#include "../models/buffer.hpp"
#include "printEvict.hpp"

int evictCLOCK(Buffer &buffer){
     while(true){
        if(!buffer.refBit[buffer.posFila]){
            int slotEvict = buffer.posFila;

            checaModificao(buffer, slotEvict);


            buffer.paginasCarregadas[slotEvict].livre = true;
            buffer.posFila = (buffer.posFila + 1) % 5;

            return slotEvict;
        }



        buffer.refBit[buffer.posFila] = false;
        buffer.posFila = (buffer.posFila + 1) % 5;
    }
}

#endif

