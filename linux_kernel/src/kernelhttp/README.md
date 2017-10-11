# kernelhttp


# Usage for server.ko

kernel server with python

```bash
make
sudo insmod ./server.ko
python url.py
```

# Usage for server_npy.ko

kernel server without python

```bash
make NO_PY=1
sudo insmod ./server_npy.ko
```
# Connect

Use browser to connect `http://localhost:8888/` with kernel web server.

You can also connect `http://localhost:9999` for server created by python only.
