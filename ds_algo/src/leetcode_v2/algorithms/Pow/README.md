## Pow(x,n)

Implement pow(x, n).

## Solution

由冪的關係X<sup>n</sup> == X<sup>n/2</sup> * X<sup>n/2</sup>,因此可以採用
二分思想計算.

先解決特殊值:

* `n == 0`, 如果`x == 0`, 沒意義，`x != 0` 返回`1`
* `n == 1, return x`
* `n == -1`, 這個需要看`x`, 如果`x != 0, return 1/x`,如果`x == 0`無意義
* 另外對於優化，當`x`為`0,1,-1`時直接可以輸出


對於|n| > 1的情況:

* 若n是奇數, `return x * pow(x, (n-1) / 2) * pow(x, (n-1) / 2)`;
* 若n是偶數, `return pow(x, n / 2) * pow(x, n / 2)`;


## 需要注意的地方

1. 當`n < 0`, 可以遞歸調用`1/pow(x, -n)`,但是當`n == INT_MIN`時，`-n`會溢出，因此應該先二分再處理負數情況，直到`n == -1`.
2. 浮點數在計算機中不能精確表示，因此不能直接通過`==`比較，比如`x == 0`是錯誤的寫法，應該寫成`fabs(x - 0) < ERROR,ERROR為誤差,可以設置為1e-8`

## Code
```c
const double ERROR = 1e-8;
int doubleEQ(double x, double y)
{
	return fabs(x - y) < ERROR;
}
double myPow(double x, int n)
{
	if (n == 0) {
		return 1;
	}
	if (n == 1)
		return x;
	if (n == -1) {
		if (fabs(x - 0) < ERROR) { // x位0 1/0 沒有意義
			return INT_MAX;
		}
		return 1.0 / x;
	}
	if (doubleEQ(x, 0)){
		return 0;
	}
	if (doubleEQ(x, 1)) {
		return 1;
	}
	if (doubleEQ(x, -1)) {
		if ((n & 0x1))
			return -1;
		else return 1;
	}
	if ((n & 0x1)) {
		int mid = (n - 1) / 2;
		double half = myPow(x, mid);
		return half * half * x;
	} else {
		double half = myPow(x, n / 2);
		return half * half;
	}
}
```
