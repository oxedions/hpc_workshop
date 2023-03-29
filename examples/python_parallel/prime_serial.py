#!/usr/bin/env python3

def is_prime(num):

  if num == 1:
      return 0
  elif num > 1:
      # check for factors
      for i in range(2,num):
          if (num % i) == 0:
              return 1
  else:
      return 0
  return 0

if __name__ == "__main__":

    nsize = 100000
    numbers = list(range(1, nsize, 1))
    result = list(range(1, nsize, 1))

    result = map(is_prime, numbers)
#    for n in range(0, nsize - 1):
#        result[n] = is_prime(numbers[n])
    
    print("Number of primes found: " + str(sum(result)))
    # print(list(zip(numbers, result)))
