#include <vector>
#include <string>
#include <cstring>
#include <iostream>

#include <fstream>

using namespace std;

//ideia de mapear os registros como mostrado no slide

typedef struct diretorio{
    int freePtr; //ponteiro pro proximo slot livre
    int numSlots; //qtd slots
}diretorio;

typedef struct slot{
    int offset;
    int tamanho;
}slot;

typedef struct pagina{
    char dados[4096]; //TAMANHO DE 4KB
    bool dirty = false;
}pagina;

typedef struct registro{
    int id;
    char dados[100];
}registro;

typedef struct buffer{
    pagina paginasCarregadas[5];
}buffer;


// ==========================================================================

int contadorId = 0;

void IniciarPagina(pagina &p){
    memset(p.dados, 0, 4096);
    diretorio *h = (diretorio*)p.dados;
    
    h->freePtr = 4096;
    h->numSlots = 0;
}

void InserirRegistro(pagina &p, registro &registro){
    diretorio *h = (diretorio*)p.dados;
    int tamanho = sizeof(registro);

    cout << "Tentando inserir registro ID " << registro.id << " tamanho " << tamanho << endl;

    if(h->freePtr - tamanho < sizeof(diretorio) + (h->numSlots+1) * sizeof(slot) ){
        cout << "Pagina cheia!" << endl;
        throw runtime_error("Pagina cheia");
    }

    slot *s = (slot*)(p.dados + sizeof(diretorio) + h->numSlots * sizeof(slot));
    int offset = h->freePtr - tamanho;

    cout << "Offset = " << offset << endl;

    memcpy(p.dados + offset, &registro , tamanho);

    h->freePtr = offset;
    h->numSlots += 1;

    s->offset = offset;
    s->tamanho = tamanho;

    cout << "Registro inserido no slot " << h->numSlots-1 << endl;
}

void apagarRegistro(pagina &p, int id){
    diretorio *h = (diretorio*)p.dados;

    for(int i = 0; i < h->numSlots; i++){
        slot *s = (slot*)(p.dados + sizeof(diretorio) + i * sizeof(slot));
        registro *r = (registro*)(p.dados + s->offset);

        if(r->id == id){
            //marcar o slot como vazio
            s->tamanho = -1;
            return;
        }
    }

    throw runtime_error("Registro nao encontrado");
}

void lerArquivo(vector<pagina> &disco){
    cout << "Abrindo arquivo..." << endl;
    ifstream file("../bancodedados.csv");
    string linha;

    if(!file.is_open()){
        throw runtime_error("Nao foi possivel abrir o arquivo");
    }

    pagina p ;
    IniciarPagina(p);

    while(getline(file, linha)){
        registro r;
        r.id = contadorId++;
        strcpy(r.dados, linha.c_str());

        cout << "Lendo linha: " << linha << endl;

        try{
            InserirRegistro(p, r);
            
        }catch(runtime_error &e){

            cout << "Pagina cheia, criando nova pagina..." << endl;
            disco.push_back(p);
            IniciarPagina(p);
            InserirRegistro(p, r);
        }

    }

    disco.push_back(p);
}


void printarDisco(vector<pagina> &disco){
    for(size_t i = 0; i < disco.size(); i++){
        pagina &p = disco[i];
        diretorio *h = (diretorio*)p.dados;

        cout << "Pagina " << i << ": " << endl;
        cout << "Numero de slots: " << h->numSlots << endl;
        cout << "Free pointer: " << h->freePtr << endl;
        
        for(int j = 0; j < h->numSlots; j++){
            slot *s = (slot*)(p.dados + sizeof(diretorio) + j * sizeof(slot));
            if(s->tamanho != -1){
                registro *r = (registro*)(p.dados + s->offset);
                cout << "Registro ID: " << r->id << ", Dados: " << r->dados << endl;
            }
        }
        cout << endl;
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