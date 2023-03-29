import dask
from dask.distributed import Client, LocalCluster
import time
import os
import random

def costly_simulation(list_param):
    time.sleep(random.random())
    return list_param * 2

if __name__ == "__main__":

    client = Client('localhost:8786')

    input_array = [1] * 200

    futures = []
    for parameters in input_array:
        future = client.submit(costly_simulation, parameters, pure=False)
        futures.append(future)

    results = client.gather(futures)
    print(results[:])

