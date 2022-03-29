import sys
import pandas as pd

def test():
    data = [1,2,3,4]
    df = pd.DataFrame(data, columns=["a"])
    print(df)

def fibo(n):
    if n == 0: return 0
    elif n == 1: return 1
    return fibo(n-1) + fibo(n-2)

if __name__ == '__main__':
    print(fibo(int(sys.argv[1])))
    test()
