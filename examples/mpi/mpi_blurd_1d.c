#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <mpi.h>

   // Grouped Calculs program

int main(int argc, char** argv)
  {
    MPI_Init(NULL, NULL);
    int nb_mpi_processes;
    MPI_Comm_size(MPI_COMM_WORLD, &nb_mpi_processes);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(nb_mpi_processes != 2) { printf("This program is design to be run with 2 processes only"); return 0;}

    int Nx1=20;
    int Nx1l = Nx1/2;
    double * Field = malloc(Nx1l * sizeof(double));
    double * Field_buff = malloc(Nx1l * sizeof(double));
    int i;

    for(i = 0; i < Nx1l; ++i)
        Field[i] = 0.0;

    if(rank==0) {Field[Nx1l-3] = 10.0; Field[Nx1l-2]=10.0;}

    char fileName[2048];
    sprintf(fileName, "IN.%d.dat", rank);

    FILE *file = NULL;
    file = fopen(fileName, "w");
    for(i = 1; i < Nx1l-1; ++i)
        fprintf(file, "%d %lf\n", i + (rank * Nx1l) - 2 * rank, Field[i]);
    fclose(file);

    int niter=8;
    int n;
    int tag = 7777;
    for(n = 1; n <= niter; ++n)
    {
        if(rank==0)
        {
            MPI_Sendrecv ( &Field[Nx1l-2] , 1 , MPI_DOUBLE , 1 , tag , &Field[Nx1l-1] , 1 , MPI_DOUBLE , 1 , tag , MPI_COMM_WORLD , MPI_STATUS_IGNORE);
        }
        if(rank==1)
        {
            MPI_Sendrecv ( &Field[1] , 1 , MPI_DOUBLE , 0 , tag, &Field[0] , 1 , MPI_DOUBLE , 0 , tag , MPI_COMM_WORLD , MPI_STATUS_IGNORE);
        }

        for(i = 1; i < Nx1l-1; ++i)
            Field_buff[i] = ( Field[i-1] + Field[i] + Field[i+1] ) / 3.0;

        for(i = 1; i < Nx1l-1; ++i)
            Field[i] =  Field_buff[i];
    }

    sprintf(fileName, "OUT.%d.dat", rank);
    file = fopen(fileName, "w");
    for(i = 1; i < Nx1l-1; ++i)
        fprintf(file, "%d %lf\n", i + (rank * Nx1l) - 2 * rank, Field[i]);
    fclose(file);

    MPI_Finalize();

    return 0;
  }

