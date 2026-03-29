
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
#include "gerenciador/gerenciador.hpp"

using namespace std;




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

    
        cout <<endl << endl << "INICIANDO SISTEMA" << endl;
        g.lerArquivo("../bancodedados.csv");

        cout << "Pressione Enter para continuar..." << endl;
        cin.get();

        string pol = "";

        while(pol == ""){
            limparTela();
            cout << "Escolha a politica de substituicao: " << endl << "1. FIFO" << endl <<
                "2. LRU" << endl << "3. MRU" << endl << "4. CLOCK" << endl;
            int opcao;
            cin >> opcao;

            switch(opcao){
                case 1:
                    pol = "FIFO";
                    break;
                case 2:
                    pol = "LRU";
                    break;
                case 3:
                    pol = "MRU";
                    break;
                case 4:
                    pol = "CLOCK";
                    break;
                default:
                    cout << "Opcao invalida!" << endl;
            }

            g.definePolitica(pol);
            
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
                            cout <<endl << "Evicting pagina usando a politica " << pol << "..." << endl << "Pressione Enter para continuar..." << endl;
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
                    cerr << RED << "Erro: " << e.what() << RESET << endl;
                    cout << "Pressione Enter para continuar..." << endl;
                    cin.ignore();
                    cin.get();
                }
        }




    return 0;
}