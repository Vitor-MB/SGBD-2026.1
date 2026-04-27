# *TRABALHO 2 - SGBD 2026.1*

#### Alunos: Américo Vitor Moreira Barbosa - 571045 // Aluizio Pereira Almendra Neto - 565068

---

Para rodar o programa deve-se possuir o arquivo in.txt com operações como: `INC:x (Para incluir chave x)` / `REM:x (Para remoção chave x)` / `BUS=:x (Para buscar chave x)`

Como exemplo de in.txt:

### in.txt
``` txt
PG/1
INC:1
INC:3
INC:5
INC:0
INC:9
INC:13
INC:15
INC:17
INC:14
INC:7
INC:10
BUS=:20
INC:11
INC:12
INC:19
INC:21
INC:23
BUS=:16
INC:16
BUS=:16
INC:18
BUS=:1
INC:20
BUS=:28
INC:22
INC:26
INC:30
INC:2
INC:42
INC:34
```

**O in.txt deve estar em src/hash/**

- É gerado no em src/hash/ um **out.txt** e um **diretorio.txt**
- Os buckets ficaram em src/hash/buckets/

## Estrutura do diretorio (.txt)
O diretorio é o diretorio.txt

Exemplo de diretorio:
```
4   -> Profundidade Global
7   -> MaxId
0   -> BucketsRefs
1
5
2
0
```

## Estrutura do bucket (.txt)
Os buckets são nomeados como `(id).txt`


Exemplo de bucket:
```
2  -> Profundidade Local 
4  -> Count
0  -> Índices
12
16
20

```

## Rodar o programa

Para rodar o programa, deve-se compilar e rodar o `main.cpp` em uma pasta `src/output/`

Caso não seja compilado em uma pasta `src/output/` deve-se mudar o caminho das funções nos seguintes arquivos:

#### hash.hpp
```cpp
//SUBSTITUIR
string BucketFileName() const {
        return "../hash/buckets/" + to_string(id) + ".txt";
}

//POR
string BucketFileName() const {
        return "hash/buckets/" + to_string(id) + ".txt";
}


//SUBSTITUIR
string DiretorioFileName() const {
        return "../hash/diretorio.txt";
}

//POR
string DiretorioFileName() const {
        return "hash/diretorio.txt";
}


```

#### main.cpp

```cpp

//SUBSTITUIR
ifstream inFile("../hash/in.txt");

//POR
ifstream inFile("hash/in.txt");

//SUBSTITUIR
ofstream outFile("../hash/out.txt");

//POR
ofstream outFile("hash/out.txt");

```
