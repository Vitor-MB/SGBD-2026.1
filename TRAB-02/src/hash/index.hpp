#ifndef INDEX_HPP
#define INDEX_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <cstring>
#include <sys/stat.h>
#include "hash.hpp"

//ESTRUTURAS PARA FACILITAR ESCREVER O out.txt
struct InclusaoResultado {
    bool success;
    vector<pair<int,int>> duplicacoes; // (pg, pl) de cada DUP DIR ocorrido
    int pgFinal;   // profundidade global após a inserção
    int plFinal;   // profundidade local do bucket onde a chave foi inserida
};

struct RemocaoResultado {
    int qtdRemovida; // 0 ou 1
    int pgFinal;
    int plFinal;     // pl do bucket onde a chave estaria
};

struct BuscaResultado {
    int qtdEncontrada; // 0 ou 1
};


void iniciaIndexHash(int initialGlobalDepth) {  //INICIALIZA O DIRETÓRIO E OS BUCKETS PARA O INDICE HASH EXTENSÍVEL
        Diretorio dir;
        dir.profundidadeGlobal = initialGlobalDepth;
        int size = 1 << initialGlobalDepth;
        dir.bucketRefs.resize(size);
        #ifdef _WIN32
            mkdir("../hash/buckets");
        #else
            mkdir("../hash/buckets", 0755);
        #endif

        for (int i = 0; i < size; i++) {
            dir.bucketRefs[i] = i;
            if (i > dir.maxId) {
                dir.maxId = i;
            }
            Bucket bucket;
            bucket.id = i;
            bucket.profundidadeLocal = initialGlobalDepth;
            bucket.BucketFileName();
            bucket.count = 0;
            if (!bucket.writeBucket()) {
                cerr << "Error writing bucket " << i << endl;
            }
        }
        if (!dir.writeDiretorio()) {
            cerr << "Error writing directory" << endl;
        }

}

int nextBucketId(Diretorio& dir) {  //GERA O PRÓXIMO ID PARA UM NOVO BUCKET
    return ++dir.maxId;
}

void dobrarDiretorio(Diretorio& dir){
    int oldSize = 1 << dir.profundidadeGlobal;
    dir.profundidadeGlobal++;
    int newSize = 1 <<dir.profundidadeGlobal;
    dir.bucketRefs.resize(newSize);

    //vamos referenciar aqui os novos elementos do diretorio
    for(int i = oldSize; i < newSize; i++) {
        dir.bucketRefs[i] = dir.bucketRefs[i - oldSize];
    }
}

pair<bool, pair<int,int>> splitBucket(Diretorio& dir, int idx){
    int oldBucketId = dir.bucketRefs[idx];

    // Carrega o bucket a ser dividido (única página de dados em memória agora)
    Bucket oldBucket;
    oldBucket.id = oldBucketId;
    oldBucket.ReadBucket();

    bool houveDuplicacao = false;
    pair<int,int> infoDup = {0, 0};

    
    // Se profundidade local == global, dobra o diretório antes de dividir
    if (oldBucket.profundidadeLocal == dir.profundidadeGlobal) {
        dobrarDiretorio(dir);
        houveDuplicacao = true;
        // Registra: pg após duplicação e pl que o bucket terá após a divisão
        infoDup = {dir.profundidadeGlobal, oldBucket.profundidadeLocal + 1};
    }
    // Incrementa a profundidade local do bucket existente
    oldBucket.profundidadeLocal++;



    // Cria o novo bucket irmão com a mesma profundidade local
    int newBucketId = nextBucketId(dir);
    Bucket newBucket;
    newBucket.id = newBucketId;
    newBucket.profundidadeLocal = oldBucket.profundidadeLocal;
    newBucket.count = 0;
    //─────────────────────────────────────────────────────────



    // Salva temporariamente as entradas do bucket antigo e o limpa para redistribuir
    int tempEntries[BUCKET_CAPACITY];
    int tempCount = oldBucket.count;
    memcpy(tempEntries, oldBucket.entries, sizeof(int) * tempCount);
    oldBucket.count = 0;
    memset(oldBucket.entries, -1, sizeof(oldBucket.entries));

    // Redistribuir: o bit na posição (profundidadeLocal - 1) decide o destino
    // (que é justamente o bit novo que vai diferenciar os buckets ex: era 10 e vai virar 10 e 11)
    for (int i = 0; i < tempCount; i++) {
        int key = tempEntries[i];
        int bit = (key >> (oldBucket.profundidadeLocal - 1)) & 1;
        if (bit == 0) {
            oldBucket.entries[oldBucket.count++] = key;
        } else {
            newBucket.entries[newBucket.count++] = key;
        }
    }

    // Atualiza ponteiros do diretório: entradas com bit diferenciador=1
    // que apontavam para o bucket antigo agora apontam para o novo bucket
    int totalEntries = 1 << dir.profundidadeGlobal;
    for (int i = 0; i < totalEntries; i++) {
        if (dir.bucketRefs[i] == oldBucketId) {
            int bit = (i >> (oldBucket.profundidadeLocal - 1)) & 1;
            if (bit == 1) {
                dir.bucketRefs[i] = newBucketId;
            }
        }
    }

    // Persiste: buckets primeiro, depois o diretório
    oldBucket.writeBucket();
    newBucket.writeBucket();
    dir.writeDiretorio();

    return {houveDuplicacao, infoDup};
}

//INCLUSÃO DE CHAVE

