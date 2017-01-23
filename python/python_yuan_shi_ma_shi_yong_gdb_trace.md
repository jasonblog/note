# Python 原始碼 使用 gdb trace

- source code download

```sh
https://www.python.org/downloads/release/python-2713/
```

- build

```sh
./configure --with-pydebug
make -j8
```

- using gdb

```sh
cgdb -q ./python
```

```sh
(gdb) rbreak ceval.c:.
Breakpoint 1 at 0x4ce23f: file Python/ceval.c, line 261.
void PyEval_AcquireLock(void);
Breakpoint 2 at 0x4ce278: file Python/ceval.c, line 273.
void PyEval_AcquireThread(PyThreadState *);
Breakpoint 3 at 0x4de11d: file Python/ceval.c, line 4201.
PyObject *PyEval_CallObjectWithKeywords(PyObject *, PyObject *, PyObject *);
Breakpoint 4 at 0x4ce87f: file Python/ceval.c, line 669.
PyObject *PyEval_EvalCode(PyCodeObject *, PyObject *, PyObject *);
Breakpoint 5 at 0x4db11e: file Python/ceval.c, line 3352.
PyObject *PyEval_EvalCodeEx(PyCodeObject *, PyObject *, PyObject *,
    PyObject **, int, PyObject **, int, PyObject **, int, PyObject *);
Breakpoint 6 at 0x4ce8be: file Python/ceval.c, line 685.
PyObject *PyEval_EvalFrame(PyFrameObject *);
Breakpoint 7 at 0x4ce8f2: file Python/ceval.c, line 690.
PyObject *PyEval_EvalFrameEx(PyFrameObject *, int);
Breakpoint 8 at 0x4ddf5d: file Python/ceval.c, line 4117.
PyObject *PyEval_GetBuiltins(void);
Breakpoint 9 at 0x4ce1b9: file Python/ceval.c, line 225.
PyObject *PyEval_GetCallStats(PyObject *);
Breakpoint 10 at 0x4ddfec: file Python/ceval.c, line 4147.
```


```sh
(gdb) python
>tup1 = (1,2,3,4,5)
>end
(gdb) r
Starting program: /home/shihyu/Python-2.7.13/python 
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".

Breakpoint 13, PyEval_GetGlobals () at Python/ceval.c:4137
(gdb) 
```





