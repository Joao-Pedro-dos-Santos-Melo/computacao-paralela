#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//João Pedro dos Santos Melo


void print_matrix(int* matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%d ", matrix[i * size + j]);
        }
        printf("\n");
    }
}

int main(int argc, char** argv) {
    int rank, size;

    srand(time(NULL));

    MPI_Init(NULL, NULL);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Rank do processo
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Número total de processos


    int matriz1[size][size];
    int matriz2[size][size];
    int matrizR[size][size];
    int tam_matriz = size * size;

    if (rank == 0) {
        // Inicializa as matrizes 1 e 2 no processo mestre
        printf("Matriz 1:\n");
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                matriz1[i][j] = rand() % 5;  // Preencher com valores quaisquer
                matriz2[i][j] = rand() % 5;  // Preencher com valores quaisquer
            }
        }
        print_matrix((int*)matriz1, size);
        printf("Matriz 2:\n");
        print_matrix((int*)matriz2, size);
        for(int i = 1; i < size; i++){
            MPI_Send(matriz2, tam_matriz, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }else{
        MPI_Recv(matriz2, tam_matriz, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }


    //crio o vetor que vai receber os valores da matriz1 e o vetor que vai levar o resultados para o processo 0
    int* vetor = (int*)malloc(sizeof(int) * size);
    int* vetorR = (int*)malloc(sizeof(int) * size);
    

    MPI_Scatter(matriz1, size, MPI_INT, vetor, size, MPI_INT, 0, MPI_COMM_WORLD);

    
    // printf("Processo : %d\nRecebeu o vetor: ", rank);
    // for(int i =0; i < size; i++){
    //     printf("%d ", vetor[i]);
    // }
    // printf("\n");
    // printf("matriz 2:\n");
    // print_matrix((int*)matriz2, size);
    // printf("\n--------------------\n");

    //fazendo a multiplicacao dos valores do vetor recebido pela matriz2 tbm recebida
    int result;
    for(int j = 0; j < size; j++){
        result = 0;
        for(int i = 0; i < size; i++){
            result = result + vetor[i] * matriz2[i][j];
            //printf("Processo %d :%d = %d * %d\n",rank, result, vetor[i], matriz2[i][j]);
        }
        vetorR[j] = result;
    }
    // printf("Processo : %d\nvai enviar o vetor: ", rank);
    // for(int i =0; i < size; i++){
    //     printf("%d ", vetorR[i]);
    // }
    // printf("\n");


    //Devolvo o vetoror com os resultados mpara a matrizR
    MPI_Gather(vetorR, size, MPI_INT, matrizR, size, MPI_INT, 0, MPI_COMM_WORLD);
    
    // O processo mestre imprime a matriz resultante
    if (rank == 0) {
        printf("Matriz R (Multiplicação de 1 * 2):\n");
        print_matrix((int*)matrizR, size);
    }

    free(vetor);
    free(vetorR);


    MPI_Finalize();  // Finaliza o MPI
    return 0;
}