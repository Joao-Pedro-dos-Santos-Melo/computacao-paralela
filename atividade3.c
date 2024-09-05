#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Atualizar o scatter para scatterv por dar erro em alguns tamanhos de matriz e divisoes de processos!!!!!!!!

//#define N 4  // Dimensão das matrizes (N x N)

// Gera duas matrizes de inteiros com numeros aleatoris de 0 a 9 de tamanho a ser definido na execuçao
// Faz a soma das duas matriz em paralelismo e retorna em uma terceira matriz

void print_matrix(int* matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%d ", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

int main(int argc, char** argv) {
    int rank, size;

    int N = atoi(argv[1]);
    srand(time(NULL));

    MPI_Init(&argc, &argv);  // Inicializa o MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Rank do processo
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Número total de processos

    int matA[N][N], matB[N][N], matC[N][N];  // Matrizes A, B e C
    int* sub_matA;
    int* sub_matB;
    int* sub_matC;
    int num_elements_per_proc = (N * N) / size;  // Elementos por processo

    if (rank == 0) {
        // Inicializa as matrizes A e B no processo mestre
        printf("Matriz A:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matA[i][j] = rand() % 10;  // Preencher com valores quaisquer
                matB[i][j] = rand() % 10;  // Preencher com valores quaisquer
            }
        }
        print_matrix((int*)matA, N, N);
        printf("Matriz B:\n");
        print_matrix((int*)matB, N, N);
    }

    // Aloca espaço para submatrizes
    sub_matA = (int*)malloc(sizeof(int) * num_elements_per_proc);
    sub_matB = (int*)malloc(sizeof(int) * num_elements_per_proc);
    sub_matC = (int*)malloc(sizeof(int) * num_elements_per_proc);

    // Distribui partes das matrizes A e B para todos os processos
    MPI_Scatter(matA, num_elements_per_proc, MPI_INT, sub_matA, num_elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(matB, num_elements_per_proc, MPI_INT, sub_matB, num_elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

    // Soma local das submatrizes
    for (int i = 0; i < num_elements_per_proc; i++) {
        sub_matC[i] = sub_matA[i] + sub_matB[i];
    }

    // Recolhe os resultados de volta no processo mestre
    MPI_Gather(sub_matC, num_elements_per_proc, MPI_INT, matC, num_elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

    // O processo mestre imprime a matriz resultante
    if (rank == 0) {
        printf("Matriz C (soma de A + B):\n");
        print_matrix((int*)matC, N, N);
    }

    // Libera a memória alocada
    free(sub_matA);
    free(sub_matB);
    free(sub_matC);

    MPI_Finalize();  // Finaliza o MPI
    return 0;
}