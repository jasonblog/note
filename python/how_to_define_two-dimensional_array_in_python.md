# How to define two-dimensional array in python


```py
w, h = 19, 10
raw_data = [['-' for x in range(w)] for y in range(h)] 
```

```sh

```


```py
>>> import numpy
>>> numpy.zeros((5, 5))
array([[ 0.,  0.,  0.,  0.,  0.],
       [ 0.,  0.,  0.,  0.,  0.],
       [ 0.,  0.,  0.,  0.,  0.],
       [ 0.,  0.,  0.,  0.,  0.],
       [ 0.,  0.,  0.,  0.,  0.]])
>>> numpy.matrix([[1, 2],[3, 4]])
matrix([[1, 2],
        [3, 4]])
```