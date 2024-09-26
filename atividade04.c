#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <mpi.h>

#define MASTER_RANK 0
#define TAG_DATA 0
#define TAG_RESULT 1

// Joao Pedro dos Santos Melo 2019.1.08.044

//posiçoes do filtro
// 1 2 3
// 4 5 6     5 é o pixel que lP e cP indicam
// 7 8 9

bool verifica (int* imagem, int lP, int cP, int linhas, int colunas, int branco){ // verifica se pode ser uma estrela

    if(cP > 0){ // 4
        if(imagem[lP * colunas + (cP-1)] >= branco){
            return true;
        }
        if(lP > 0){ // 1
            if(imagem[(lP-1) * colunas + (cP-1)] >= branco){
                return true;
            }
        }
    }
    if(cP < (colunas-1)){ // 6
        if(imagem[lP * colunas + cP+1] >= branco){
            return true;
        }
        if(lP < (linhas-1)){ // 9
            if(imagem[(lP+1) * colunas + cP+1] >= branco){
                return true;
            }
        }
    }
    if(lP > 0){ // 2
        if(imagem[(lP-1) * colunas + cP] >= branco){
            return true;
        }
        if(cP < (colunas-1)){ // 3
            if(imagem[(lP-1) * colunas + cP+1] >= branco){
                return true;
            }
        }
    }
    if(lP < (linhas-1)){ // 8
        if(imagem[(lP+1) * colunas + cP] >= branco){
            return true;
        }
        if(cP > 0){ // 7
            if(imagem[(lP+1) * colunas + cP-1] >= branco){
                return true;
            }
        }
    }
    return false;
}

void pinta (int* imagem, int lP, int cP, int linhas, int colunas, int branco){
    imagem[lP * colunas + cP] = 0;
    if(cP > 0){
        if(imagem[lP * colunas + cP-1] >= branco){ // 4
            pinta(imagem, lP, (cP-1), linhas, colunas, branco);
        }
        if(lP > 0){
            if(imagem[(lP-1) * colunas + cP-1] >= branco){ // 1
                pinta(imagem, (lP-1), (cP-1), linhas, colunas, branco);
            }
        }
    }
    if(lP > 0){
        if(imagem[(lP-1) * colunas + cP] >= branco){ // 2
            pinta(imagem, (lP-1), cP, linhas, colunas, branco);
        }
        if(cP < (colunas-1)){
            if(imagem[(lP-1) * colunas + cP+1] >= branco){ // 3
                pinta(imagem, (lP-1), (cP+1), linhas, colunas, branco);
            }
        }
    }
    if(cP < (colunas-1)){ 
        if(imagem[lP * colunas + cP+1] >= branco){ // 6
            pinta(imagem, lP, (cP+1), linhas, colunas, branco);
        }else{
            if(lP < (linhas-1)){
                if(imagem[(lP+1) * colunas + cP+1] >= branco){ // 9
                    pinta(imagem, (lP+1), (cP+1), linhas, colunas, branco);
                }
            }
        }

    }
    if(lP < (linhas-1)){
        if(imagem[(lP+1) * colunas + cP] >= branco){ // 8
            pinta(imagem, (lP+1), cP, linhas, colunas, branco);
        }else{
            if(cP > 0){
                if(imagem[(lP+1) * colunas + cP-1] >= branco){ // 7
                    pinta(imagem, (lP+1), (cP-1), linhas, colunas, branco);
                }
            }
        }
    }  
}

int main() {
    
    int rank, size;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    FILE* file = fopen("estrelas1.pgm", "r"); // troca a imagem *******************************************************************
    if (!file) {
        perror("Erro ao abrir o arquivo");
        return 0;
    }

    int colunas, linhas, max_val;
    char p2[5];
    char frase[100];
    int cont = 0;



    // Ler cabeçalho P2
    fscanf(file, "%s\n", p2);
    //fgets(frase, sizeof(frase), file);
    fscanf(file, "%d %d", &colunas, &linhas);  // Lê a largura e altura
    fscanf(file, "%d", &max_val);            // Lê o valor máximo de cinza

    int lin_part = linhas / size;

    int* parte = (int*)malloc(lin_part * colunas * sizeof(int*));

    int quant = lin_part * colunas;
    int branco = max_val / 2;

    // printf("branco %d \n", branco);
    // printf("linparte %d \n", lin_part);
    // printf("quant %d \n", quant);
    // printf("colunas %d \n", colunas);

    if(rank == 0){
        //printf("iniciando processo %d\n", rank);
        
        //int imagem[linhas][colunas];
        int* imagem = (int*)malloc(linhas * colunas * sizeof(int*));
        
        for(int i = 0; i < linhas; i++){
            for(int j = 0; j < colunas; j++){
                fscanf(file, "%d", &imagem[i * colunas + j]);
            }
        }

        //printf("iniciando envios para escravos\n");
        int ip = lin_part;
        for(int r = 1; r < size; r++){ // mestre enviando as parte para os escravos
            int linhaA = 0;
            for(int i = ip; i <(ip+lin_part); i++){
                for(int j = 0; j < colunas; j++){
                    parte[linhaA * colunas + j] = imagem[i * colunas + j];
                }
                linhaA++;
            }
            //printf("Enviando para escravos %d\n", r);
            MPI_Send(parte, quant, MPI_INT, r, 0, MPI_COMM_WORLD);
            //printf("matriz enviada para escravo %d\n", r);

            ip = ip+ lin_part;
        }

        //printf("Envios prontos, inciando busca");
        for(int i = 0; i < lin_part; i++){ // mestre contando sua parte
            for(int j = 0; j < colunas; j++){
                if(imagem[i * colunas + j] >= branco){
                    if(verifica(imagem, i, j, lin_part, colunas, branco)){
                        //printf("ACHAMOS UMA ESTRELA no processo %d\n", rank);
                        pinta(imagem, i, j, lin_part, colunas, branco);
                        cont++;
                    }
                }
            }
        }
        //printf("cont %d\n", cont);
        free(imagem);


    }else{
        //printf("iniciando processo %d\n", rank);

        int* parte2 = (int*)malloc(lin_part * colunas * sizeof(int*));


        MPI_Recv(parte2, quant, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for(int i = 0; i < lin_part; i++){ // mestre contando sua parte
            //printf("teste 1 %d\n", parte2[i][0]);
            for(int j = 0; j < colunas; j++){
                //printf("teste 2 %d\n", parte2[i][j]);
                if(parte2[i * colunas + j] >= branco){
                    if(verifica(parte2, i, j, lin_part, colunas, branco)){
                        //printf("ACHAMOS UMA ESTRELA no processo %d\n", rank);
                        pinta(parte2, i, j, lin_part, colunas, branco);
                        cont++;
                    }
                }
            }
        }

        free(parte2);

        MPI_Send(&cont, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

   

    if(rank == 0){
        int result;
        int soma_cont = cont;
        for(int r = 1; r < size; r++){
            MPI_Recv(&result, 1, MPI_INT, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            soma_cont = soma_cont + result;
        }

        printf("Foram encontradas %d estrelas.\n", soma_cont);
    }
    free(parte);
    MPI_Finalize();
    return 0;
}