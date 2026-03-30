#ifndef LRU_HPP
#define LRU_HPP

#include <iostream>
#include <string>
#include "../models/buffer.hpp"
#include "printEvict.hpp"


int evictLRU(Buffer &buffer){
    int slotEvict = 0;

    //Confere se o slot tá não tá livre, e pega o que possui o menor tempo (o mais antigo) 
    for(int i = 0; i < 5; i++){
        if (!buffer.paginasCarregadas[i].livre && buffer.tempo[i] < buffer.tempo[slotEvict]){
            slotEvict = i;
        }
    }

    //Caso não tenha entrado nenhuma vez no if, vai ser passado como padrao o 0, que é o primeiro do buffer pra comecar a preencher
    checaModificao(buffer, slotEvict);

    buffer.paginasCarregadas[slotEvict].livre = true;
    return slotEvict;



}

#endif