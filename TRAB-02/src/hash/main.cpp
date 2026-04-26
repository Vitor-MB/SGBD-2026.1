#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "index.hpp"

using namespace std;

int main() {
    // ─ Abre os arquivos de entrada e saída ─
    ifstream inFile("../in.txt");
    if (!inFile.is_open()) {
        cerr << "Erro: não foi possível abrir in.txt" << endl;
        return 1;
    }

    ofstream outFile("../out.txt");
    if (!outFile.is_open()) {
        cerr << "Erro: não foi possível criar out.txt" << endl;
        return 1;
    }

    string firstLine;
    getline(inFile, firstLine);

    // Extrai o valor numérico após "PG/"
    int pgInicial = 0;
    if (firstLine.substr(0, 3) == "PG/") {
        pgInicial = stoi(firstLine.substr(3));
    } else {
        cerr << "Formato inválido na primeira linha: " << firstLine << endl;
        return 1;
    }

    // Replica a primeira linha no arquivo de saída
    outFile << firstLine << "\n";

    //incializa o hash extensivel
    initIndexHash(pgInicial);

    //  Processa cada operação do arquivo de entrada
    string line;
    while (getline(inFile, line)) {
        // Ignora linhas em branco
        if (line.empty()) continue;

        // Inclusão: INC:x
        if (line.substr(0, 4) == "INC:") {
            int key = stoi(line.substr(4));
            InclusaoResultado res = insertKey(key);

            if (res.success) {
                // Emite DUP DIR antes do INC para cada duplicação ocorrida
                for (auto& dup : res.duplicacoes) {
                    outFile << "DUP DIR:/" << dup.first << "," << dup.second << "\n";
                }
                outFile << "INC:" << key << "/"
                        << res.pgFinal << "," << res.plFinal << "\n";
            } else {
                // Inserção falhou (chave duplicada); registra com profundidade global/profundidade local atuais
                // Lê o diretório para obter prof global corrente
                Diretorio dir;
                dir.ReadDiretorio();
                int idx = dir.hashKey(key);
                Bucket b;
                b.id = dir.bucketRefs[idx];
                b.ReadBucket();
                outFile << "INC:" << key << "/"
                        << dir.profundidadeGlobal << "," << b.profundidadeLocal << "\n";
            }
        }
        // Remoção: REM:x
        else if (line.substr(0, 4) == "REM:") {
            int key = stoi(line.substr(4));
            RemocaoResultado res = removeKey(key);

            outFile << "REM:" << key << "/"
                    << res.qtdRemovida << ","
                    << res.pgFinal << "," << res.plFinal << "\n";
        }
        // Busca: BUS:x
        else if (line.substr(0, 5) == "BUS=:") {
            int key = stoi(line.substr(5));
            BuscaResultado res = buscarKey(key);

            outFile << "BUS:" << key << "/" << res.qtdEncontrada << "\n";
        }
        else {
            cerr << "Operação desconhecida: " << line << endl;
        }
    }

    // Escrever na ultima linha do out.txt
    Diretorio dirFinal;
    dirFinal.ReadDiretorio();
    outFile << "P:/" << dirFinal.profundidadeGlobal << "\n";

    inFile.close();
    outFile.close();

    cout << "Processamento concluído. Resultado em out.txt" << endl;
    return 0;
}
