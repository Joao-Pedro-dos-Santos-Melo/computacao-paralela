#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
//ANEL
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

	// printf("Hello world from processor %s, rank %d out of %d processors\n", processer_name, world_rank, world_size);

	MPI_Barrier(MPI_COMM_WORLD);
	// printf("Finalizing process %d out of %d processors\n", world_rank, world_size);

	int number;
	number = world_rank;

	// if (world_rank != 0){
	// 	//(mensagem, quantos, tipo, quem recebe, tecnica, comunicador)
	// 	MPI_Send(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	// 	MPI_Recv(&number, 1, MPI_INT, world_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	// 	printf("Process %d received number %d from the process %d\n", world_rank, number, 0);
	// }else{
	// 	MPI_Send(&number, 1, MPI_INT, world_rank+1, 0, MPI_COMM_WORLD);
	// 	MPI_Recv(&number, 1, MPI_INT, world_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	// 	printf("Process %d received number %d from the process %d\n", world_rank, number, world_rank-1);
	// }
	int token;
	// Receive from the lower process and send to the higher process. Take care
	// of the special case when you are the first process to prevent deadlock.
	if (world_rank != 0)
	{
		MPI_Recv(&token, 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Process %d received token %d from process %d\n", world_rank, token,
			   world_rank - 1);
	}
	else
	{
		// Set the token's value if you are process 0
		token = -1;
	}
	MPI_Send(&token, 1, MPI_INT, (world_rank + 1) % world_size, 0,
			 MPI_COMM_WORLD);
	// Now process 0 can receive from the last process. This makes sure that at
	// least one MPI_Send is initialized before all MPI_Recvs (again, to prevent
	// deadlock)
	if (world_rank == 0)
	{
		MPI_Recv(&token, 1, MPI_INT, world_size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Process %d received token %d from process %d\n", world_rank, token, world_size - 1);
	}

	MPI_Finalize();
}
