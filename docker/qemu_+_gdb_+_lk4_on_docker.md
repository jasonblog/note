# QEMU + GDB + LK4 on Docker


## 安裝Docker
```sh
sudo apt install docker.io
sudo systemctl start docker
sudo systemctl enable docker
docker --version
sudo usermod -aG docker shihyu
```

### Docker 修改默認存儲位置

```sh
sudo /etc/init.d/docker stop

cd /var/lib
sudo mv docker /media/shihyu/ssd/docker
sudo ln -s  /media/shihyu/ssd/docker docker

sudo /etc/init.d/docker start
```

### 進同一個 Docker容器

```sh
docker ps 
docker exec -it CONTAINER ID /bin/bash
```

### 快速刪除多個 docker 容器的方法 
總結了2種批量刪除容器的方法

```sh
docker stop $(docker ps -qa)
docker rm $(docker ps -qa)

docker ps -qa | xargs -n 1 docker stop
docker ps -qa | xargs -n 1 docker rm

docker ps -a
docker stop c8fddb25161f && docker rm c8fddb25161f
```

### 刪除docker image

```sh
docker image rm IMAGE ID


// 強制刪除所有Image
docker rmi -f $(docker images -q)
```

### docker commit


```sh
docker ps
docker commit -m "qemu + gdb + linux kernel 4.0" CONTAINER ID  yshihyu/qemu_gdb_lk4
```


### 登入 Docker

```sh
docker login
ID: yshihyu
PW: 2l
```

### Push

```sh
docker images
docker push yshihyu/qemu_gdb_lk4
```

### ssh 方式登入
```sh
docker run -itd -p 10023:22 yshihyu/qemu_gdb_lk4
docker ps 

docker exec -it CONTAINER ID /bin/bash
```

## 建構環境 從0開始

- 下載ubuntu鏡像


```sh
docker pull ubuntu:18.04
```

- 啟動鏡像

```sh
docker run -ti ubuntu:18.04
```

- 安裝需要的套件

```sh

apt-get update
apt-get install qemu build-essential gcc g++ vim wget git automake libncurses5-dev flex bison texinfo subversion libreadline-dev bc libpython2.7-dev net-tools
```

```sh
cd ~/
wget https://releases.linaro.org/components/toolchain/binaries/latest-5/aarch64-linux-gnu/gcc-linaro-5.5.0-2017.10-x86_64_aarch64-linux-gnu.tar.xz


mkdir ~/.mybin
vim ~/.bashrc
export PATH=/root/gcc-linaro-5.5.0-2017.10-x86_64_aarch64-linux-gnu/bin:$PATH
export PATH=$HOME/.mybin:$PATH
export PATH=$HOME/.mybin/gdb_python3/bin:$PATH
source ~/.bashrc
```

- 下載 runninglinuxkernel_4

```sh
git clone https://github.com/figozhang/runninglinuxkernel_4.0

export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-
cd _install_arm64 && mkdir dev && cd dev
mknod console c 5 1 
cd runninglinuxkernel_4.0 
make defconfig
make -j8
```


### Anaconda3 & GDB & CGDB 安裝

- Anaconda3 安裝

```sh
wget https://repo.anaconda.com/archive/Anaconda3-5.2.0-Linux-x86_64.sh
sh Anaconda3-5.2.0-Linux-x86_64.sh
source ~/.bashrc
```

- GDB 安裝

```sh
git clone https://github.com/bminor/binutils-gdb
```

```sh

gdb 編譯新版修改  // 因為 gdb 會出現  'g' packet reply is too long:
修改gdb/remote.c文件，屏蔽process_g_packet函數中的下列兩行：
if (buf_len > 2 * rsa->sizeof_g_packet)
error (_(“Remote ‘g’ packet reply is too long: %s”), rs->buf);
在其後添加：

if (buf_len > 2 * rsa->sizeof_g_packet) {
    rsa->sizeof_g_packet = buf_len ;
    for (i = 0; i < gdbarch_num_regs (gdbarch); i++)
    {
        if (rsa->regs[i].pnum == -1)
            continue;

        if (rsa->regs[i].offset >= rsa->sizeof_g_packet)
            rsa->regs[i].in_g_packet = 0;
        else
            rsa->regs[i].in_g_packet = 1;
    }
}
```

