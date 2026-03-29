#ifndef PAGINA_HPP
#define PAGINA_HPP

#include <iostream>
#include <string>

using namespace std;


typedef struct Pagina{
    int page_id;
    string dados; 
    bool dirty = false;

}Pagina;


#endif