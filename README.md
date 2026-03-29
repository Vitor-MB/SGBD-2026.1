# 🖥️ Gerenciador de Buffer - SGBD 2026.1


## 1. 📄 Descrição do Projeto
Este projeto implementa um **Gerenciador de Buffer** para um Sistema Gerenciador de Banco de Dados (SGBD). O objetivo é intermediar o acesso entre o armazenamento em disco (simulado por um arquivo `.csv`) e a memória RAM (buffer), otimizando a performance através de estatísticas de *Cache Hit* e *Cache Miss* e políticas de substituição de slot.


## 2. 📋 Especificações Técnicas
*   **Capacidade Máxima:** O buffer suporta exatamente **5 páginas** simultâneas.
*   **Página de Dados:** Cada página contém um `page_id` único, o conteúdo em texto (`dados`) e um bit de modificação (`dirty`).
*   **Estatísticas:** O sistema monitora sucessos (*Hits*) quando a página já está no buffer e falhas (*Misses*) quando é necessário buscar no disco.


## 3. 📂 Estrutura de Diretórios
O projeto segue uma organização modular para facilitar a manutenção:


*   **`src/gerenciador/`**: Contém o `gerenciador.hpp`, o núcleo que coordena as operações, por meio da classe *gerenciador*.
*   **`src/models/`**: Define as estruturas básicas:
    *   `buffer.hpp`: Estrutura de controle do buffer (filas, tempos e bits de referência).
    *   `pagina.hpp`: Definição da estrutura da página.
    *   `slot.hpp`: Estrutura que envolve a página no buffer para controle de ocupação.
*   **`src/politicas/`**: Implementações individuais dos algoritmos:
    *   `fifo.hpp`: First In First Out
    *   `lru.hpp`: Least Recently Used
    *   `mru.hpp`: Most Recently Used
    *   `clock.hpp`: Algoritmo de segunda chance
*   **`main.cpp`**: Ponto de entrada com a interface de usuário.
*   **`bancodedados.csv`**: O arquivo que simula o disco rígido.


## 4. 🚀 Fluxo de Execução Detalhado
O funcionamento do programa segue rigorosamente o ciclo abaixo, conforme implementado no `main.cpp`:


1.  **Inicialização:**
   *   O sistema inicia instanciando o `Gerenciador` e executando `lerArquivo("../bancodedados.csv")`.
   *   Cada linha do CSV é convertida em um objeto `Pagina` com um ID sequencial e armazenada no vetor `disco`.


2.  **Seleção Obrigatória de Política:**
   *   Antes de qualquer operação, um loop `while` exige que o usuário escolha uma política de substituição (1-FIFO, 2-LRU, 3-MRU ou 4-CLOCK).
   *   A escolha define o comportamento global da função `Evict()`.


3.  **Menu de Interação Principal:**
   *   **Opção 1 (Fetch):** O usuário digita um ID. O sistema busca no buffer. Caso a página esteja no buffer (*Hit*), os dados da página são retornados e mostrados na interface. Se houver falha (*Miss*), a página é trazida do disco. Se o buffer estiver lotado, o sistema chama automaticamente o `Evict()` para abrir espaço.
   *   **Opção 2 (Evict Manual):** Força a remoção de uma página do buffer seguindo a política ativa. Se a página removida estiver marcada como modificada (`dirty = true`), o sistema exibe o aviso **"W"**.
   *   **Opção 3 (DisplayCache):** Mostra o estado atual de todos os 5 slots (se estão livres ou qual página ocupam).
   *   **Opção 4 (DisplayStats):** Exibe o balanço acumulado de Hits e Misses.


4.  **Tratamento de Exceções:**
   *   O fluxo é protegido por um bloco `try-catch`. Se o usuário solicitar um ID de página que não existe no disco, o sistema captura o `runtime_error`, exibe uma mensagem de erro em vermelho e retorna ao menu sem travar.


## 5. 🔄 Detalhes das Políticas de Substituição
| Política | Mecanismo de Implementação |
| :--- | :--- |
| **FIFO** | Utiliza `posFila` para remover a página que entrou primeiro (fila circular). |
| **LRU** | Compara o array `tempo[]` para remover a página com o **menor** tempo de acesso. |
| **MRU** | Compara o array `tempo[]` para remover a página com o **maior** tempo de acesso. |
| **CLOCK** | Percorre o `refBit[]`. Se for 0, remove. Se for 1, reseta para 0 e passa para a próxima (segunda chance). |


## 6. 🛠️ Requisitos e Compilação
*   **Linguagem:** C++.
*   **Compilação:** O projeto possui um executável denominado `exe`.