InclusaoResultado insertKey(int key){
    Diretorio dir;

    InclusaoResultado resultado;
    resultado.success = false;
    resultado.pgFinal = 0;
    resultado.plFinal = 0;

    if(!dir.ReadDiretorio()){
        cout << "Erro ao abrir diretorio" << endl;
        return resultado;
    }

    //laço para tentar inserir a chave, caso necessita realiza o split (que pode gerar dobra do diretorio)
    while(true){
        int idx = dir.hashKey(key);
        int bucketId = dir.bucketRefs[idx];

        Bucket bucket;
        bucket.id = bucketId;
        bucket.ReadBucket();

        // Chave duplicada: não insere
        if (bucket.contem(key)) {
            cout << "Chave " << key << " já existe no índice." << endl;
            return resultado;
        }

        // Bucket com espaço: insere normalmente
        if (!bucket.estaCheio()) {
            bucket.entries[bucket.count++] = key;
            bucket.writeBucket();
            dir.writeDiretorio();
            
            resultado.success = true;
            resultado.pgFinal = dir.profundidadeGlobal;
            resultado.plFinal = bucket.profundidadeLocal;
            return resultado;
        
        }

        // Bucket cheio: divide e tenta novamente
        auto result = splitBucket(dir, idx);

        bool houveDup = result.first;
        pair<int,int> infoDup = result.second;

        //Se precisar duplicar, a gente registra a duplicacao para informar no out.txt
        if(houveDup){
            resultado.duplicacoes.push_back(infoDup);
        }

        // Relê o diretório atualizado pela divisão
        dir.ReadDiretorio();
    }
}

//FUNÇÃO PARA REMOVER UMA KEY DO DIRETÓRIO
RemocaoResultado removeKey(int key){
    RemocaoResultado resultado = {0, 0, 0};

    Diretorio dir;
    if(!dir.ReadDiretorio()){
        cout << "Erro ao abrir diretorio" << endl;
        return resultado;
    }

    resultado.pgFinal = dir.profundidadeGlobal;

    int idx = dir.hashKey(key);
    int bucketId = dir.bucketRefs[idx];

    Bucket bucket;
    bucket.id = bucketId;
    bucket.ReadBucket();

    resultado.plFinal = bucket.profundidadeLocal;

    // Busca e remove a chave compactando o array
    for (int i = 0; i < bucket.count; i++) {
        if (bucket.entries[i] == key) {
            // Se encontrarmos substituimos pela última entrada e reduz o contador
            bucket.entries[i] = bucket.entries[--bucket.count];
            bucket.entries[bucket.count] = -1;
            bucket.writeBucket();
            resultado.qtdRemovida = 1;
            return resultado;
        }
    }

    cout << "Chave " << key << " não encontrada para remoção." << endl;
    return resultado;

    
}

//FUNÇÃO PARA BUSCAR UMA KEY NO DIRETORIO
BuscaResultado buscarKey(int key){
    BuscaResultado resultado = {0};

    Diretorio dir;
    if(!dir.ReadDiretorio()){
        cout << "Erro ao abrir diretorio";
        return resultado;
    }

    int idx = dir.hashKey(key);
    int bucketId = dir.bucketRefs[idx];

    Bucket bucket;
    bucket.id = bucketId;
    bucket.ReadBucket();

    if(bucket.contem(key)){
        cout << "Chave " << key << " encontrada no bucket " << bucketId << "." << endl;
        resultado.qtdEncontrada = 1;
        
        //busca no bd a linha correspondente
        ifstream file("../database/bd-trab2 - dataset.csv");
        
        if(!file.is_open()){
            cerr << "Erro ao abrir dataset.csv" << endl;
            return resultado;
        }

        string line;
        int atual = 0;

        while(getline(file, line)) {
            if(atual == key) {
                cout << "Linha correspondente à chave " << key << ": " << line << endl;
                break;
            }
            atual++;
        }

        return resultado;
    }

    cout << "A chave " << key << " nao foi encontrada" << endl;
    return resultado;
}

//=============== GUI PARA AJUDAR NO DEBUG E APRESENTAÇÃO DE COMO ESTÁ O DIRETÓRIO ==============

//TRANSFORMAR NUMERAL EM BINÁRIO
string toBinary(int num, int bits) {
    string s = "";
    for (int i = bits - 1; i >= 0; i--) {
        s += ((num >> i) & 1) ? '1' : '0';
    }
    return s;
}


void printIndex() {
    Diretorio dir;
    if (!dir.ReadDiretorio()) {
        cerr << "Erro ao ler diretório" << endl;
        return;
    }

    cout << endl << "==== DIRETORIO ====" << endl;
    cout << "Profundidade Global: " << dir.profundidadeGlobal << endl;
    cout << "MaxId: " << dir.maxId << endl;
    int totalEntries = 1 << dir.profundidadeGlobal;

    //COLOCA QUAL SÃO OS BITS MENOS SIGNIFICATIVOS CORRESPONDENTES A CADA BUCKET

    for (int i = 0; i < totalEntries; i++) {
        cout << "  [" << toBinary(i, dir.profundidadeGlobal) << "] -> Bucket " << dir.bucketRefs[i] << endl;
    }

    cout << "\n==== BUCKETS ====" << endl;

    // Imprime os BUCKETS únicos (evita repetição de buckets compartilhados (caso em que a prof local < prof global))

    vector<bool> printed(dir.maxId + 1, false);
    for (int i = 0; i <= dir.maxId; i++) {
        if (printed[i]) continue;
        printed[i] = true;

        Bucket b;
        b.id = i;
        if (!b.ReadBucket()) continue;

        cout << "  Bucket " << i
             << " (prof.local=" << b.profundidadeLocal
             << ", count=" << b.count << "): [";
        for (int j = 0; j < b.count; j++) {
            cout << b.entries[j];
            if (j < b.count - 1) cout << ", ";
        }
        cout << "]" << endl;
    }
}

#endif