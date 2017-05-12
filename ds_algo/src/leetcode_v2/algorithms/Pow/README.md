## Pow(x,n)

Implement pow(x, n).

## Solution

由幂的关系X<sup>n</sup> == X<sup>n/2</sup> * X<sup>n/2</sup>,因此可以采用
二分思想计算.

先解决特殊值:

* `n == 0`, 如果`x == 0`, 没意义，`x != 0` 返回`1`
* `n == 1, return x`
* `n == -1`, 这个需要看`x`, 如果`x != 0, return 1/x`,如果`x == 0`无意义
* 另外对于优化，当`x`为`0,1,-1`时直接可以输出


对于|n| > 1的情况:

* 若n是奇数, `return x * pow(x, (n-1) / 2) * pow(x, (n-1) / 2)`;
* 若n是偶数, `return pow(x, n / 2) * pow(x, n / 2)`;


## 需要注意的地方

1. 当`n < 0`, 可以递归调用`1/pow(x, -n)`,但是当`n == INT_MIN`时，`-n`会溢出，因此应该先二分再处理负数情况，直到`n == -1`.
2. 浮点数在计算机中不能精确表示，因此不能直接通过`==`比较，比如`x == 0`是错误的写法，应该写成`fabs(x - 0) < ERROR,ERROR为误差,可以设置为1e-8`

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
		if (fabs(x - 0) < ERROR) { // x位0 1/0 没有意义
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
