# Jdbshell

```
https://github.com/xianjimli/jdbshell
```

- jdbshell.c

```
/*
 * File:    jdbshell.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   a shell for gdb.
 *
 * Copyright (c) 2009  topwise, Inc.
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * History:
 * ================================================================
 * 2009-07-08 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <signal.h>

static const char* commands[] =
{
	"class",
	"classes",
	"classpath",
	"clear",
	"connectors",
	"cont",
	"disablegc",
	"down",
	"dump",
	"enablegc",
	"eval",
	"exit",
	"fields",
	"help",
	"interrupt",
	"kill",
	"list",
	"locals",
	"lock",
	"methods",
	"monitor",
	"next",
	"pop",
	"print",
	"read",
	"reenter",
	"resume",
	"run",
	"set",
	"step",
	"stepi",
	"stop",
	"suspend",
	"thread",
	"threadgroup",
	"threadgroups",
	"threadlocks",
	"threads",
	"trace",
	"unmonitor",
	"untrace",
	"up",
	"version",
	"where",
	"wherei",
	NULL
};

static struct Alias
{
	const char* alias;
	const char* command;
}g_alias[] =
{
	{"c",  "cont"},
	{"l",  "list"},
	{"n",  "next"},
	{"r",  "run"},
	{"s",  "step"},
	{"si", "stepi"},
	{"f",  "step up"},
	{"bt", "wherei"},
	{NULL, NULL}
};

static char* command_generator(const char *text, int state)
{
	const char *name;
	static int list_index, len;

	if (!state)
	{
		list_index = 0;
		len = strlen (text);
	}

	while (name = commands[list_index])
	{
		list_index++;

		if (strncmp (name, text, len) == 0)
			return strdup(name);
	}

	return ((char *)NULL);
}

char** command_completion (const char *text, int start, int end)
{
	char **matches = NULL;

	if (start == 0)
		matches = rl_completion_matches (text, command_generator);

	return (matches);
}

void initialize_readline ()
{
	rl_readline_name = "jdbshell";
	rl_attempted_completion_function = command_completion;

	return ;
}

char * stripwhite (char *string)
{
	char *s = NULL;
	char *t = NULL;

	for (s = string; whitespace (*s); s++);

	if (*s == 0) return (s);

	t = s + strlen (s) - 1;
	while (t > s && whitespace (*t))t--;
	*++t = '\0';

	return s;
}

void show_usage(void)
{
	int i = 0;
	fprintf(stderr, "usage: jdbshell [jdb args]\n");
	fprintf(stderr, "example: ./jdbshell -sourcepath $ANDROID_SRC_PATH -attach 6107\n");
	fprintf(stderr, "command alias:\n");

	for(i = 0; g_alias[i].alias != NULL; i++)
	{
		fprintf(stderr, "	%2s -- %s\n", g_alias[i].alias, g_alias[i].command);
	}

	return;
}

int main (int argc, char **argv)
{
	int  pid = 0;
	char *line = NULL;
	char *s = NULL;
	char last_cmd[1024] = {0};
	int  parent_to_child[2] = {0};

	if(argc == 1)
	{
		show_usage();
		return 0;
	}

	initialize_readline ();
	pipe(parent_to_child);

	pid = fork();
	if(pid == 0)
	{
		close(parent_to_child[1]);
		dup2(parent_to_child[0], STDIN_FILENO);
		strcpy(argv[0], "jdb");
		int ret = execvp("jdb", argv);
		fprintf(stderr, "jdb exit: ret=%d\n", ret);
	}
	else
	{
		close(parent_to_child[0]);
        write(parent_to_child[1], "threads\n",strlen("threads\n"));

		while(1)
		{
			int i = 0;
			line = readline ("");

			if (!line) break;

			s = stripwhite (line);

			if(*s)
			{
			  add_history (s);
			  strncpy(last_cmd, s, sizeof(last_cmd)-1);
			}
			else if(last_cmd[0])
			{
				s = last_cmd;
				printf(">%s\n", s);
			}
			else
			{
				free (line);
				printf(">");
				continue;
			}

			for(i = 0; g_alias[i].alias != NULL; i++)
			{
				int len = strlen(g_alias[i].alias);
				if(strncmp(g_alias[i].alias, s, len) == 0
					&& (s[len] == ' ' || s[len] == '\0' || s[len] == '\t'))
				{
					write(parent_to_child[1], g_alias[i].command,
						strlen(g_alias[i].command));
					break;
				}
			}

			if(g_alias[i].alias == NULL)
			{
				write(parent_to_child[1], s, strlen(s));
			}

			if(write(parent_to_child[1], "\n", 1) <= 0)
			{
				fprintf(stderr, "jdb exited\n");
				break;
			}
			free (line);
		}

		kill(pid, SIGTERM);
	}

	return 0;
}

```


