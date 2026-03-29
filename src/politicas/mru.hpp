#ifndef MRU_HPP
#define MRU_HPP
#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

#include <iostream>
#include <string>
#include "../models/buffer.hpp"


int evictMRU(Buffer &buffer){
    int slotEvict = 0;

    for(int i = 1; i < 5; i++){
        if (buffer.tempo[i] > buffer.tempo[slotEvict]){
            slotEvict = i;
        }
    }

    //Aleatorizador para o dirtybit que vai sair
    if(rand() % 2 == 0){
        buffer.paginasCarregadas[slotEvict].p.dirty = true;
    }

    //Se tiver sido modificado, printa o W
    if(buffer.paginasCarregadas[slotEvict].p.dirty){
        cout << endl << BLUE << "W" << RESET<< endl ;
        cout << endl << "Pagina atualizada, escrevendo no disco..."<< endl << "Pressione ENTER para continuar" << endl;
        cin.get();
    }

    buffer.paginasCarregadas[slotEvict].livre = true;
    return slotEvict;



}

#endif