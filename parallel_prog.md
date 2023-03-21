# Parallel computing

This practical session objective is to make user aware of his environment and how to use some parallel computing standard.

## Environment

Connect via ssh to your remote system. Note that you can add -vvv flags to the ssh connection to get some details about the ssh connection mechanism, and dialog between client and server.

Once on the Linux system, first get the number of cores available and CPU details, using command: `cat /proc/cpuinfo`

The result shows you multiple things:

First, the number of processor entries reflect the number of cores available.
For example:

```
:~$ cat /proc/cpuinfo | grep '^processor'
processor       : 0
processor       : 1
processor       : 2
processor       : 3
```

Shows that there are 4 cores on this system.
Another interesting value is the physical id. The following command allows you to know how much CPU socket your server is equipped with:

```
:~$ cat /proc/cpuinfo | grep "physical id" | sort -u | wc -l
1
```

You can also get SIMD available instructions on this CPU by having a look at the flags key output:

```
:~$ cat /proc/cpuinfo | grep flags
```

Well known SIMD instructions are AVX, FMA, SSE, MMX, etc.

The command `lscpu` also allows you to get details, and check if HyperThreading is active or not (Threads per core).

It is important to know available CPU cores resources (and how these are distributed physically) and available SIMD at disposal, to get maximum performances of current system.

Note that using the following commands, you can get details on other hardware:

* `lspci` (if installed) shows PCI devices, and so GPU and Infiniband cards.
* `free -h` shows available memory
* `df -h` shows available disk

## OpenMP

OpenMP is natively embedded in GCC. Install GCC and all needed tools:

```
dnf groupinstall "Development Tools" -y
```

Then create a new file called my_openmp_code.c with the following content:

```C
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    int n = 10;
    #pragma omp parallel shared(n) default(shared)
    {
        printf("Hello from process: %d\n", omp_get_thread_num());

        # pragma omp for
            for (int i = 0; i < n; i++ )
            {
                printf("Iteration %d\n", i);
                sleep(2);
            }
    }
    return 0;
}
```

And compile it with gcc using the -fopenmp flag:

gcc my_openmp_code.c -fopenmp

You will get an executable a.out.

Ask system to only use 1 thread, and launch the executable with a time to know how much time program execution took:

```
export OMP_NUM_THREADS=1
:~$ time ./a.out
Hello from process: 0
Iteration 0
Iteration 1
Iteration 2
Iteration 3
Iteration 4
Iteration 5
Iteration 6
Iteration 7
Iteration 8
Iteration 9

real    0m20.002s
user    0m0.002s
sys     0m0.000s
```

You can see it took 20s, as expected (2s per iteration, for 10 iterations).

Now lets request 4 threads:

```
export OMP_NUM_THREADS=4
:~$ time ./a.out
Hello from process: 0
Iteration 0
Hello from process: 1
Iteration 3
Hello from process: 2
Iteration 6
Hello from process: 3
Iteration 8
Iteration 1
Iteration 4
Iteration 7
Iteration 9
Iteration 2
Iteration 5

real    0m6.002s
user    0m0.012s
sys     0m0.000s
```

You can see that total time was reduced to 6s. This is logical: 4 + 4 + 2, so during last part of execution, 2 threads where idle while 2 threads were executing the sleep.

Etc.

OpenMP is a simple and convenient way to parallelize C/C++ and Fortran codes.
A lot of tunings are available to ensure a proper scaling. For example, when dealing with a large amount of small time consuming tasks per loop, adjusting scheduler strategy with a static chunk size greater than 1 can significantly reduce execution time.

## MPI

MPI main objective is to distribute memory and exchange between nodes to achieve large calculations.
MPI can also be used locally to communicate between local processes.

Unlike OpenMP, MPI need additional libraries and runtime to build and execute.

Install first required dependencies:

```
sudo apt-get install openmpi........
```

Then, create a new file called my_mpi_code.c with the following content:


```


```


And this time, build it with openmpi wrapper:

```
mpicc my_mpi_code.c
```

And execute it with the mpirun runtime:

```
mpirun -n 4 ./a.out
```

You should see that 4 isolated processes were created. You can also see that they do not share the same memory, as their values of `a` variable are different.

Now, in this example, we are going to calculate a very simple 1D blur to expose basic real case usage.


Before going further, try to understand what this program does:

1. we create a basic array of data, that could be temperature, pressure, whatever. The whole field is set to 0.0 real value, except in the middle where there is a sharp value raise to 10.0.
2. we apply iteratively a filter on this field, that will simply "smooth" the filed (blur).

Build and execute this simple program now:

```
gcc my_mpi_code.c -o blur_1d
```

And execute it:

```
./blur_1d
```

You can see that after few iterations, the resulting field is a smoothed version of the initial field.

Now, lest say calculations are so long (imagine a very very long field) that we need to distribute calculations and memory arrays between nodes.

In this next step, we are going to split the 1D field in 4 pieces, and ensure communications between each process at each iteration so process can work on their part of the field, using other process values as frontiers values.

Update the code to:

```

```