- 因爲我基本上每次一開始都要執行threads變量，所以我在開始wile循環前加了一句話

```
else
{
    close(parent_to_child[0]);
    write(parent_to_child[1], "threads\n",strlen("threads\n"));

```



- Makefile

```
CFLAGS=-DRL_LIBRARY_VERSION='"5.1"' -I. -I.. -g -O
all:
	gcc $(CFLAGS) jdbshell.c -lreadline -ltermcap -o jdbshell
clean:
	rm -f jdbshell *.o
```
- build/envsetup.sh 仿照gdbclient，在build/envsetup.sh中加入jdbclient函數

```
function jdbclient()
{
    local MY_SRC_PATH="$1"
    if [ "$MY_SRC_PATH" ] ; then
        MY_SRC_PATH=$1
        echo $MY_SRC_PATH
    else
        echo "ROOT" $OUR_ROOT
        MY_SRC_PATH="app_process"
    fi


    local PORT="$2"
    if [ "$PORT" ] ; then
        PORT=$2
    else
        PORT=":9000"
    fi


    local PID
    local PROG="$3"
    if [ "$PROG" ] ; then
        PID=`pid $3`
        echo tcp$PORT jdwp$PID
        adb forward "tcp$PORT" "jdwp:$PID"
        echo jdbshell -sourcepath $MY_SRC_PATH -attach localhost$PORT
        jdbshell -sourcepath $MY_SRC_PATH -attach localhost$PORT
        sleep 2
    else
        echo ""
        echo "please specify app you want to debug:"
        echo ""
    fi
}
```

- env.sh