```sh
export LDFLAGS="-Wl,-rpath,/root/anaconda3/lib -L/root/anaconda3/lib"

mkdir build && cd build

../configure --enable-targets=all \
             --enable-64-bit-bfd  \
             --with-python=python3.6 \
             --with-system-readline \
             --prefix=/root/.mybin/gdb_python3


make -j8 && make install

cd /root/.mybin/gdb_python3
svn co svn://gcc.gnu.org/svn/gcc/trunk/libstdc++-v3/python
```



- CGDB 安裝

`只能使用docker run 那個終端機開啟cgdb 不然會crash 原因不清楚` 

```sh
git clone https://github.com/cgdb/cgdb
```

```sh
cd cgdb
./autogen.sh
./configure
make 
cp cgdb/cgdb ~/.mybin
```

```sh
mkdir ~/.cgdb
~/.cgdb/cgdbrc
```

```sh
set arrowstyle=highlight
set autosourcereload
set ignorecase
set tabstop=4
set winminheight=2
set wrapscan

map <s>   :start<CR>
map <b>   :break<CR>
map <F5>  :run<CR>
map <F7>  imake<CR><Esc>
map <F8>  :continue<CR>
map <F10> :next<CR>
map <F11> :step<CR>
map <F12> :finish<CR>

hi Statement cterm=bold ctermfg=6
hi PreProc cterm=bold ctermfg=4
hi IncSearch ctermfg=2
hi LineHighlight cterm=bold ctermfg=3 ctermbg=4
```

### RUN + GDB

```sh
./run.sh arm64 debug


cgdb -d aarch64-linux-gnu-gdb vmlinux
target remote localhost:1234
b start_kernel
c
```

