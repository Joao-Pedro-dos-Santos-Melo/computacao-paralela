#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//no programa acima, realiza M vezes

int main(int argc, char** argv){
    MPI_Init(NULL, NULL);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    // if(world_size != 2){
    //     fprintf(stderr, "Must use two processes for this example\n");
    //     MPI_Abort(MPI_COMM_WORLD, 1);
    // }
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    srand(time(NULL));
    if(world_rank == 0){

        int result;
        int soma = 0;
        int numeros[2000];

        for(int i = 1; i < world_size; i++){
            int qn = (rand() % 1001) + 1000;
            for(int j = 0; j < qn; j++){
                numeros[j] = rand() % 100;
            }
            MPI_Send(&qn, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(numeros, 2000, MPI_INT, i, 0, MPI_COMM_WORLD);

        }
        int salva;
        for (int i = 0; i <= ((world_size -1)*5); i++) {
            MPI_Status status;
            MPI_Recv(&result, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printf("Processo 0 recebeu valor %d do processo %d\n", result, status.MPI_SOURCE);
            soma += result;
            
                int qn = (rand() % 1001) + 1000;
                for(int j = 0; j < qn; j++){
                    numeros[j] = rand() % 100;
                }
                printf("Enviando para o processo %d\n", status.MPI_SOURCE);
                salva = status.MPI_SOURCE;
                MPI_Send(&qn, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
                MPI_Send(numeros, 2000, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
            
        }

        for(int i = 1; i < world_size; i++){
            int x = 0;
            MPI_Send(&x, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
        }
        //MPI_Send(0, 1, MPI_INT, salva, 1, MPI_COMM_WORLD);
        
        printf("Soma total: %d\n", soma);
        
    }else{
        while(1){
            MPI_Status status;
            int qn2;
            int num[2000];
            MPI_Recv(&qn2, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            if(status.MPI_TAG == 1){
                break;
            }
            MPI_Recv(num , 2000, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            
            //MPI_Get_count(&status, MPI_INT, &number_amount);
            int soma2 = 0;
            for(int i = 0; i < qn2; i++){
                soma2 += num[i];
            }
            MPI_Send(&soma2, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            
        }

    }
    MPI_Finalize();
}