```
export ANDROID_SRC=/opt/work/android/grepcode.com
export ANDROID_SRC_PATH=$ANDROID_SRC/frameworks/base/opengl/java:$ANDROID_SRC/frameworks/base/awt/java:$ANDROID_SRC/frameworks/base/core/java:$ANDROID_SRC/frameworks/base/location/java:$ANDROID_SRC/frameworks/base/sax/java:$ANDROID_SRC/frameworks/base/graphics/java:$ANDROID_SRC/frameworks/base/telephony/java:$ANDROID_SRC/frameworks/base/services/java:$ANDROID_SRC/frameworks/base/media/java:$ANDROID_SRC/frameworks/base/wifi/java:$ANDROID_SRC/frameworks/base/im/java:$ANDROID_SRC/dalvik/libcore/suncompat/src/main/java:$ANDROID_SRC/dalvik/libcore/nio_char/src/main/java:$ANDROID_SRC/dalvik/libcore/nio_char/src/main/java/java:$ANDROID_SRC/dalvik/libcore/security-kernel/src/main/java:$ANDROID_SRC/dalvik/libcore/security-kernel/src/main/java/java:$ANDROID_SRC/dalvik/libcore/security/src/main/java:$ANDROID_SRC/dalvik/libcore/security/src/main/java/java:$ANDROID_SRC/dalvik/libcore/archive/src/main/java:$ANDROID_SRC/dalvik/libcore/archive/src/main/java/java:$ANDROID_SRC/dalvik/libcore/awt-kernel/src/main/java:$ANDROID_SRC/dalvik/libcore/awt-kernel/src/main/java/java:$ANDROID_SRC/dalvik/libcore/luni/src/main/java:$ANDROID_SRC/dalvik/libcore/luni/src/main/java/java:$ANDROID_SRC/dalvik/libcore/math/src/main/java:$ANDROID_SRC/dalvik/libcore/math/src/main/java/java:$ANDROID_SRC/dalvik/libcore/x-net/src/main/java:$ANDROID_SRC/dalvik/libcore/openssl/src/main/java:$ANDROID_SRC/dalvik/libcore/dalvik/src/main/java:$ANDROID_SRC/dalvik/libcore/auth/src/main/java:$ANDROID_SRC/dalvik/libcore/concurrent/src/main/java:$ANDROID_SRC/dalvik/libcore/concurrent/src/main/java/java:$ANDROID_SRC/dalvik/libcore/sql/src/main/java:$ANDROID_SRC/dalvik/libcore/sql/src/main/java/java:$ANDROID_SRC/dalvik/libcore/prefs/src/main/java:$ANDROID_SRC/dalvik/libcore/prefs/src/main/java/java:$ANDROID_SRC/dalvik/libcore/xml/src/main/java:$ANDROID_SRC/dalvik/libcore/text/src/main/java:$ANDROID_SRC/dalvik/libcore/text/src/main/java/java:$ANDROID_SRC/dalvik/libcore/luni-kernel/src/main/java:$ANDROID_SRC/dalvik/libcore/luni-kernel/src/main/java/java:$ANDROID_SRC/dalvik/libcore/regex/src/main/java:$ANDROID_SRC/dalvik/libcore/regex/src/main/java/java:$ANDROID_SRC/dalvik/libcore/nio/src/main/java:$ANDROID_SRC/dalvik/libcore/nio/src/main/java/java:$ANDROID_SRC/dalvik/libcore/json/src/main/java:$ANDROID_SRC/dalvik/libcore/crypto/src/main/java:$ANDROID_SRC/dalvik/libcore/icu/src/main/java:$ANDROID_SRC/dalvik/libcore/annotation/src/main/java:$ANDROID_SRC/dalvik/libcore/annotation/src/main/java/java:$ANDROID_SRC/dalvik/libcore/junit/src/main/java:$ANDROID_SRC/dalvik/libcore/logging/src/main/java:$ANDROID_SRC/dalvik/libcore/logging/src/main/java/java:$ANDROID_SRC/dalvik/libcore-disabled/instrument/src/main/java:$ANDROID_SRC/dalvik/libcore-disabled/instrument/src/main/java/java:$ANDROID_SRC/dalvik/libcore-disabled/sound/src/main/java
```

- adb jdwp列出支持JDPW的進程ID

```
adb jdwp
```

- 啓動android模擬器，運行瀏覽器

```
jdbclient packets/app/Browser/src/ :9000 browser 第一個參數是你要load進來的代碼的地址，如果你要load多個代碼（調試的時候可以看)，可以 export DEBUG_SRC_PATH= 然後jdbclient $DEBUG_SRC_PATH :9000 browser

第二個參數是attach端口，如果9000有在用，可以用其他端口
第三個參數是要調試的應用，腳本裏面根據
pid browser來取得進程id，所以你要保證
adb shell ps |grep browser
能夠看到對應的進程
```

- 用threads查詢所有線程：

