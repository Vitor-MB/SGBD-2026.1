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


typedef struct buffer{
    pagina paginasCarregadas[5];
}buffer;

class Gerenciador{
public:
    Gerenciador();
    void IniciarPag(pagina &p);
    void InserirRegistro(pagina &p, string entrada);
    void apagarRegistro(pagina &p);
    void lerArquivo(vector<pagina> &disco);
    void printarDisco(vector<pagina> &disco);

    void displayStats(){
        cout << "Cache Hit: " << cacheHit << endl;
        cout << "Cache Miss: " << cacheMiss << endl;
    }

    int Evict(int politica){
        //implementar politicas de substituição
    }

    void Fetch(int page_id){
        bool encontrado = false;
        for(int i = 0; i < 5; i++){
            if(buffer.paginasCarregadas[i].page_id == page_id){
                cacheHit++;
                encontrado = true;
                break;
            }
        }
        if(!encontrado){
            cacheMiss++;

            int livre = Evict(0);

            for(int i = 0; i < this->disco.size() ; i++){
                if(this->disco[i].page_id == page_id){

                } 
            }

        }
    }

    

private:
    buffer buffer;
    int cacheMiss = 0;
    int cacheHit = 0;
    vector<pagina> disco;

};

// ==========================================================================

int contadorId = 0;

void IniciarPag(pagina &p){
    p.page_id = contadorId++;
}

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

void lerArquivo(vector<pagina> &disco){
    cout << "Abrindo arquivo..." << endl << endl;
    ifstream file("../bancodedados.csv");
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
    cout << "Disco inciado com " << disco.size() << " paginas." << endl;
}


void printarDisco(vector<pagina> &disco){
    for(size_t i = 0; i < disco.size(); i++){
        pagina &p = disco[i];

        cout << "Pagina " << p.page_id << ": " << endl;

        cout <<"conteudo: " << p.dados << endl << endl;
    }
}

int main(){

    vector<pagina> disco;
    

    try{
        cout << "Lendo arquivo..." << endl;
        lerArquivo(disco);
        printarDisco(disco);
    }catch(runtime_error &e){
        cerr << "Erro: " << e.what() << endl;
    }


    return 0;
}