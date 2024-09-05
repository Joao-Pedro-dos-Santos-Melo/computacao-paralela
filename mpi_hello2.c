#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	MPI_Init(NULL, NULL);

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	char processer_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;
	MPI_Get_processor_name(processer_name, &name_len);



	//printf("Hello world from processor %s, rank %d out of %d processors\n", processer_name, world_rank, world_size);

	MPI_Barrier(MPI_COMM_WORLD);
	//printf("Finalizing process %d out of %d processors\n", world_rank, world_size);

	int number;
	number = world_rank;
	//printf("Numero a ser enviado: %d\n", (number-1));
	if (world_rank == 0){
		//(mensagem, quantos, tipo, quem recebe, tecnica, comunicador)
		MPI_Ssend(&number, 1, MPI_INT, world_rank+1, 0, MPI_COMM_WORLD);
		MPI_Recv(&number, 1, MPI_INT, world_size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Process %d received number %d from other process\n", world_rank, number);
	}else{
		
		MPI_Recv(&number, 1, MPI_INT, world_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Ssend(&number, 1, MPI_INT, (world_rank+1)%world_size, 0, MPI_COMM_WORLD);
		printf("Process %d received number %d from other process\n", world_rank, number);
	}
	
	MPI_Finalize();
}
