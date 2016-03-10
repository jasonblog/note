# GDB dashboard: debug info at a glance

https://github.com/cyrus-and/gdb-dashboard



GDB dashboard is a useful python utility for programmers to show valuable information while debugging with gdb. It uses the gdb python interface of gdb and implements several modules in different categories to show debug information:

- assembly: disassembled code
- expressions: watches user expressions
- history: last entries in gdb value history
- memory: inspect contiguous memory locations
- registers
- source (if available)
- stack: current stack trace
- threads

All the modules are enabled by default.

##Features

- Single GDB init file
- Interaction with GDB using the native Python API
- Several default modules are included to address the most basic needs: source code, assembly, registers, etc.)
- User-defined modules can be easily developed by extending a Python class
- Additional configuration files (both GDB and Python) are read from ~/.gdbinit.d/
F- ully stylable user interface and dynamic command prompt
- No GDB command has been redefined, instead all the features are available as subcommands of the main dashboard command
##Installation

Clone the GutHub repo and copy the file in your home directory:

```sh
$ git clone https://github.com/cyrus-and/gdb-dashboard
$ cp gdb-dashboard/.gdbinit ~/
```

##Usage

GDB dashboard is loaded when gdb is invoked, thanks to the ~/.gdbinit file. The dashboard can be shown whenever the program stops. By default it is turned off. Use the following command within gdb to change the behaviour:

```sh
>>> dashboard -enabled [on|off]
```

To show the dashboard:

```sh
>>> dashboard
```

To change the layout, e.g., show only assembly, expressions and registers and disable source, run:

```sh
>>> dashboard -layout assembly expressions registers !source
```



As you can understand, it’s very simple to extend this framework. For more information, hit the project page.


```c
#include <iostream>
#include <vector>
using namespace std;

typedef enum {Tree, Bug} Species;
typedef enum {Big_tree, Acorn, Seedling} Tree_forms;
typedef enum {Caterpillar, Cocoon, Butterfly}
Bug_forms;
struct thing {
    Species it;
    union {
        Tree_forms tree;
        Bug_forms bug;

    } form;

};

struct a {
    int aa;
    int bb;
};


void test (){

    std::cout << "test"  << std::endl;
}

int main()
{
    vector<int> ivector(10);
    int ar[2][2] = {0};
    struct thing foo = {Tree, {Acorn}};
    struct a aa = {100,22};

    test();
    

    for (int i = 0; i < ivector.size(); i++) {
        ivector[i] = i;
    }

    for (int i = 0; i < ivector.size(); i++) {
        cout << ivector[i] << " ";
    }

    cout << endl;

    return 0;
}
```


```
gdb ./test
source ~/.gdb-dashboard
b main
r
```