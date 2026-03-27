#include <vector>
#include <string>
#include <cstring>
#include <iostream>

#include <fstream>

using namespace std;



typedef struct pagina{
    int page_id;
    string dados; 
    bool dirty = false;
}pagina;

typedef struct slot{
    pagina p;
    bool livre = true;
}slot;

typedef struct Buffer{
    slot paginasCarregadas[5];
}Buffer;

class Gerenciador{
public:
    
    Gerenciador(){
        cacheHit = 0;
        cacheMiss = 0;
        
        for(int i = 0; i < 5; i++){
            buffer.paginasCarregadas[i].livre = true;
        }
    }

    void IniciarPag(pagina &p){
        p.page_id = contadorId++;
    }

    void lerArquivo(string arquivo){
        cout << "Abrindo arquivo: " << arquivo << endl << endl;
        ifstream file(arquivo);
        string linha;

        if(!file.is_open()){
            throw runtime_error("Nao foi possivel abrir o arquivo");
        }

        cout << "Lendo arquivo";
        while(getline(file, linha)){
            pagina p ;
            IniciarPag(p);
            cout << "." ;

            p.dados = linha;
            disco.push_back(p);

        }

        cout << endl << "Disco inciado com " << disco.size() << " paginas." << endl;
    }

    void printarDisco(){
    for(size_t i = 0; i < disco.size(); i++){
        pagina &p = disco[i];

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

    int Evict(){
        for(int i = 0; i < 5; i++){
            if(buffer.paginasCarregadas[i].livre){
                return i;
            }
        }

        //politica de substituição: FIFO (COrrigir, ele n está como uma fila, só ta tirando o primeiro)

        int slotEvict = 0;

        if(buffer.paginasCarregadas[slotEvict].p.dirty){
            for(size_t j = 0; j < disco.size(); j++){
                if(disco[j].page_id == buffer.paginasCarregadas[slotEvict].p.page_id){
                    disco[j] = buffer.paginasCarregadas[slotEvict].p;
                    buffer.paginasCarregadas[slotEvict].p.dirty = false;
                    break;
                }
            }
        }

        buffer.paginasCarregadas[slotEvict].livre = true;
        return slotEvict;
    }

    void Fetch(int page_id){
        bool encontrado = false;
        for(int i = 0; i < 5; i++){
            if( !buffer.paginasCarregadas[i].livre && buffer.paginasCarregadas[i].p.page_id == page_id ){
                cacheHit++;
                encontrado = true;
                break;
            }
        }
        if(!encontrado){
            cacheMiss++;

            int livre = Evict(); //retorna o slot livre pra carregar a pagina

            for(long long unsigned int i = 0; i < this->disco.size() ; i++){
                if(this->disco[i].page_id == page_id){
                    buffer.paginasCarregadas[livre].p = this->disco[i];
                    buffer.paginasCarregadas[livre].livre = false;
                    break;
                } 
            }
        }
    }

    void DisplayCache(){
        
        cout << "=======================================================" << endl << "BUFFER" << endl << "=======================================================" <<endl;
        cout << "Slots livres: " << contarSlotsLivres() << endl;
        cout << "=======================================================" << endl << "PAGINAS" << endl;

        for(int i = 0; i<5; i++){

            if(buffer.paginasCarregadas[i].livre){
                cout << "Slot " << i+1 << ": Livre" << endl;
                cout << "=======================================================" << endl;
                continue;
            }

            cout << "Slot " << i+1 << ": " << endl;
            cout << "Pagina " << buffer.paginasCarregadas[i].p.page_id << ": " << buffer.paginasCarregadas[i].p.dados << endl << "Modificado:" << (buffer.paginasCarregadas[i].p.dirty ? "Sim" : "Nao") << endl;
            cout << "=======================================================" << endl;
        }

        cout << endl;

    }
    

private:
    Buffer buffer;
    int contadorId = 0;
    int cacheMiss = 0;
    int cacheHit = 0;
    vector<pagina> disco;

};

// ==========================================================================





void InserirRegistro(pagina &p, string entrada){
    cout<< "Inserindo registro na pagina" << p.page_id;
    p.dirty = true;

    p.dados = entrada;
}

void apagarRegistro(pagina &p){
    cout<< "Apagando conteudo da pagina " << p.page_id;
    p.dados = "";
    p.dirty = true;
}


int main(){

    Gerenciador g;
    

    try{
        cout << "INICIANDO SISTEMA" << endl;
        g.lerArquivo("../bancodedados.csv");
        
        g.DisplayCache();
        g.Fetch(0);
        g.Fetch(1);
        g.Fetch(2);
        g.Fetch(3);
        g.Fetch(4);
        g.DisplayCache();

        g.Fetch(8);
        g.DisplayCache();
   
        g.DisplayCache();
        g.Fetch(8);
        g.displayStats();

    }catch(runtime_error &e){
        cerr << "Erro: " << e.what() << endl;
    }


    return 0;
}