# NVIM



## 編譯

```bash
# 關閉  anaconda 因為編譯環境會導向 anaconda lib
conda deactivate  

sudo apt-get install ninja-build \
     gettext libtool libtool-bin \
     autoconf automake cmake g++ \
     pkg-config unzip xsel

git clone https://github.com/neovim/neovim.git
cd neovim
git checkout stable
make CMAKE_EXTRA_FLAGS="-DCMAKE_INSTALL_PREFIX=$HOME/.mybin/nvim" CMAKE_BUILD_TYPE=Release  -j8
make install

cd ~/.mybin/nvim/share/nvim/runtime/autoload
wget https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim

# copy vimrc to /home/shihyu/.config/nvim/init.vim   
 /home/shihyu/.config/nvim/init.vim   
```



```bash
No "python3" provider found. Run :checkhealth provider
pip install --user --upgrade pynvim


# node 要新版的不然 coc 會報錯
sudo n 16 

// Enable Copy and Paste 
sudo apt-get install xclip xsel
```

---



