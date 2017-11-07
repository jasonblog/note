# 三角函數



![](images/6800492262706.jpg)

![](images/T3.htm1.gif)

![](images/maxresdefault.jpg)


---

### python

- sin：求正弦；傳入弧度值
- cos：求餘弦；傳入弧度值
- tan：求正切；傳入弧度值


```py
import math
 
a = math.pi/6   # math.pi = 3.141592653589793
 
math.sin(a)   # 0.49999999999999994
math.cos(a)   # 0.8660254037844387
math.tan(a)   # 0.5773502691896256
```

hypot(a,b)：a,b是直角三角形的兩個對角邊，求斜邊長；使用勾股定理，等同於sqrt(a*a + b*b)


```py
math.hypot(3,4)      # 5.0
math.sqrt(3*3 + 4*4) # 5.0
```

- degrees：把弧度值轉換為角度
- radians：把角度值轉換為弧度


```py
math.degrees(math.pi/6)  # 29.999999999999996
math.radians(30)         # 0.5235987755982988
```