# 採用離散積分的方法求圓周率，並著手透過 SIMD 指令作效能最佳化

Authored by Lee Chao Yuan `<charles620016@gmail.com>`

![](https://raw.githubusercontent.com/charles620016/embedded-fall2015/master/hw1-computePi/screenshot/integral.png)

## 相關連結

* [開發紀錄與效能分析](https://charles620016.hackpad.com/Charles-Week-1-kBMD0GhbC7d)
* [Leibniz formula for π](https://en.wikipedia.org/wiki/Leibniz_formula_for_%CF%80)

## Baseline 版本

```c
double computePi_v1(size_t N)
{
    double pi = 0.0;
    double dt = 1.0 / N;                // dt = (b-a)/N, b = 1, a = 0
    for (size_t i = 0; i < N; i++) {
        double x = (double) i / N;      // x = ti = a+(b-a)*i/N = i/N
        pi += dt / (1.0 + x * x);       // integrate 1/(1+x^2), i = 0....N
    }
    return pi * 4.0;
}
```

## Benchmarking

![](https://raw.githubusercontent.com/charles620016/embedded-fall2015/master/hw1-computePi/screenshot/time_command.png)

![](https://raw.githubusercontent.com/charles620016/embedded-fall2015/master/hw1-computePi/screenshot/Wall-clock_time.png)

![](https://raw.githubusercontent.com/charles620016/embedded-fall2015/master/hw1-computePi/screenshot/CPU_time.png)
