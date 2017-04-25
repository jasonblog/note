# so file crash  using addr2line




```sh
env LD_DEBUG=files LD_DEBUG_OUTPUT=myld.log ./main
```


```py
hex (0x00007fa1a7fcf6b4  - 0x00007fa1a7fcf000) = 0x6b4
```


```sh
addr2line -e libs/libdl.so 0x6b4  -f


print
??:?
```