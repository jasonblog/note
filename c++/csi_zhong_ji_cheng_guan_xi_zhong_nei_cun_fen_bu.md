# C++四种继承关系中内存分布


###1、单一继承不含虚函数

在这种情况下，对象会保持基类的原样性，并且一个class derived class object表现出来的东西，是其自己的members加上其base classes members的总和。


```cpp
class Point2d  
{  
    int a;  
    char b;  
};  
  
class Point3d : public Point2d  
{  
    char c;  
};  
```

对于上述类，其模型如下：

![](images/20160907170721505.png)
![](images/20160907172519996.png)
![](images/20160907173017067.png)
![](images/20160908193138438.png)
![](images/20160909102740041.png)