```sh
#   STL GDB evaluators/views/utilities - 1.03
#
#   The new GDB commands:                                                         
#         are entirely non instrumental                                             
#         do not depend on any "inline"(s) - e.g. size(), [], etc
#       are extremely tolerant to debugger settings
#                                                                                 
#   This file should be "included" in .gdbinit as following:
#   source stl-views.gdb or just paste it into your .gdbinit file
#
#   The following STL containers are currently supported:
#
#       std::vector<T> -- via pvector command
#       std::list<T> -- via plist or plist_member command
#       std::map<T,T> -- via pmap or pmap_member command
#       std::multimap<T,T> -- via pmap or pmap_member command
#       std::set<T> -- via pset command
#       std::multiset<T> -- via pset command
#       std::deque<T> -- via pdequeue command
#       std::stack<T> -- via pstack command
#       std::queue<T> -- via pqueue command
#       std::priority_queue<T> -- via ppqueue command
#       std::bitset<n> -- via pbitset command
#       std::string -- via pstring command
#       std::widestring -- via pwstring command
#
#   The end of this file contains (optional) C++ beautifiers
#   Make sure your debugger supports $argc
#
#   Simple GDB Macros writen by Dan Marinescu (H-PhD) - License GPL
#   Inspired by intial work of Tom Malnar, 
#     Tony Novac (PhD) / Cornell / Stanford,
#     Gilad Mishne (PhD) and Many Many Others.
#   Contact: dan_c_marinescu@yahoo.com (Subject: STL)
#
#   Modified to work with g++ 4.3 by Anders Elton
#   Also added _member functions, that instead of printing the entire class in map, prints a member.



#
# std::vector<>
#

define pvector
    if $argc == 0
        help pvector
    else
        set $size = $arg0._M_impl._M_finish - $arg0._M_impl._M_start
        set $capacity = $arg0._M_impl._M_end_of_storage - $arg0._M_impl._M_start
        set $size_max = $size - 1
    end
    if $argc == 1
        set $i = 0
        while $i < $size
            printf "elem[%u]: ", $i
            p *($arg0._M_impl._M_start + $i)
            set $i++
        end
    end
    if $argc == 2
        set $idx = $arg1
        if $idx < 0 || $idx > $size_max
            printf "idx1, idx2 are not in acceptable range: [0..%u].\n", $size_max
        else
            printf "elem[%u]: ", $idx
            p *($arg0._M_impl._M_start + $idx)
        end
    end
    if $argc == 3
      set $start_idx = $arg1
      set $stop_idx = $arg2
      if $start_idx > $stop_idx
        set $tmp_idx = $start_idx
        set $start_idx = $stop_idx
        set $stop_idx = $tmp_idx
      end
      if $start_idx < 0 || $stop_idx < 0 || $start_idx > $size_max || $stop_idx > $size_max
        printf "idx1, idx2 are not in acceptable range: [0..%u].\n", $size_max
      else
        set $i = $start_idx
        while $i <= $stop_idx
            printf "elem[%u]: ", $i
            p *($arg0._M_impl._M_start + $i)
            set $i++
        end
      end
    end
    if $argc > 0
        printf "Vector size = %u\n", $size
        printf "Vector capacity = %u\n", $capacity
        printf "Element "
        whatis $arg0._M_impl._M_start
    end
end

document pvector
    Prints std::vector<T> information.
    Syntax: pvector <vector> <idx1> <idx2>
    Note: idx, idx1 and idx2 must be in acceptable range [0..<vector>.size()-1].
    Examples:
    pvector v - Prints vector content, size, capacity and T typedef
    pvector v 0 - Prints element[idx] from vector
    pvector v 1 2 - Prints elements in range [idx1..idx2] from vector
end 

#
# std::list<>
#

define plist
    if $argc == 0
        help plist
    else
        set $head = &$arg0._M_impl._M_node
        set $current = $arg0._M_impl._M_node._M_next
        set $size = 0
        while $current != $head
            if $argc == 2
                printf "elem[%u]: ", $size
                p *($arg1*)($current + 1)
            end
            if $argc == 3
                if $size == $arg2
                    printf "elem[%u]: ", $size
                    p *($arg1*)($current + 1)
                end
            end
            set $current = $current._M_next
            set $size++
        end
        printf "List size = %u \n", $size
        if $argc == 1
            printf "List "
            whatis $arg0
            printf "Use plist <variable_name> <element_type> to see the elements in the list.\n"
        end
    end
end

document plist
    Prints std::list<T> information.
    Syntax: plist <list> <T> <idx>: Prints list size, if T defined all elements or just element at idx
    Examples:
    plist l - prints list size and definition
    plist l int - prints all elements and list size
    plist l int 2 - prints the third element in the list (if exists) and list size
end

define plist_member
    if $argc == 0
        help plist_member
    else
        set $head = &$arg0._M_impl._M_node
        set $current = $arg0._M_impl._M_node._M_next
        set $size = 0
        while $current != $head
            if $argc == 3
                printf "elem[%u]: ", $size
                p (*($arg1*)($current + 1)).$arg2
            end
            if $argc == 4
                if $size == $arg3
                    printf "elem[%u]: ", $size
                    p (*($arg1*)($current + 1)).$arg2
                end
            end
            set $current = $current._M_next
            set $size++
        end
        printf "List size = %u \n", $size
        if $argc == 1
            printf "List "
            whatis $arg0
            printf "Use plist_member <variable_name> <element_type> <member> to see the elements in the list.\n"
        end
    end
end

document plist_member
    Prints std::list<T> information.
    Syntax: plist <list> <T> <idx>: Prints list size, if T defined all elements or just element at idx
    Examples:
    plist_member l int member - prints all elements and list size
    plist_member l int member 2 - prints the third element in the list (if exists) and list size
end


#
# std::map and std::multimap
#

define pmap
    if $argc == 0
        help pmap
    else
        set $tree = $arg0
        set $i = 0
        set $node = $tree._M_t._M_impl._M_header._M_left
        set $end = $tree._M_t._M_impl._M_header
        set $tree_size = $tree._M_t._M_impl._M_node_count
        if $argc == 1
            printf "Map "
            whatis $tree
            printf "Use pmap <variable_name> <left_element_type> <right_element_type> to see the elements in the map.\n"
        end
        if $argc == 3
            while $i < $tree_size
                set $value = (void *)($node + 1)
                printf "elem[%u].left: ", $i
                p *($arg1*)$value
                set $value = $value + sizeof($arg1)
                printf "elem[%u].right: ", $i
                p *($arg2*)$value
                if $node._M_right != 0
                    set $node = $node._M_right
                    while $node._M_left != 0
                        set $node = $node._M_left
                    end
                else
                    set $tmp_node = $node._M_parent
                    while $node == $tmp_node._M_right
                        set $node = $tmp_node
                        set $tmp_node = $tmp_node._M_parent
                    end
                    if $node._M_right != $tmp_node
                        set $node = $tmp_node
                    end
                end
                set $i++
            end
        end
        if $argc == 4
            set $idx = $arg3
            set $ElementsFound = 0
            while $i < $tree_size
                set $value = (void *)($node + 1)
                if *($arg1*)$value == $idx
                    printf "elem[%u].left: ", $i
                    p *($arg1*)$value
                    set $value = $value + sizeof($arg1)
                    printf "elem[%u].right: ", $i
                    p *($arg2*)$value
                    set $ElementsFound++
                end
                if $node._M_right != 0
                    set $node = $node._M_right
                    while $node._M_left != 0
                        set $node = $node._M_left
                    end
                else
                    set $tmp_node = $node._M_parent
                    while $node == $tmp_node._M_right
                        set $node = $tmp_node
                        set $tmp_node = $tmp_node._M_parent
                    end
                    if $node._M_right != $tmp_node
                        set $node = $tmp_node
                    end
                end
                set $i++
            end
            printf "Number of elements found = %u\n", $ElementsFound
        end
        if $argc == 5
            set $idx1 = $arg3
            set $idx2 = $arg4
            set $ElementsFound = 0
            while $i < $tree_size
                set $value = (void *)($node + 1)
                set $valueLeft = *($arg1*)$value
                set $valueRight = *($arg2*)($value + sizeof($arg1))
                if $valueLeft == $idx1 && $valueRight == $idx2
                    printf "elem[%u].left: ", $i
                    p $valueLeft
                    printf "elem[%u].right: ", $i
                    p $valueRight
                    set $ElementsFound++
                end
                if $node._M_right != 0
                    set $node = $node._M_right
                    while $node._M_left != 0
                        set $node = $node._M_left
                    end
                else
                    set $tmp_node = $node._M_parent
                    while $node == $tmp_node._M_right
                        set $node = $tmp_node
                        set $tmp_node = $tmp_node._M_parent
                    end
                    if $node._M_right != $tmp_node
                        set $node = $tmp_node
                    end
                end
                set $i++
            end
            printf "Number of elements found = %u\n", $ElementsFound
        end
        printf "Map size = %u\n", $tree_size
    end
end

document pmap
    Prints std::map<TLeft and TRight> or std::multimap<TLeft and TRight> information. Works for std::multimap as well.
    Syntax: pmap <map> <TtypeLeft> <TypeRight> <valLeft> <valRight>: Prints map size, if T defined all elements or just element(s) with val(s)
    Examples:
    pmap m - prints map size and definition
    pmap m int int - prints all elements and map size
    pmap m int int 20 - prints the element(s) with left-value = 20 (if any) and map size
    pmap m int int 20 200 - prints the element(s) with left-value = 20 and right-value = 200 (if any) and map size
end


define pmap_member
    if $argc == 0
        help pmap_member
    else
        set $tree = $arg0
        set $i = 0
        set $node = $tree._M_t._M_impl._M_header._M_left
        set $end = $tree._M_t._M_impl._M_header
        set $tree_size = $tree._M_t._M_impl._M_node_count
        if $argc == 1
            printf "Map "
            whatis $tree
            printf "Use pmap <variable_name> <left_element_type> <right_element_type> to see the elements in the map.\n"
        end
        if $argc == 5
            while $i < $tree_size
                set $value = (void *)($node + 1)
                printf "elem[%u].left: ", $i
                p (*($arg1*)$value).$arg2
                set $value = $value + sizeof($arg1)
                printf "elem[%u].right: ", $i
                p (*($arg3*)$value).$arg4
                if $node._M_right != 0
                    set $node = $node._M_right
                    while $node._M_left != 0
                        set $node = $node._M_left
                    end
                else
                    set $tmp_node = $node._M_parent
                    while $node == $tmp_node._M_right
                        set $node = $tmp_node
                        set $tmp_node = $tmp_node._M_parent
                    end
                    if $node._M_right != $tmp_node
                        set $node = $tmp_node
                    end
                end
                set $i++
            end
        end
        if $argc == 6
            set $idx = $arg5
            set $ElementsFound = 0
            while $i < $tree_size
                set $value = (void *)($node + 1)
                if *($arg1*)$value == $idx
                    printf "elem[%u].left: ", $i
                    p (*($arg1*)$value).$arg2
                    set $value = $value + sizeof($arg1)
                    printf "elem[%u].right: ", $i
                    p (*($arg3*)$value).$arg4
                    set $ElementsFound++
                end
                if $node._M_right != 0
                    set $node = $node._M_right
                    while $node._M_left != 0
                        set $node = $node._M_left
                    end
                else
                    set $tmp_node = $node._M_parent
                    while $node == $tmp_node._M_right
                        set $node = $tmp_node
                        set $tmp_node = $tmp_node._M_parent
                    end
                    if $node._M_right != $tmp_node
                        set $node = $tmp_node
                    end
                end
                set $i++
            end
            printf "Number of elements found = %u\n", $ElementsFound
        end
        printf "Map size = %u\n", $tree_size
    end
end

document pmap_member
    Prints std::map<TLeft and TRight> or std::multimap<TLeft and TRight> information. Works for std::multimap as well.
    Syntax: pmap <map> <TtypeLeft> <TypeRight> <valLeft> <valRight>: Prints map size, if T defined all elements or just element(s) with val(s)
    Examples:
    pmap_member m class1 member1 class2 member2 - prints class1.member1 : class2.member2
    pmap_member m class1 member1 class2 member2 lvalue - prints class1.member1 : class2.member2 where class1 == lvalue
end


#
# std::set and std::multiset
#

define pset
    if $argc == 0
        help pset
    else
        set $tree = $arg0
        set $i = 0
        set $node = $tree._M_t._M_impl._M_header._M_left
        set $end = $tree._M_t._M_impl._M_header
        set $tree_size = $tree._M_t._M_impl._M_node_count
        if $argc == 1
            printf "Set "
            whatis $tree
            printf "Use pset <variable_name> <element_type> to see the elements in the set.\n"
        end
        if $argc == 2
            while $i < $tree_size
                set $value = (void *)($node + 1)
                printf "elem[%u]: ", $i
                p *($arg1*)$value
                if $node._M_right != 0
                    set $node = $node._M_right
                    while $node._M_left != 0
                        set $node = $node._M_left
                    end
                else
                    set $tmp_node = $node._M_parent
                    while $node == $tmp_node._M_right
                        set $node = $tmp_node
                        set $tmp_node = $tmp_node._M_parent
                    end
                    if $node._M_right != $tmp_node
                        set $node = $tmp_node
                    end
                end
                set $i++
            end
        end
        if $argc == 3
            set $idx = $arg2
            set $ElementsFound = 0
            while $i < $tree_size
                set $value = (void *)($node + 1)
                if *($arg1*)$value == $idx
                    printf "elem[%u]: ", $i
                    p *($arg1*)$value
                    set $ElementsFound++
                end
                if $node._M_right != 0
                    set $node = $node._M_right
                    while $node._M_left != 0
                        set $node = $node._M_left
                    end
                else
                    set $tmp_node = $node._M_parent
                    while $node == $tmp_node._M_right
                        set $node = $tmp_node
                        set $tmp_node = $tmp_node._M_parent
                    end
                    if $node._M_right != $tmp_node
                        set $node = $tmp_node
                    end
                end
                set $i++
            end
            printf "Number of elements found = %u\n", $ElementsFound
        end
        printf "Set size = %u\n", $tree_size
    end
end

document pset
    Prints std::set<T> or std::multiset<T> information. Works for std::multiset as well.
    Syntax: pset <set> <T> <val>: Prints set size, if T defined all elements or just element(s) having val
    Examples:
    pset s - prints set size and definition
    pset s int - prints all elements and the size of s
    pset s int 20 - prints the element(s) with value = 20 (if any) and the size of s
end



#
# std::dequeue
#

define pdequeue
    if $argc == 0
        help pdequeue
    else
        set $size = 0
        set $start_cur = $arg0._M_impl._M_start._M_cur
        set $start_last = $arg0._M_impl._M_start._M_last
        set $start_stop = $start_last
        while $start_cur != $start_stop
            p *$start_cur
            set $start_cur++
            set $size++
        end
        set $finish_first = $arg0._M_impl._M_finish._M_first
        set $finish_cur = $arg0._M_impl._M_finish._M_cur
        set $finish_last = $arg0._M_impl._M_finish._M_last
        if $finish_cur < $finish_last
            set $finish_stop = $finish_cur
        else
            set $finish_stop = $finish_last
        end
        while $finish_first != $finish_stop
            p *$finish_first
            set $finish_first++
            set $size++
        end
        printf "Dequeue size = %u\n", $size
    end
end

document pdequeue
    Prints std::dequeue<T> information.
    Syntax: pdequeue <dequeue>: Prints dequeue size, if T defined all elements
    Deque elements are listed "left to right" (left-most stands for front and right-most stands for back)
    Example:
    pdequeue d - prints all elements and size of d
end



#
# std::stack
#

define pstack
    if $argc == 0
        help pstack
    else
        set $start_cur = $arg0.c._M_impl._M_start._M_cur
        set $finish_cur = $arg0.c._M_impl._M_finish._M_cur
        set $size = $finish_cur - $start_cur
        set $i = $size - 1
        while $i >= 0
            p *($start_cur + $i)
            set $i--
        end
        printf "Stack size = %u\n", $size
    end
end

document pstack
    Prints std::stack<T> information.
    Syntax: pstack <stack>: Prints all elements and size of the stack
    Stack elements are listed "top to buttom" (top-most element is the first to come on pop)
    Example:
    pstack s - prints all elements and the size of s
end



#
# std::queue
#

define pqueue
    if $argc == 0
        help pqueue
    else
        set $start_cur = $arg0.c._M_impl._M_start._M_cur
        set $finish_cur = $arg0.c._M_impl._M_finish._M_cur
        set $size = $finish_cur - $start_cur
        set $i = 0
        while $i < $size
            p *($start_cur + $i)
            set $i++
        end
        printf "Queue size = %u\n", $size
    end
end

document pqueue
    Prints std::queue<T> information.
    Syntax: pqueue <queue>: Prints all elements and the size of the queue
    Queue elements are listed "top to bottom" (top-most element is the first to come on pop)
    Example:
    pqueue q - prints all elements and the size of q
end



#
# std::priority_queue
#

define ppqueue
    if $argc == 0
        help ppqueue
    else
        set $size = $arg0.c._M_impl._M_finish - $arg0.c._M_impl._M_start
        set $capacity = $arg0.c._M_impl._M_end_of_storage - $arg0.c._M_impl._M_start
        set $i = $size - 1
        while $i >= 0
            p *($arg0.c._M_impl._M_start + $i)
            set $i--
        end
        printf "Priority queue size = %u\n", $size
        printf "Priority queue capacity = %u\n", $capacity
    end
end

document ppqueue
    Prints std::priority_queue<T> information.
    Syntax: ppqueue <priority_queue>: Prints all elements, size and capacity of the priority_queue
    Priority_queue elements are listed "top to buttom" (top-most element is the first to come on pop)
    Example:
    ppqueue pq - prints all elements, size and capacity of pq
end



#
# std::bitset
#

define pbitset
    if $argc == 0
        help pbitset
    else
        p /t $arg0._M_w
    end
end

document pbitset
    Prints std::bitset<n> information.
    Syntax: pbitset <bitset>: Prints all bits in bitset
    Example:
    pbitset b - prints all bits in b
end



#
# std::string
#

define pstring
    if $argc == 0
        help pstring
    else
        printf "String \t\t\t= \"%s\"\n", $arg0._M_data()
        printf "String size/length \t= %u\n", $arg0._M_rep()._M_length
        printf "String capacity \t= %u\n", $arg0._M_rep()._M_capacity
        printf "String ref-count \t= %d\n", $arg0._M_rep()._M_refcount
    end
end

document pstring
    Prints std::string information.
    Syntax: pstring <string>
    Example:
    pstring s - Prints content, size/length, capacity and ref-count of string s
end 

#
# std::wstring
#

define pwstring
    if $argc == 0
        help pwstring
    else
        call printf("WString \t\t= \"%ls\"\n", $arg0._M_data())
        printf "WString size/length \t= %u\n", $arg0._M_rep()._M_length
        printf "WString capacity \t= %u\n", $arg0._M_rep()._M_capacity
        printf "WString ref-count \t= %d\n", $arg0._M_rep()._M_refcount
    end
end

document pwstring
    Prints std::wstring information.
    Syntax: pwstring <wstring>
    Example:
    pwstring s - Prints content, size/length, capacity and ref-count of wstring s
end 

#
# C++ related beautifiers (optional)
#

set height 0
set history size 10000
set history filename ~/.gdb_history
set history save on

#退出時不顯示提示信息
set confirm off

#按照派生類型打印對象
set print object on

#打印數組的索引下標
set print array-indexes on

#每行打印一個結構體成員
set print pretty on

set print union on

set print address on 

set print static-members on
set print vtbl on
set print demangle on
set demangle-style gnu-v3
set print sevenbit-strings off
set step-mode on

shell rm -f ./gdb.log
set logging off
set logging file ./gdb.log
set logging on

python
import sys
import os
p = os.path.expanduser('/root/.mybin/gdb_python3/python') 
print(p)
if os.path.exists(p):
    sys.path.insert(0, p)
    from libstdcxx.v6.printers import register_libstdcxx_printers
    register_libstdcxx_printers(None)
end
```