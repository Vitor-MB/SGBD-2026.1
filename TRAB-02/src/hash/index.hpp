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

 void initIndexHash(int initialGlobalDepth) {  //INICIALIZA O DIRETÓRIO E OS BUCKETS PARA O INDICE HASH EXTENSÍVEL
        Diretorio dir;
        dir.profundidadeGlobal = initialGlobalDepth;
        int size = 1 << initialGlobalDepth;
        dir.bucketRefs.resize(size);
        #ifdef _WIN32
            _mkdir(dir.DiretorioFileName().c_str());
        #else
            mkdir(dir.DiretorioFileName().c_str(), 0755);
        #endif

        for (int i = 0; i < size; i++) {
            dir.bucketRefs[i] = i;
            if (i > dir.maxId) {
                dir.maxId = i;
            }
            Bucket bucket;
            bucket.id = i;
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



#endif