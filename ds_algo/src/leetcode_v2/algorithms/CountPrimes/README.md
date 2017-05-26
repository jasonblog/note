Description:

Count the number of prime numbers less than a non-negative number, n

click to show more hints.

References:
How Many Primes Are There?

Sieve of Eratosthenes

Credits:
Special thanks to @mithmatt for adding this problem and creating all test cases.

## Solution

使用[埃拉託斯特尼篩法](http://en.wikipedia.org/wiki/Sieve_of_Eratosthenes), 具體步驟是

![Sieve\_of\_Eratosthenes](img/Sieve_of_Eratosthenes_animation.gif)

> A prime number is a natural number that has exactly two distinct natural number divisors: 1 and itself.
> 
> To find all the prime numbers less than or equal to a given integer n by Eratosthenes' method:
> 
> Create a list of consecutive integers from 2 through n: (2, 3, 4, ..., n).
> Initially, let p equal 2, the first prime number.
> Starting from p, enumerate its multiples by counting to n in increments of p, and mark them in the list (these will be 2p, 3p, 4p, ... ; the p itself should not be marked).
> Find the first number greater than p in the list that is not marked. If there was no such number, stop. Otherwise, let p now equal this new number (which is the next prime), and repeat from step 3.

```
Input: an integer n > 1
 
Let A be an array of Boolean values, indexed by integers 2 to n,
initially all set to true.
 
 for i = 2, 3, 4, ..., not exceeding √n:
  if A[i] is true:
    for j = i2, i2+i, i2+2i, i2+3i, ..., not exceeding n :
      A[j] := false
 
Output: all i such that A[i] is true.
```


## Code
```c
int countPrimes(int n)
{
	vector<bool> flags(n, true);
	flags[0] = false;
	flags[1] = false;
	int sqr = sqrt(n - 1);
	for (int i = 2; i <= sqr; ++i) {
		if (flags[i]) {
			for (int j = i * i; j < n; j += i)
				flags[j] = false;
		}
	}
	return count(flags.begin(), flags.end(), true);
}
```

## 關於素數

參見[How Many Primes Are There?](https://primes.utm.edu/howmany.html#pi_def).

設pi(x)表示小於等於x的素數個數，主要有3個關於素數結論:

1. pi(x)約等於`x / (logx - 1)
2. 第n個素數的值約為`nlogn`
3. x是素數的概率約為`1/logx`
