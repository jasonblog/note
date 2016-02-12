# 安裝 NodeJS、NPM

- source code & build

```sh
git clone https://github.com/joyent/node.git && cd node
git br -a 
git checkout vxx.xx

./configure --prefix=/usr/local/node

```

-  ~/.bashrc

```sh
export PATH=/usr/local/node/bin:$PATH
export NODE_PATH=/usr/local/node:/usr/local/node/lib/node_modules

```


```sh
node -v  
npm -v  
```