```
Set uncaught java.lang.Throwable
Set deferred uncaught java.lang.Throwable
Initializing jdb ...
Group system:
  (java.lang.Thread)0xc14050d0c0                  <6> Compiler             cond. waiting
  (java.lang.Thread)0xc14050cf50                  <4> Signal Catcher       cond. waiting
  (java.lang.Thread)0xc14050cea8                  <3> GC                   cond. waiting
  (java.lang.Thread)0xc14050cdf0                  <2> HeapWorker           cond. waiting
Group main:
  (java.lang.Thread)0xc14001f1a8                  <1> main                 running
  (java.lang.Thread)0xc140562ad8                  <21> AsyncTask #5        cond. waiting
  (android.os.HandlerThread)0xc1405528f8          <20> WebViewWorkerThread runnin
```


- 選擇感興趣的線程大部分應用跑在main線程裏面，browser也不例外命令 thread 線程ID

```
thread 0xc14001f1a8
<1> main[1]
```

- 掛起線程命令 suspend 線程ID

```
<1> main[1] suspend 0xc14001f1a8
```


- 8)設置斷點
命令：

```
stop at <類>:<行號> 或
stop in <類>.<方法名>[(參數類型,...)]


<1> main[1] stop at com.android.browser.BrowserActivity:2689
Set breakpoint com.android.browser.BrowserActivity:2689

這個地方要寫類名稍微麻煩一點，哪位大蝦可以轉化成像gdb那樣，直接文件名？
```

- 9)繼續執行

```
命令 c(ont)

<1> main[1] c
>
```


- 10)點擊網頁中的某一個鏈接
就會看到程序在BrowserActivty:2689(shouldOverideUrlLoading)處斷住了
此時可以看堆棧
命令:bt/wherei

```
<1> main[1]bt
  [1] com.android.browser.BrowserActivity.shouldOverrideUrlLoading (BrowserActivity.java:2,689), pc = 8
  [2] com.android.browser.Tab$2.shouldOverrideUrlLoading (Tab.java:552), pc = 44
  [3] android.webkit.CallbackProxy.uiOverrideUrlLoading (CallbackProxy.java:216), pc = 19
  [4] android.webkit.CallbackProxy.handleMessage (CallbackProxy.java:323), pc = 347
  [5] android.os.Handler.dispatchMessage (Handler.java:99), pc = 20
  [6] android.os.Looper.loop (Looper.java:123), pc = 75
  [7] android.app.ActivityThread.main (ActivityThread.java:3,683), pc = 31
  [8] java.lang.reflect.Method.invokeNative (native method)
  [9] java.lang.reflect.Method.invoke (Method.java:507), pc = 18
  [10] com.android.internal.os.ZygoteInit$MethodAndArgsCaller.run (ZygoteInit.java:839), pc = 11
  [11] com.android.internal.os.ZygoteInit.main (ZygoteInit.java:597), pc = 84
  [12] dalvik.system.NativeStart.main (native method)
```


- 看代碼命令:l(ist)

```
<1> main[1] l
2,685            }
2,686        }
2,687
2,688        boolean shouldOverrideUrlLoading(WebView view, String url) {
2,689 =>         if (url.startsWith(SCHEME_WTAI)) {
2,690                // wtai://wp/mc;number
2,691                // number=string(phone-number)
2,692                if (url.startsWith(SCHEME_WTAI_MC)) {
2,693                    Intent intent = new Intent(Intent.ACTION_VIEW,
2,694                            Uri.parse(WebView.SCHEME_TEL +
```


- 看變量命令:print

```
<1> main[1] print url
 url = "http://m.baidu.com/img?tn=bdidxiphone&ssid=0&from=844b&bd_page_type=1&uid=wiaui_1320452293_7438&pu=sz%401320_480&itj=41"


繼續執行
s(tep) – 執行當前行
step up – 執行到當前方法返回到其調用程序
s(tep)i – 執行當前指令
n(ext) – 跳過一行（跨過調用）
c(ont) – 從斷點處繼續執行
```

- 不說了，help是一種美德

