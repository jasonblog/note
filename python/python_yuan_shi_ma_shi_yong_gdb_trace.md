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
(gdb) rbreak tupleobject.c:.
Breakpoint 1 at 0x486cb8: file Objects/tupleobject.c, line 893.
int PyTuple_ClearFreeList(void);
Breakpoint 2 at 0x486d49: file Objects/tupleobject.c, line 918.
void PyTuple_Fini(void);
Breakpoint 3 at 0x484d4a: file Objects/tupleobject.c, line 124.
PyObject *PyTuple_GetItem(PyObject *, Py_ssize_t);
Breakpoint 4 at 0x485b71: file Objects/tupleobject.c, line 428.
PyObject *PyTuple_GetSlice(PyObject *, Py_ssize_t, Py_ssize_t);
Breakpoint 5 at 0x484b34: file Objects/tupleobject.c, line 53.
PyObject *PyTuple_New(Py_ssize_t);
Breakpoint 6 at 0x485146: file Objects/tupleobject.c, line 186.
PyObject *PyTuple_Pack(Py_ssize_t, ...);
Breakpoint 7 at 0x484dbf: file Objects/tupleobject.c, line 140.
int PyTuple_SetItem(PyObject *, Py_ssize_t, PyObject *);
Breakpoint 8 at 0x484d0c: file Objects/tupleobject.c, line 113.
Py_ssize_t PyTuple_Size(PyObject *);
Breakpoint 9 at 0x484f85: file Objects/tupleobject.c, line 164.
void _PyTuple_MaybeUntrack(PyObject *);
Breakpoint 10 at 0x486921: file Objects/tupleobject.c, line 844.
int _PyTuple_Resize(PyObject **, Py_ssize_t);
Breakpoint 11 at 0x4868e1: file Objects/tupleobject.c, line 760.
static PyObject *tuple_getnewargs(PyTupleObject *);
Breakpoint 12 at 0x4870e1: file Objects/tupleobject.c, line 1028.
static PyObject *tuple_iter(PyObject *);
Breakpoint 13 at 0x4863f8: file Objects/tupleobject.c, line 646.
static PyObject *tuple_new(PyTypeObject *, PyObject *, PyObject *);
Breakpoint 14 at 0x4864ae: file Objects/tupleobject.c, line 667.
static PyObject *tuple_subtype_new(PyTypeObject *, PyObject *, PyObject *);
Breakpoint 15 at 0x485bd3: file Objects/tupleobject.c, line 442.
static PyObject *tupleconcat(PyTupleObject *, PyObject *);
Breakpoint 16 at 0x485974: file Objects/tupleobject.c, line 376.
```

```sh
(gdb) python
>tup1 = (1,2,3)
>end
(gdb) r
Starting program: /home/shihyu/Python-2.7.13/python 
[Thread debugging using libthread_db enabled]
Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".

Breakpoint 5, PyTuple_New (size=0) at Objects/tupleobject.c:53
```


```c
  48│ PyObject *
  49│ PyTuple_New(register Py_ssize_t size)
  50│ {
  51│     register PyTupleObject *op;
  52│     Py_ssize_t i;
  53│     if (size < 0) {
  54│         PyErr_BadInternalCall();
  55│         return NULL;
  56│     }
  57│ #if PyTuple_MAXSAVESIZE > 0
  58│     if (size == 0 && free_list[0]) {
  59│         op = free_list[0];
  60│         Py_INCREF(op);
  61│ #ifdef COUNT_ALLOCS
  62│         tuple_zero_allocs++;
  63│ #endif
  64│         return (PyObject *) op;
  65│     }
  66│     if (size < PyTuple_MAXSAVESIZE && (op = free_list[size]) != NULL) {
  67│         free_list[size] = (PyTupleObject *) op->ob_item[0];
  68│         numfree[size]--;
  69│ #ifdef COUNT_ALLOCS
  70│         fast_tuple_allocs++;
  71│ #endif

```





