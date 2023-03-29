#!/usr/bin/env python3
import time

def heavycalculations(n):
  time.sleep(n)
  return 1

if __name__ == "__main__":

    nsize = 10
    numbers = list(range(0, nsize, 1))
    print(len(numbers))
    result = list(range(0, nsize, 1))
    for n in range(0, nsize):
        numbers[n] = 2

    result = map(heavycalculations, numbers)
#    for n in range(0, nsize - 1):
#        result[n] = is_prime(numbers[n])
    
    print(sum(list(result)))
    # print(list(zip(numbers, result)))
