
![alt tag](https://raw.githubusercontent.com/JaredCJR/JCC/master/demo/pictures/logo.jpg)

# JCC
Jared's C Compiler : Build a mini C compiler(x86-64) from scratch.

**This is a term-project for [NCKU compiler-system course](http://class-qry.acad.ncku.edu.tw/syllabus/online_display.php?syear=0104&sem=2&co_no=Q355300&class_code=).**


Quick Start:
====================================
All the toolchain you need is GCC on Linux based system.
Tested Platform: Ubuntu 16.04(x86-64)(gcc version 5.3.1 20160413 (Ubuntu 5.3.1-14ubuntu2) )

```
git clone https://github.com/JaredCJR/JCC
cd JCC
```
Then,jump to the specific `Phase` to see the guide.


Phase 1:Lexical Analysis
---------------------------
- This is a Token Colorer,it will color the tokens in the target source file,and print in the terminal colorfully.
- Using the lexical analysis to identify tokens for coloring.
- How to demo:
```
git checkout DEMO_LEX
make clean
make demo
```
![alt tag](https://raw.githubusercontent.com/JaredCJR/JCC/master/demo/pictures/phase_lexical.jpg)


Phase 2:Syntax Analysis
---------------------------
- This is a Syntax Indenter,it will indent the tokens with basic functionalities and Token Colorer.
- Using syntax analysis to check basic grammars , based on `Token Colorer(Phase 1)`.
  - Indent the code after the end of grammar for demonstrating the `Syntax Analysis`
- How to demo:
```
git checkout DEMO_SYNTAX
make clean
make demo
```
![alt tag](https://raw.githubusercontent.com/JaredCJR/JCC/master/demo/pictures/phase_syntax.jpg)



Phase 3:Semantic Analysis
---------------------------
- This will scan the source files and record the corresponding information into the `symbol table`.
  - `Symbol table` contains two stack -> `global_sym_stack` and `local_sym_stack`
- The correspoind behavior of the semantics can see the comments inside the source files in the [syntax dir](https://github.com/JaredCJR/JCC/tree/master/syntax/src) and the [demo_result.txt](https://github.com/JaredCJR/JCC/tree/master/demo/demo_result.txt),which explains elaborately!
  - You can see the explaination in [demo_result.txt](https://github.com/JaredCJR/JCC/tree/master/demo/demo_result.txt) after `<=================`
  - JCC's `semantic analysis` is much powerful than the JCC's `code generation`.This is because the time limitted during the semester.
    - If you would like to know more details,you should trace the stack with `gdb`,the pre-defined gdb tracing srcipt is ready.
- How to demo:
```
git checkout DEMO_SEMANTIC
make clean
make demo
```
![alt tag](https://raw.githubusercontent.com/JaredCJR/JCC/master/demo/pictures/phase_semantic.jpg)

- This picture shows part of the [demo_result.txt](https://github.com/JaredCJR/JCC/tree/master/demo/demo_result.txt).
- JCC use [GDB](https://www.gnu.org/software/gdb/) as the tool for this auto-demo.
  - GDB help us to check the `symbol table` without messing up the code!
  - GDB do the automatic test based on the [GDB script](https://github.com/JaredCJR/JCC/tree/master/demo/demo_autoGDB)

**IMPORTANT**
- If you would like to see the stack change on your own source code,the local stack will be destroied after the `index` leave `local scope`.
  - We only have one `local stack` for `symbol table` ,so the `local stack` will no longer exist after the `index` move outside the corresponding scope.


Phase 4:Code Generation
---------------------------
JCC does not support the fully function as the `lexical and semantic phases` in code generation due to time limitted.

What JCC is able to do,please refer to the [next chapter](https://github.com/JaredCJR/JCC#supported-functionality).

- How to demo:
```
git checkout DEMO_CodeGen
make clean
make demo
./demo/demo_code
```
![alt tag](https://raw.githubusercontent.com/JaredCJR/JCC/master/demo/pictures/phase_codeGen.jpg)
- JCC uses a fibonacci sequence with iterative method as the demo code to show what JCC is able to do!
  - This picture only shows part of the results.


Supported Functionality
==============================

Phase 1:Lexical Analysis
---------------------------
- **Tokens:**
  - Arithmetic Operators
    - `+`
    - `-`
    - `*`
    - `/`
    - `%`
  - Relation Oerators
    - `==`
    - `!=`
    - `<`
    - `<=`
    - `>`
    - `>=`
  - Assignment Operators
    - `=`
  - Pointer Operators
    - `->`
    - `.`
    - `&`
  - Others
    - `(` and `)`
    - `[` and `]`
    - `{` and `}`
    - `;`
    - `,`
    - `...`
- **Keywords:**
  - Constants
    - `tk_cINT` (number,  ex:999)
    - `tk_cCHAR`(single character,  ex:'X')
    - `tk_cSTR` (characters,  ex:`This is JCC\n`)
  - keywords
    - `int`
    - `char`
    - `void`
    - `struct`
    - `if`
    - `else`
    - `for`
    - `continue`
    - `break`
    - `sizeof`
  - user defined
    - Ex:`int user_ident;` (`user_ident` will be added.)

**IMPORTANT:Not all the tokens will be dealed in the `Phase 2 to Phase 4`**


Phase 2:Syntax Analysis
---------------------------

- JCC divides `Syntax Analysis into 3 sub-phase`
  - `Declaration`
  - `Expression`
  - `Statement`
- All the implementing details can be saw in the [source code](https://github.com/JaredCJR/JCC/tree/master/syntax/src).

**Declaration**

- In JCC,
  - All of the program of JCC must start with declaration.
    - Ex:`int x = 10;`
    - Ex:`int add(int x,int y);`
    - Ex:`add(1,2);`
    - Ex:`void main(){ }`
      - The `{ }` in above example,meaning `compound statement`,and you could use `statement` inside.

```
 * <translation_unit>::={<external_declaration>}<tk_EOF>
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
 * <external_declaration>::=<function_definition> | <declarator>
 * <function_definition>::=<type_specifier><declarator><funcbody>
 * <declarator>::=<type_specifier><tk_SEMICOLON> | <type_specifier><init_declarator_list><tk_SEMICOLON>
 * <init_declarator_list>::=<init_declarator>{<tk_COMMA><init_declarator>}
 * <init_declarator>::=<declarator>{<tk_ASSIGN><initializer>}
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * <type_specifier>::=<kw_INT>   |
 *                    <kw_CHAR>  |
 *                    <kw_VOID>  |
 *                    <struct_specifier>
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
 * <struct_specifier>::=<kw_STRUCT><IDENTIFIER><tk_BEGIN><struct_declaration_list><tk_END>
 *                    | <kw_STRUCT><IDENTIFIER>
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * <struct_declaration_list>::=<struct_declaration>{<struct_declaration>}
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
 * <struct_declaration>::=<type_specifier><struct_declaration_list><tk_SEMICOLON>
 * <struct_declaration_list>::=<declarator>{<tk_COMMA><declarator>}
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * <declarator>::={<pointer>}<direct_declarator>
 * <pointer>::=<tk_STAR>
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * <direct_declarator>::=<IDENTIFIER><direct_declarator_postfix>
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * <direct_declarator_postfix>::={<tk_openBR><tk_cINT><tk_closeBR>
 *                                | <tk_openBR><tk_closeBR>
 *                                | <tk_openPA><parameter_type_list><tk_closePA>
 *                                | <tk_openPA><tk_closePA>
 * } 
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * <parameter_type_list>::=<parameter_list> | <parameter_list><tk_COMMA><tk_ELLIPSIS>
 * <parameter_list>::<parameter_declaration>{<tk_COMMA><parameter_declaration>}
 * <parameter_declaration>::=<type_specifier>{<declarator>} 
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
 * <funcbody>::=<compound_statement>
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
 * <initializer>::=<assignment_expression>
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
```

**Expression**

- In JCC,
  - `Expression` help us estabilish the relationship between `tokens`.
    - Ex: `a >= b`
    - Ex: `X.member = 2;`
  - Help us support the arthimetic rules:`multiplication and division first,then addition and substration`
    - Ex: `int x = 0-6*6;`
      - `x is equal to -36`
  - Pass arguments
    - Ex: `add(x,y,z);`
      - `(x,y,z)` is dealed here.
  - We do not support bitwise operation.

```
 * <expression>::=<assignment_expression>{<tk_COMMA><assignment_expression>}
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
 * <assignment_expression>::=<equality_expression>
 *                         | <unary_expression><tk_ASSIGN><assignment_expression>
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * <equality_expression>::=<relational_expression>
 *                       { <tk_EQ><relational_expression>
 *                       | <tk_NEQ><relational_expression>
 *                       } 
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * <relational_expression>::=<additive_expression>
 *                        {
 *                           <tk_LT><additive_expression>
 *                         | <tk_GT><additive_expression>
 *                         | <tk_LEQ><additive_expression>
 *                         | <tk_GEQ><additive_expression>
 *                        } 
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
 * <additive_expression>::=<multiplicative_expression>
 *                      {
 *                        <tk_PLUS><multiplicative_expression>
 *                      | <tk_MINUS><multiplicative_expression>
 *                      }
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
 * <multiplicative_expression>::=<unary_expression>
 *                             {
 *                               <tk_STAR><unary_expression>
 *                             | <tk_DIVIDE><unary_expression>
 *                             | <tk_MOD><unary_expression>
 *                             }
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * <unary_expression>::=<postfix_expression>
 *                    | <tk_AND><unary_expression>
 *                    | <tk_STAR><unary_expression>
 *                    | <tk_PLUS><unary_expression>
 *                    | <tk_MINUS><unary_expression>
 *                    | <sizeof_expression>
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
 * <sizeof_expression>::=<kw_SIZEOF><tk_openPA><type_specifier><tk_closePA>
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
 * <postfix_expression>::=<primary_expression>
 *                     {
 *                        <tk_openBR><expression><tk_closeBR>
 *                      | <tk_openPA><tk_closePA>
 *                      | <tk_openPA><argument_expression_list><tk_closePA>
 *                      | <tk_DOT><IDENTIFIER>
 *                      | <tk_POINTTO><IDENTIFIER>
 *                     }
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
 * <primary_expression>::=<IDENTIFIER>
 *                      | <tk_cINT>
 *                      | <tk_cSTR>
 *                      | <tk_cCHAR>
 *                      | <tk_openPA><expression><tk_closePA>
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * <argument_expression_list>::=<assignment_expression>{<tk_COMMA><assignment_expression>}
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
```


**Statement**
- In JCC,
  - `statement` help the `keywords` to be known.
    - Ex: `if(a == c){ }`
      - `if` is identified here.
    - Ex: `return add(a,b);`
      - `return` is identified here.

``` 
 * <statement>::=<compound_statement>
 *             | <if_statement>
 *             | <return_statement>
 *             | <break_statement>
 *             | <continue_statement>
 *             | <for_statement>
 *             | <expression_statement>
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
 * <compound_statement>::=<tk_BEGIN>{<declaration>}{<statement>}<tk_END> 
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
 * <expression_statement>::=<tk_SEMICOLON>
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
 * <if_statement>::=<kw_IF><tk_openPA><expression><tk_closePA><statement>[<kw_ELSE><statement>]
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
 * <for_statement>::=
 *           <kw_FOR><tk_openPA><expression_statement><expression_statement><expression><tk_closePA><statement>
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
 * <continue_statement>::=<kw_CONTINUE><tk_SEMICOLON>
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
 * <break_statement>::=<kw_BREAK><tk_SEMICOLON>
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
 * <return_statement>::=<kw_RETURN><tk_SEMICOLON>
 *                    | <kw_RETURN><expression><tk_SEMICOLON>
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
```

Phase 3:Semantic Analysis
---------------------------
- The correspoind behavior of the semantics can see the comments inside the source files in the [syntax dir](https://github.com/JaredCJR/JCC/tree/master/syntax/src) and the [demo_result.txt](https://github.com/JaredCJR/JCC/tree/master/demo/demo_result.txt),which explains elaborately!
  - You can see the explaination in [demo_result.txt](https://github.com/JaredCJR/JCC/tree/master/demo/demo_result.txt) after `<=================`

Phase 4:Code Generation
---------------------------
- As the above says,JCC's `code generation` does not support the full functionalities as the previous phases.
**Supported semantic code generation:**
- `Variable` supporting:
  - `global variable`
  - `local variable`
    - including
      - `int`
      - `char`
      - special `char *`
        - Ex:  `char* string = "TEST string"`
        - YOU SHOULD NOT MODIFY THE `char *` VARIABLE!
          - Ex: The behavior of `string = "abc"` is unpredictable.(`string` declaration is as the previous example)


- `if(expression)`
- `if(expression){ statement }else{ statement }`
  - Supporting recursive `if` or `if else`.


- `for(expression;expression;expression)`
  - Supporting recursive `for`


- `declaration` always before `statement`
  - declaration
    - JCC does not check type for variable assignment,if and only if there is LValue at the left side.


- I/O:`scanf` and `printf`
  - use `printf` as the C89 <stdio.h> usage.
  - use `scanf` as the C99 <stdio.h> usage.
    - Only support at most `4 arguments`.
    - Need to do:
      - passing `return value` from either I/O function.


- Arthimatic
  - `+` and `-` can be use together
    - `Ex: int a = 5+100-10000;`
    - `Ex: a = b+c-100;`
      - if `b` and `c` are already declared.
  - `*` , `/` and `%` can be use together
    - `Ex: int a = 1000*5%79`
    - `Ex: a = b*c%100;`
      - if `b` and `c` are already declared.


- Function Call
  - Only support at most `4 arguments`.
  - if function return type is `int` or `char`
    - return value is accepted in limitted sutuation.
      - `Ex:a = test();`
        - However,JCC does not support mutiple function call arthimatic.
          - `Ex:a = test() + test();  will cause error!`
      - `return` in `if` or `for` will not work.
        - Therefore,`recursive function call` is imposible in JCC.
  - JCC does not support `expression` as parameter.
    - `Ex:add(1+2,3); will cause error!`


- See [demo_code.c](https://github.com/JaredCJR/JCC/blob/master/demo/demo_code.c) to know the real situation!
  - JCC currently does not support mixing `+` or `-` with `*` or `/`or `%`.
    - If you would like to support this functionality,all you need is to provide temporary registers or creat a space in local stack to store the temporary value.


How to debug the JCC
===========================
- If you would like to print somee information in you terminal
  - JCC provide an API `DBG_printf`, you could use it as [printf](http://linux.die.net/man/3/printf) in Linux.
    - REMEMBER:You need to uncomment `CFLAGS += -D$(JCC_DEBUG)` in the [Makefile](https://github.com/JaredCJR/JCC/blob/master/Makefile)
    - `DBG_printf` is define [here](https://github.com/JaredCJR/JCC/blob/master/core/include/debug.h)
- If you would like to use GNU debugger with JCC
  - `make demo_gdb`
    - It will call `gdb` or `gdbtui` with JCC to load the default demo source file.
    - You can write a `gdb script` for `gdb` family,such as the branch `DEMO_SEMANTIC` does.

AUTHOR:
====================================
- Jia-Rung Chang(張家榮/Jared)
    - email: jaredcjr.tw@gmail.com
	- Education
		- 2016/09 ~ future
			- Master in Department of Computer Science,National Chiao Tung University,Taiwan
		- 2012/09 ~ 2016/06
            - Bachelor in Department of Engineering Science,National Cheng Kung University,Taiwan

Licensing
========================
[GPLv3](http://www.gnu.org/licenses/)


