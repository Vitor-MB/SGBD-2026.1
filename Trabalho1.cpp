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
    int posFila = 0;
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

            linha.pop_back(); //remover a virgula
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

    int Evict(string politica){

        
        if(contarSlotsLivres() == 5){
            return 0;
        }

        int slotEvict = 0;

        //politica de substituição: FIFO 
        if(politica == "FIFO"){
            slotEvict = buffer.posFila;
            buffer.posFila = (buffer.posFila + 1) % 5;

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
        }

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
            pagina *p = nullptr;

            for(long long unsigned int i = 0; i < this->disco.size() ; i++){
                if(this->disco[i].page_id == page_id){
                    p = &this->disco[i];
                    break;
                } 
            }

            if(p == nullptr){
                throw runtime_error("Pagina nao encontrada no disco");
            }

            for(int i = 0; i < 5; i++){
                if(buffer.paginasCarregadas[i].livre){
                    buffer.paginasCarregadas[i].p = *p;
                    buffer.paginasCarregadas[i].livre = false;
                    return;
                }
            }

            int livre = Evict("FIFO"); //retorna o slot livre pra carregar a pagina

            buffer.paginasCarregadas[livre].p = *p;
            buffer.paginasCarregadas[livre].livre = false;
            return;
        }
    }

    void DisplayCache(){
        
        cout << "=======================================================" << endl << "BUFFER" << endl << "=======================================================" <<endl;
        cout << GREEN << "Slots livres: " << contarSlotsLivres() << endl << RESET;
        cout << "=======================================================" << endl << "PAGINAS" << endl;

        for(int i = 0; i<5; i++){

            if(buffer.paginasCarregadas[i].livre){
                cout << BLUE << "Slot " << i+1 << RESET << ":" << GREEN << " Livre" << RESET << endl;
                cout << "=======================================================" << endl;
                continue;
            }

            cout << BLUE << "Slot " << i+1 << ": " << RESET;
            cout << "Pagina " << buffer.paginasCarregadas[i].p.page_id << endl << "Conteudo: " << buffer.paginasCarregadas[i].p.dados << endl << "Modificado:" << YELLOW << (buffer.paginasCarregadas[i].p.dirty ? " Sim" : " Nao") << RESET << endl;
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


void limparTela() {
    #ifdef _WIN32
    system("cls");
    #else
        system("clear");
    #endif
}


int main(){

    Gerenciador g;
    bool Interface = true;

    
        cout << "INICIANDO SISTEMA" << endl;
        g.lerArquivo("../bancodedados.csv");

        cout << "Pressione Enter para continuar..." << endl;
        cin.get();

        string pol = "";

        while(pol == ""){
            limparTela();
            cout << "Escolha a politica de substituicao: " << endl << "1. FIFO" << endl;
            int opcao;
            cin >> opcao;

            switch(opcao){
                case 1:
                    pol = "FIFO";
                    break;
                default:
                    cout << "Opcao invalida!" << endl;
            }
        }


        while(Interface){
                try{
                    limparTela();

                    int opcao;
                    cout << GREEN << "Escolha uma opcao: " << RESET << endl;
                    cout << "1. Procurar/Trazer Página para o Buffer (Fetch)" << endl;
                    cout << "2. Apagar Pagina do Buffer (Evict)" << endl;
                    cout << "3. Exibir cache (DisplayCache)" << endl;
                    cout << "4. Exibir estatisticas (DisplayStats)" << endl;
                    cout << "5. Sair" << endl;

                    cin >> opcao;

                    switch(opcao){
                        case 1: {
                            int page_id;
                            cout << "Digite o ID da pagina: ";
                            cin >> page_id;

                            g.Fetch(page_id); 
                        
                            break;
                        }
                        case 2: {
                            cout << "Evicting pagina usando a politica " << pol << "..." << endl;
                            cin.ignore();
                            cin.get();
                            g.Evict(pol);
                            break;
                        }
                        case 3:
                            g.DisplayCache();
                            cout << "Pressione Enter para continuar..." << endl;
                            cin.ignore();
                            cin.get();
                            break;
                        case 4:
                            g.displayStats();
                            cout << "Pressione Enter para continuar..." << endl;
                            cin.ignore();
                            cin.get();
                            break;
                        case 5:
                            Interface = false;
                            break;
                        default:
                            cout << "Opcao invalida!" << endl;
                    }
                
                }catch(runtime_error &e){
                    cerr << "Erro: " << e.what() << endl;
                    cout << "Pressione Enter para continuar..." << endl;
                    cin.ignore();
                    cin.get();
                }
        }




    return 0;
}