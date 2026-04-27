#ifndef HASH_HPP
#define HASH_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <cstring>
#include <sys/stat.h>


using namespace std;
#define BUCKET_CAPACITY 5

struct Diretorio{
    int profundidadeGlobal;
    vector<int> bucketRefs;
    int maxId;

    Diretorio() : profundidadeGlobal(0), maxId(-1) {}

    string DiretorioFileName() const {
        return "../hash/diretorio.txt";
    }

    bool ReadDiretorio() { //LE O DIRETÓRIO DO ARQUIVO PARA A MEMORIA
        ifstream inFile(DiretorioFileName());
        if (!inFile.is_open()) {
            return false;
        }
        inFile >> profundidadeGlobal;
        inFile >> maxId;
        bucketRefs.resize(1 << profundidadeGlobal);
        for (int i = 0; i < (1 << profundidadeGlobal); i++) {
            inFile >> bucketRefs[i];
        }
        inFile.close();
        return true;
    }

    bool writeDiretorio() { //ESCREVE O DIRETÓRIO DA MEMORIA PARA O ARQUIVO
        ofstream outFile(DiretorioFileName());
        if (!outFile.is_open()) {
            return false;
        }
        outFile << profundidadeGlobal << endl;
        outFile << maxId << endl;
        for (size_t i = 0; i < bucketRefs.size(); i++) {
            outFile << bucketRefs[i] << endl;
        }
        outFile.close();
        return true;
    }

    int hashKey(int key) const {  //RETORNA OS BITS MENOS SIGNIFICATIVOS DO HASH
        return key & ((1 << profundidadeGlobal) - 1);
    }

};

struct Bucket{
    int id;
    int profundidadeLocal;
    int count;
    int entries[BUCKET_CAPACITY];

    Bucket() : profundidadeLocal(0), count(0) {
        memset(entries, -1, sizeof(entries));
    }

    string BucketFileName() const {
        return "../hash/buckets/" + to_string(id) + ".txt";
    }

    bool ReadBucket() { //LE O BUCKET DO ARQUIVO PARA A MEMORIA
        ifstream inFile(BucketFileName());
        if (!inFile.is_open()) {
            return false;
        }
        

        inFile >> profundidadeLocal >> count;
        for (int i = 0; i < count && i < BUCKET_CAPACITY; i++) {
            inFile >> entries[i];
        }
        inFile.close();
        return true;
    }

    bool writeBucket() { //ESCREVE O BUCKETDA MEMORIA PARA O ARQUIVO
        ofstream outFile(BucketFileName());
        if (!outFile.is_open()) {
            return false;
        }
        outFile << profundidadeLocal << "\n" << count << endl;
        for (int i = 0; i < count && i < BUCKET_CAPACITY; i++) {
            outFile << entries[i] << endl;
        }
        outFile.close();
        return true;
    }

    bool contem(int key) const{
        for(int i = 0 ; i < count; i++){
            if(entries[i] == key){
                return 1;
            }
        }
        return 0;
    }

    bool estaCheio() const{
        return (count >= BUCKET_CAPACITY);
    }
    
};




#endif
