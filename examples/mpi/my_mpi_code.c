#include <stddef.h>
#include <mpi.h>

int main(int argc, char** argv)
  {
    MPI_Init(NULL, NULL); // Init MPI (init MPI_COMM_WORLD communicator, set rank to each process, etc)

    int nb_mpi_processes;
    MPI_Comm_size(MPI_COMM_WORLD, &nb_mpi_processes); // Ask the number of MPI processes running

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Ask the rank of the current process

    char hostname[256];
    int hostname_len;
    MPI_Get_processor_name(hostname, &hostname_len); // Ask the name of the host the process is running on

    printf("Hello world I am process %d on %d processes. I am running on %s\n",rank, nb_mpi_processes, hostname);

    MPI_Finalize(); // Close MPI

    return 0;
  }
