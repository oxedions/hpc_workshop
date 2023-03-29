import dask
from dask.distributed import Client

import dask.array as da

if __name__ == "__main__":

    x = da.random.random((10000, 10000), chunks=(1000, 1000))

    client = Client('localhost:8786')

    y = x + x.T
    z = y[::2, 5000:].mean(axis=1)
    print(z.compute())


