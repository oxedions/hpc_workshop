#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>
    
int main(int argc, char** argv)
{
    int n = 1000000;
    #pragma omp parallel shared(n) default(shared)
    {
        printf("Hello from thread: %d\n", omp_get_thread_num());

        # pragma omp for
            for (int i = 0; i < n; i++ )
            {
                //printf("Iteration %d\n", i);
                usleep(10);
            }
    }
    return 0;
}