```
<1> main[1] help
** command list **
connectors                -- list available connectors and transports in this VM


run [class [args]]        -- start execution of application's main class


threads [threadgroup]     -- list threads
thread <thread id>        -- set default thread
suspend [thread id(s)]    -- suspend threads (default: all)
resume [thread id(s)]     -- resume threads (default: all)
where [<thread id> | all] -- dump a thread's stack
wherei [<thread id> | all]-- dump a thread's stack, with pc info
up [n frames]             -- move up a thread's stack
down [n frames]           -- move down a thread's stack
kill <thread id> <expr>   -- kill a thread with the given exception object
interrupt <thread id>     -- interrupt a thread


print <expr>              -- print value of expression
dump <expr>               -- print all object information
eval <expr>               -- evaluate expression (same as print)
set <lvalue> = <expr>     -- assign new value to field/variable/array element
locals                    -- print all local variables in current stack frame


classes                   -- list currently known classes
class <class id>          -- show details of named class
methods <class id>        -- list a class's methods
fields <class id>         -- list a class's fields


threadgroups              -- list threadgroups
threadgroup <name>        -- set current threadgroup


stop in <class id>.<method>[(argument_type,...)]
                          -- set a breakpoint in a method
stop at <class id>:<line> -- set a breakpoint at a line
clear <class id>.<method>[(argument_type,...)]
                          -- clear a breakpoint in a method
clear <class id>:<line>   -- clear a breakpoint at a line
clear                     -- list breakpoints
catch [uncaught|caught|all] <class id>|<class pattern>
                          -- break when specified exception occurs
ignore [uncaught|caught|all] <class id>|<class pattern>
                          -- cancel 'catch' for the specified exception
watch [access|all] <class id>.<field name>
                          -- watch access/modifications to a field
unwatch [access|all] <class id>.<field name>
                          -- discontinue watching access/modifications to a field
trace [go] methods [thread]
                          -- trace method entries and exits.
                          -- All threads are suspended unless 'go' is specified
trace [go] method exit | exits [thread]
                          -- trace the current method's exit, or all methods' exits
                          -- All threads are suspended unless 'go' is specified
untrace [methods]         -- stop tracing method entrys and/or exits
step                      -- execute current line
step up                   -- execute until the current method returns to its caller
stepi                     -- execute current instruction
next                      -- step one line (step OVER calls)
cont                      -- continue execution from breakpoint


list [line number|method] -- print source code
use (or sourcepath) [source file path]
                          -- display or change the source path
exclude [<class pattern>, ... | "none"]
                          -- do not report step or method events for specified classes
classpath                 -- print classpath info from target VM


monitor <command>         -- execute command each time the program stops
monitor                   -- list monitors
unmonitor <monitor#>      -- delete a monitor
read <filename>           -- read and execute a command file


lock <expr>               -- print lock info for an object
threadlocks [thread id]   -- print lock info for a thread


pop                       -- pop the stack through and including the current frame
reenter                   -- same as pop, but current frame is reentered
redefine <class id> <class file name>
                          -- redefine the code for a class


disablegc <expr>          -- prevent garbage collection of an object
enablegc <expr>           -- permit garbage collection of an object


!!                        -- repeat last command
<n> <command>             -- repeat command n times
# <command>               -- discard (no-op)
help (or ?)               -- list commands
version                   -- print version information
exit (or quit)            -- exit debugger


<class id>: a full class name with package qualifiers
<class pattern>: a class name with a leading or trailing wildcard ('*')
<thread id>: thread number as reported in the 'threads' command
<expr>: a Java(tm) Programming Language expression.
Most common syntax is supported.


Startup commands can be placed in either "jdb.ini" or ".jdbrc"
in user.home or user.dir
```

- 如果要看別名映射，在終端執行

```
tom@tom-laptop:~/work/gingerbread$ jdbshell
usage: jdbshell [jdb args]
example: ./jdbshell -sourcepath $ANDROID_SRC_PATH -attach 6107
command alias:
c -- cont
l -- list
n -- next
r -- run
s -- step
si -- stepi
f -- step up
bt -- wherei
```
