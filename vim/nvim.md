# NVIM



## 編譯

```bash
# 關閉  anaconda 因為編譯環境會導向 anaconda lib
conda deactivate  

sudo apt-get install ninja-build \
     gettext libtool libtool-bin \
     autoconf automake cmake g++ \
     pkg-config unzip

git clone https://github.com/neovim/neovim.git
cd neovim
git checkout stable
make CMAKE_EXTRA_FLAGS="-DCMAKE_INSTALL_PREFIX=$HOME/.mybin/nvim" CMAKE_BUILD_TYPE=Release  -j8
make install
```



```bash
No "python3" provider found. Run :checkhealth provider
pip install --user --upgrade pynvim
```

