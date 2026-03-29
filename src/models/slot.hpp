#ifndef SLOT_HPP
#define SLOT_HPP

#include <iostream>
#include <string>
#include "pagina.hpp"


using namespace std;


typedef struct Slot{
    Pagina p;
    bool livre = true;
}Slot;



#endif