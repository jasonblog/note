# scons取代makefile編譯C/C++工程


##1. 基本使用
SConstruct文件就功能而言相當於Makefile文件，就內容而言則是Python腳本，scons讀入它時，會把腳本的命令都執行一遍，但具體編譯過程則有scons自己相機而定。

```sh
Program('hello.c')
Program('new_hello','hello_c')   前為目標，後為源文件
Program(['prog.c','file1.c','file2.c'])  多個源文件
Program(『program'，['prog.c','file1.c','file2.c'])   指定目標文件，多個源文件
Program('program『，Glob('*.c'))
Program('program',Split('main.c file.c file2.c'))  多個原文件更清晰的寫法
src_files=Split("""main.c
                           file1.c
                           file2.c""")
Program(target='program',source=src_files)
```
##2. 編譯多個目標文件就多次調用Program()

3.8  多個程序共享源文件

```sh
eg.1  
       Program(Split('foo.c common1.c common2.c'))
       Program('bar', Split('bar1.c bar2.c common1.c common2.c'))
eg.2    
       common = ['common1.c', 'common2.c']
       foo_files = ['foo.c'] + common
       bar_files = ['bar1.c', 'bar2.c'] + common
       Program('foo', foo_files)
       Program('bar', bar_files)
```

##4. 庫編譯和鏈接
###4.1 庫編譯
```sh
Library('foo', ['f1.c', 'f2.c', 'f3.c'])
```
###4.1.1  你甚至可以在文件List裡混用源文件和.o文件
```sh
Library('foo', ['f1.c', 'f2.o', 'f3.c', 'f4.o'])
```

###4.1.2  顯式編譯靜態庫
```sh
StaticLibrary('foo', ['f1.c', 'f2.c', 'f3.c'])
```

###4.1.3 編譯動態庫
```sh
SharedLibrary('foo', ['f1.c', 'f2.c', 'f3.c'])
```

##4.2 庫鏈接
```sh
Library('foo', ['f1.c', 'f2.c', 'f3.c'])
    Program('prog.c', LIBS=['foo', 'bar'], LIBPATH='.')
```

不用指明庫的前綴和後綴，如lib,.a等。
##4.3 找到庫：$LIBPATH變量
```sh
Program('prog.c', LIBS = 'm',
                LIBPATH = ['/usr/lib', '/usr/local/lib'])
Unix上     LIBPATH = '/usr/lib:/usr/local/lib'
Windows上  LIBPATH = 'C:\\lib;D:\\lib'
```

##5. 節點對象（nodes）
SCons把所有知道的文件和目錄表示為節點。
5.1 builder方法的返回值都是節點對象List,這一點很重要，在python中List+List還是List，下面可以看到應用。
```sh
hello_list = Object('hello.c', CCFLAGS='-DHELLO')
goodbye_list = Object('goodbye.c', CCFLAGS='-DGOODBYE')
Program(hello_list + goodbye_list)
```
這裡如果不使用變量的話，就不能跨平台使用了，因為不同系統object擴展名是不同的

###5.2 顯示創建文件和目錄節點
```sh
hello_c = File('hello.c')
Program(hello_c)
classes = Dir('classes')
Java(classes, 'src')
```

一般，你不用顯式調用他們，因為builder方法會自動處理文件名字符串並轉換為節點的。
當你不知道目標是文件還是目錄時：
```sh
xyzzy = Entry('xyzzy')
```
###5.3 打印節點文件名
```sh
hello_c = File('hello.c')
Program(hello_c)

classes = Dir('classes')
Java(classes, 'src')

object_list = Object('hello.c')
program_list = Program(object_list)
print "The object file is:", object_list[0]
print "The program file is:", program_list[0]
```

###5.4 把節點文件名當作字符串來使用
```py
import os.path
program_list = Program('hello.c')
program_name = str(program_list[0])
if not os.path.exists(program_name):
    print program_name, "does not exist!"
```
此例判斷了文件是否存在


##6. 依賴關係
###6.1 
Decider函數，確定文件是否被修改
####6.1.1 默認使用MD5檢查源文件內容而不是時間戳，用touch不算修改過
顯示調用
```sh
Program('hello.c')
Decider('MD5')
```

####6.1.2 使用時間戳
```sh
Program('hello.c')
Decider('timestamp-newer')
```

與下面語句等價
```sh
Program('hello.c')
Decider('make')
```
更詳細的檢查
```sh
Program('hello.c')
Decider('timestamp-match')
```
####6.1.3 兩者同時使用
```sh
Program('hello.c')
Decider('MD5-timestamp')
```
###6.3 $CPPPATH 隱含依賴關係
```sh
Program('hello.c', CPPPATH = ['include', '/home/project/inc'])
```

會在編譯時加 -I

###6.4 緩衝隱式依賴關係
```sh
% scons -Q --implicit-cache hello  運行選項
SetOption('implicit_cache', 1)      寫在腳本裡
```
這樣下次就不用從頭開始找依賴關係了
```sh
% scons -Q --implicit-deps-changed hello
```

再打開

###6.5 顯式依賴關係： Depends函數
當Scons沒有找到關係時
```sh
hello = Program('hello.c')
Depends(hello, 'other_file')
```

###6.6    總是編譯 
```sh
hello = Program('hello.c')
AlwaysBuild(hello)
```

##7. 環境
###7.2 創建環境
####7.2.1
```py
imp ort os
env = Environment(CC = 'gcc', CCFLAGS = '-O2')
env.Program('foo.c')
```

% scons -Q
####7.2.2 獲取環境變量
```sh
env = Environment()
print "CC is:", env['CC']
```

% scons -Q
CC is: cc
scons: `.' is up to date.
```sh
env = Environment(FOO = 'foo', BAR = 'bar')
dict = env.Dictionary()
for key in ['OBJSUFFIX', 'LIBSUFFIX', 'PROGSUFFIX']:
    print "key = %s, value = %s" % (key, dict[key])
``` 

This SConstruct file will print the specified dictionary items for us on POSIX systems as follows:

```sh
% scons -Q
key = OBJSUFFIX, value = .o
key = LIBSUFFIX, value = .a
key = PROGSUFFIX, value = 
scons: `.' is up to date.
```

##13. Install安裝文件至其他文件夾
```sh
env = Environment()
hello = env.Program('hello.c')
env.Install('/usr/bin', hello)
```

###13.1 在目錄裡安裝多個文件
```sh
env = Environment()
hello = env.Program('hello.c')
goodbye = env.Program('goodbye.c')
env.Install('/usr/bin', [hello, goodbye])
env.Alias('install', '/usr/bin')
```

###13.2 一個文件不同存放
```sh
env = Environment()
hello = env.Program('hello.c')
env.InstallAs('/usr/bin/hello-new', hello)
env.Alias('install', '/usr/bin')

% scons -Q install
cc -o hello.o -c hello.c
cc -o hello hello.o
Install file: "hello" as "/usr/bin/hello-new"
```
##14 平台獨立的文件系統操作
###14.1 複製
```sh
Command("file.out", "file.in", Copy("$TARGET", "$SOURCE"))
```

或者
```sh
Command("file.out", [], Copy("$TARGET", "file.in"))
```
##16. 多層目錄構建
`Sconconstruct在最頂層`
###16.1 SConscript文件可以層層包含
```sh
SConscript(['drivers/SConscript', 'parser/SConscript', 'utilities/SConscript'])
```

在drivers下：
```sh
SConscript(['display/SConscript', 'mouse/SConscript'])
```

###16.2 對文件SConscript的相對目錄
```sh
SConscript(['prog1/SConscript', 'prog2/SConscript'])
```