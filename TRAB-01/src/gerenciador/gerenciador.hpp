#include <vector>
#include <string>
#include <cstring>
#include <iostream>

#include <fstream>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"
#include "../models/buffer.hpp"
#include "../models/slot.hpp"
#include "../models/pagina.hpp"
#include "../politicas/clock.hpp"
#include "../politicas/fifo.hpp"
#include "../politicas/lru.hpp"
#include "../politicas/mru.hpp"



using namespace std;

//Essa é a classe responsável por gerenciar todo o nosso sistema
class Gerenciador{

private:
    Buffer buffer;
    string politica;
    int contadorId = 0;
    int cacheMiss = 0;
    int cacheHit = 0;
    vector<Pagina> disco;


public:
    
    Gerenciador(){
        cacheHit = 0;
        cacheMiss = 0;
        
        for(int i = 0; i < 5; i++){
            buffer.paginasCarregadas[i].livre = true;
        }
    }

    void definePolitica(string pol){
        this->politica = pol;
    }

    void IniciarPag(Pagina &p){
        //Coloca o page_id na pagina
        p.page_id = contadorId++;
    }

    void lerArquivo(string arquivo){
        //Funcao para ler o csv e colocar as paginas no disco
        cout << "Abrindo arquivo: " << arquivo << endl << endl;
        ifstream file(arquivo);
        string linha;

        if(!file.is_open()){
            throw runtime_error("Nao foi possivel abrir o arquivo");
        }

        cout << "Lendo arquivo";
        while(getline(file, linha)){
            Pagina p ;
            IniciarPag(p);
            cout << "." ;

            linha.pop_back(); //remover a virgula
            p.dados = linha;
            disco.push_back(p);

        }

        cout << endl << "Disco inciado com " << disco.size() << " Paginas." << endl;
    }

    void printarDisco(){
        //Funcao para ver como está o Disco
        for(size_t i = 0; i < disco.size(); i++){
            Pagina &p = disco[i];

            cout << "Pagina " << p.page_id << ": " << endl;

            cout <<"conteudo: " << p.dados << endl << endl;
        }
    }

    int contarSlotsLivres(){
        int count = 0;
        for(int i = 0; i < 5; i++){
            if(this->buffer.paginasCarregadas[i].livre){
                count++;
            }
        }
        return count;
    }

    void displayStats(){
        cout << "Cache Hit: " << cacheHit << endl;
        cout << "Cache Miss: " << cacheMiss << endl;
    }

    int Evict(string politica){
        
        //Se todos os slots estiverem livres, retorna o 0 já que é o slot 1
        if(contarSlotsLivres() == 5){
            return 0;
        }

        int slotEvict = 0;


        if (politica=="FIFO") {  //politica de substituição: FIFO 
            slotEvict = evictFIFO(buffer);
        }
        if (politica=="LRU"){
            slotEvict = evictLRU(buffer);
        }
        if (politica=="MRU"){
            slotEvict = evictMRU(buffer);
        }
        if (politica=="CLOCK"){
            slotEvict = evictCLOCK(buffer);
        }
        

        return slotEvict;
    }

    void Fetch(int page_id){
        bool encontrado = false;
        buffer.contadorTempo++;
        //Procura se a Pagina já está no buffer

        for(int i = 0; i < 5; i++){
            if( !buffer.paginasCarregadas[i].livre && buffer.paginasCarregadas[i].p.page_id == page_id ){
                cacheHit++;

                buffer.refBit[i] = 1;
                buffer.tempo[i] = buffer.contadorTempo;
                buffer.indices.push(i);
                encontrado = true;  //Se já estiver ok e incrementa o cachehit 
                break;
            }
        }
        if(!encontrado){
            //Se nao, incrementa o cacheMiss -> procura a Pagina no disco -> verifica se tem espaço livre -> se nao tiver, encaixa de acordo com a politica escolhida do evict

            cacheMiss++;
            Pagina *p = nullptr;

            //Procurando no disco
            for(long long unsigned int i = 0; i < this->disco.size() ; i++){
                if(this->disco[i].page_id == page_id){
                    p = &this->disco[i];
                    break;
                } 
            }

            //Se por acaso estiver procurando uma pagino com id inexistente
            if(p == nullptr){
                throw runtime_error("Pagina nao encontrada no disco");
            }

            //Verifica se tem slot livre no buffer
            for(int i = 0; i < 5; i++){
                if(buffer.paginasCarregadas[i].livre){
                    buffer.paginasCarregadas[i].p = *p;
                    buffer.refBit[i] = 1;
                    buffer.tempo[i] = buffer.contadorTempo;
                    buffer.indices.push(i);
                    buffer.paginasCarregadas[i].livre = false;
                    return;
                }
            }

            int livre = Evict(politica); //retorna o slot livre pra carregar a Pagina

            buffer.paginasCarregadas[livre].p = *p;
            buffer.refBit[livre] = 1;
            buffer.tempo[livre] = buffer.contadorTempo;
            buffer.indices.push(livre);
            buffer.paginasCarregadas[livre].livre = false;
            return;
        }
    }

    void DisplayCache(){
        
        cout << "=======================================================" << endl << "BUFFER" << endl << "=======================================================" <<endl;
        cout << GREEN << "Slots livres: " << contarSlotsLivres() << endl << RESET;
        cout << "=======================================================" << endl << "Paginas" << endl;

        for(int i = 0; i<5; i++){

            if(buffer.paginasCarregadas[i].livre){
                cout << BLUE << "Slot " << i+1 << RESET << ":" << GREEN << " Livre" << RESET << endl;
                cout << "=======================================================" << endl;
                continue;
            }

            cout << BLUE << "Slot " << i+1 << ": " << RESET;
            cout << "Pagina " << buffer.paginasCarregadas[i].p.page_id << endl << "Conteudo: " 
            << buffer.paginasCarregadas[i].p.dados << endl << "Modificado:" << YELLOW 
            << (buffer.paginasCarregadas[i].p.dirty ? " Sim" : " Nao") << RESET << endl;
            cout << "=======================================================" << endl;
        }

        cout << endl;

    }
    

};