#ifndef MRU_HPP
#define MRU_HPP


#include <iostream>
#include <string>
#include "../models/buffer.hpp"
#include "printEvict.hpp"


int evictMRU(Buffer &buffer){
    int slotEvict = 0;

    //Confere se o slot tá não tá livre, e pega o que possui o maior tempo (o mais recente) 
    for(int i = 0; i < 5; i++){
        if (!buffer.paginasCarregadas[i].livre && buffer.tempo[i] > buffer.tempo[slotEvict]){
            slotEvict = i;
        }
    }
    
    checaModificao(buffer, slotEvict);

    buffer.paginasCarregadas[slotEvict].livre = true;
    return slotEvict;



}

#endif