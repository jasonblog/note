---
title: MySQL 启动脚本
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,database,启动,脚本
description: mysqld_safe 是一个 shell 脚本，通常用来启动 MySQL 服务进程，在这篇文章中，我们看下该脚本具体做了什么。
---

mysqld_safe 是一个 shell 脚本，通常用来启动 MySQL 服务进程，在这篇文章中，我们看下该脚本具体做了什么。


<!-- more -->


## 简介

该 shell 脚本，详细交代了 MySQL 的启动流程：查找 MySQL 相关目录；解析配置文件；调用 mysqld 程序来启动实例。常见的配置项有：

{% highlight text %}
--help
    查看帮助。

--no-defaults
    不要读任何配置项文件。如果给出，必须首选该选项。
--defaults-extra-file=path
    除了通用选项文件所读取的选项文件名。如果给出，必须首选该选项。
--defaults-file=path
    读取的代替通用选项文件的选项文件名。如果给出，必须首选该选项。

--basedir=path
    指定 MySQL 安装目录的路径。
--datadir=path
    数据目录的路径。

--syslog, --skip-syslog
    是否使用syslog记录mysqld_safe的日志。如果是skip-syslog，会写到文件中，默认是hostname.err。
--log-error
    指定错误日志的文件路径，需要同时设置--skip-syslog 。
{% endhighlight %}

接下来看看 mysqld_safe 是如何执行的。

### 执行流程简介

如下是其简单的处理流程。

##### 1. 检查系统和选项

强制设置四个选项，并检查其有效性，包括了默认配置、basedir、datadir、plugin-dir 。

##### 2. 检查MyISAM表

默认是注释掉的，也可以在启动前检查 MyISAM 表。

##### 3. 监控服务

在 MySQL 服务器异常宕机时，mysqld_safe 默认会自动重启服务，而正常关闭则不会重启服务，那么它是如何判断 MySQL 是否是正常关闭的呢？能否控制不自动重启呢？

如果 MySQL 服务正常关闭，默认会删除 PID 文件，mysqld_safe 会通过判断是否存在该文件来决定是否要重新拉起 MySQL 服务；当然，如果想即使 MySQL 服务宕机也不启动文件，也可以新建一个名为 PID 文件名+ ```.shutdown``` 的文件即可，例如 /tmp/mysql.pid.shutdown 。

在此，还有些异常处理：

1. 如果在 MySQL 服务在一秒以内重启，则说明有异常，此时会通过 ```sleep 1``` 休眠一秒。
2. 在发现 MySQL 宕后，默认会检查是否有 mysqld 进程 hanging ，如果有，则直接通过 ```kill -9``` 强制关闭。

##### 4. 记录日志

将 mysqld 的错误消息发送到数据目录中的 host_name.err 文件，或者写入到 syslog 中。

<!--
6. 将mysqld_safe的屏幕输出发送到数据目录中的host_name.safe文件
-->

## 源码解析

该脚本实际上最终会通过 ```eval_log_error()``` 函数启动 MySQL 服务，并将输出日志记录到文件或者 syslog 中，这个函数应该是 mysqld_safe 脚本的核心处理函数。

{% highlight bash %}
#!/bin/sh

#----- 首先是一些变量参数的定义
KILL_MYSQLD=1;              # 试图kill多余的mysqld_safe程序，1表示需要kill
MYSQLD=                     # mysqld二进制可执行文件的目录名称
niceness=0                  # 进程的调度优先级
mysqld_ld_preload=          # 通常指定libjemalloc.so.1所在路径
mysqld_ld_library_path=     # 库目录

#----- 下面的变量用于设置日志，默认不打开错误日志，不使用syslog
logging=init                # 日志记录状态，init(输出到终端)，可以查看log_generic()
want_syslog=0               # 是否要使用syslog
syslog_tag=                 # 如果使用，则需要增加的标示
user='mysql'                # 启动时使用的用户，也就是指定--user的选项值
pid_file=                   # pid文件的路径
err_log=                    # 错误日志的路径
timestamp_format=UTC        # 使用UTC时间格式

#----- 定义的syslog中标志位，在后面需要写入日志到syslog中时使用
syslog_tag_mysqld=mysqld
syslog_tag_mysqld_safe=mysqld_safe
syslog_facility=daemon

trap '' 1 2 3 15            # 不允许程序在终端上被kill，包括挂起、中断、退出、系统终止
trap '' 13                  # 也包括SIGPIPE，可以通过kill -l查看数字对应的信号量

#----- 设置默认权限为770，可以查看umask(2)，为了兼容所以处理的时候比较复杂
umask 007                               # fallback
UMASK="${UMASK-0640}"
fmode=`echo "$UMASK" | sed -e 's/[^0246]//g'`
octalp=`echo "$fmode"|cut -c1`
fmlen=`echo "$fmode"|wc -c|sed -e 's/ //g'`
if [ "x$octalp" != "x0" -o "x$UMASK" != "x$fmode" -o "x$fmlen" != "x5" ]
then
  fmode=0640
  echo "UMASK must be a 3-digit mode with an additional leading 0 to indicate octal." >&2
  echo "The first digit will be corrected to 6, the others may be 0, 2, 4, or 6." >&2
fi
fmode=`echo "$fmode"|cut -c3-4`
fmode="6$fmode"
if [ "x$UMASK" != "x0$fmode" ]
then
  echo "UMASK corrected from $UMASK to 0$fmode ..."
fi

#----- 使用的默认配置文件，可以在命令行中指定
defaults=
case "$1" in
  --no-defaults|--defaults-file=*|--defaults-extra-file=*)
   defaults="$1"; shift
   ;;
esac

#----- 使用--help选项时输出的使用帮助信息
usage () {
        cat <<EOF
Usage: $0 [OPTIONS]
  --no-defaults              Don't read the system defaults file
  ... ...
EOF
        exit 1
}

#----- 相当于which，检索$PATH中的路径，打印出命令的全路径
my_which ()        # 该函数只在my_which logger用到，也就是转换为/usr/bin/logger
{
  save_ifs="${IFS-UNSET}"     # 保存当前的分隔符，用于后面恢复IFS
  IFS=:                       # 使用:来分割PATH中的路径
  ret=0
  for file                    # 等同于for file in $*
  do
    for dir in $PATH
    do
      if [ -f "$dir/$file" ]
      then
        echo "$dir/$file"
        continue 2            # 跳出外层循环
      fi
    done

	ret=1  #signal an error
	break
  done

  if [ "$save_ifs" = UNSET ]  # 重置IFS变量
  then
    unset IFS
  else
    IFS="$save_ifs"
  fi
  return $ret  # Success
}

#----- 通用的日志输出，被log_error和log_notice函数调用
log_generic () {
  priority="$1"        # 日志级别，有daemon.error和daemon.notice两种类别
  shift

  msg="`eval $DATE` mysqld_safe $*"     # 设置日志格式，时间+mysqld_safe+日志
  echo "$msg"
  case $logging in
    init) ;;                            # 只输出到命令行，不记录到日志
    file) echo "$msg" >> "$err_log" ;;  # 输出到错误日志文件
    # 用logger命令将日志记录到系统日志syslog
    syslog) logger -t "$syslog_tag_mysqld_safe" -p "$priority" "$*" ;;
    both) echo "$msg" >> "$err_log"; logger -t "$syslog_tag_mysqld_safe" -p "$priority" "$*" ;;
    *)
      echo "Internal program error (non-fatal):" \
           " unknown logging method '$logging'" >&2
      ;;
  esac
}

log_error () {
  log_generic ${syslog_facility}.error "$@" >&2
}

log_notice () {
  log_generic ${syslog_facility}.notice "$@"
}

#----- 记录日志的初始化操作，会根据日志类新记录一些初始化日志
eval_log_error () {
  cmd="$1"                              # 最后的eval命令会解析$cmd中的值并执行命令
  case $logging in
    file) cmd="$cmd >> "`shell_quote_string "$err_log"`" 2>&1" ;;
    syslog)
      cmd="$cmd 2>&1 | logger -t '$syslog_tag_mysqld' -p daemon.error"
      ;;
    *)
      echo "Internal program error (non-fatal):" \
           " unknown logging method '$logging'" >&2
    ;;
  esac

  #!!!可以如下命令查看真正执行的命令!!!
  #echo "Running mysqld: [$cmd]"
  eval "$cmd"
}

#----- 转义函数，用于在非"a-z"、"A-Z"、"0-9"、'/'、'_'、'.'、'='、'-'的特殊字符前加上转义字符"\"
shell_quote_string() {
  echo "$1" | sed -e 's,\([^a-zA-Z0-9/_.=-]\),\\\1,g' # sed中的\1代表引用前面\(\)中匹配的值
}

#----- 该函数用于解析配置文件中的选项，并赋值给相应的变量，无法识别则转递给服务器
parse_arguments() {
  pick_args=
  if test "$1" = PICK-ARGS-FROM-ARGV
  then
    pick_args=1
    shift
  fi

  for arg do        # 取出参数值
    val=`echo "$arg" | sed -e "s;--[^=]*=;;"`
    optname=`echo "$arg" | sed -e 's/^\(--[^=]*\)=.*$/\1/'`
    optname_subst=`echo "$optname" | sed 's/_/-/g'`
    arg=`echo $arg | sed "s/^$optname/$optname_subst/"`
    case "$arg" in  # 将参数值传递给对应的变量
      --basedir=*) MY_BASEDIR_VERSION="$val" ;;
      --datadir=*) DATADIR="$val" ;;
      --pid-file=*) pid_file="$val" ;;
      --plugin-dir=*) PLUGIN_DIR="$val" ;;
      --user=*) user="$val"; SET_USER=1 ;;

      # 有些值已经在my.cnf配置文件的[mysqld_safe]组下设置，可被命令行覆盖
      --log-error=*) err_log="$val" ;;
      --port=*) mysql_tcp_port="$val" ;;
      --socket=*) mysql_unix_port="$val" ;;

      # 如下的配置项必须在配置文件的[mysqld_safe]组中设置
      --core-file-size=*) core_file_size="$val" ;;
      --ledir=*) ledir="$val" ;;
      --malloc-lib=*) set_malloc_lib "$val" ;;
      --mysqld=*)
        if [ -z "$pick_args" ]; then
          log_error "--mysqld option can only be used as command line option, found in config file"
          exit 1
        fi
        MYSQLD="$val" ;;
      --mysqld-version=*)
        if [ -z "$pick_args" ]; then
          log_error "--mysqld-version option can only be used as command line option, found in config file"
          exit 1
        fi
        if test -n "$val"
        then
          MYSQLD="mysqld-$val"
          PLUGIN_VARIANT="/$val"
        else
          MYSQLD="mysqld"
        fi
        ;;
      --nice=*) niceness="$val" ;;
      --open-files-limit=*) open_files="$val" ;;
      --open-files-limit=*) open_files="$val" ;;
      --skip-kill-mysqld*) KILL_MYSQLD=0 ;;
      --mysqld-safe-log-timestamps=*) timestamp_format="$val" ;;
      --syslog) want_syslog=1 ;;
      --skip-syslog) want_syslog=0 ;;
      --syslog-tag=*) syslog_tag="$val" ;;
      --timezone=*) TZ="$val"; export TZ; ;;   # 生效了一下时区设置

      --help) usage ;;                         # 调用了usage函数，输出帮助信息

      *)
        if test -n "$pick_args"
        then
          append_arg_to_args "$arg"            # 将其他命令行参数值附加到$arg的后面
        fi
        ;;
    esac
  done
}

# 添加到LD_PRELOAD，使用空格分割
add_mysqld_ld_preload() {
  lib_to_add="$1"
  log_notice "Adding '$lib_to_add' to LD_PRELOAD for mysqld"

  case "$lib_to_add" in
    *' '*)
      # Must strip path from lib, and add it to LD_LIBRARY_PATH
      lib_file=`basename "$lib_to_add"`
      case "$lib_file" in
        *' '*)
          # The lib file itself has a space in its name, and can't
          # be used in LD_PRELOAD
          log_error "library name '$lib_to_add' contains spaces and can not be used with LD_PRELOAD"
          exit 1
          ;;
      esac
      lib_path=`dirname "$lib_to_add"`
      lib_to_add="$lib_file"
      [ -n "$mysqld_ld_library_path" ] && mysqld_ld_library_path="$mysqld_ld_library_path:"
      mysqld_ld_library_path="$mysqld_ld_library_path$lib_path"
      ;;
  esac

  # LD_PRELOAD is a space-separated
  [ -n "$mysqld_ld_preload" ] && mysqld_ld_preload="$mysqld_ld_preload "
  mysqld_ld_preload="${mysqld_ld_preload}$lib_to_add"
}


# Returns LD_PRELOAD (and LD_LIBRARY_PATH, if needed) text, quoted to be
# suitable for use in the eval that calls mysqld.
#
# All values in mysqld_ld_preload are prepended to LD_PRELOAD.
mysqld_ld_preload_text() {
  text=

  if [ -n "$mysqld_ld_preload" ]; then
    new_text="$mysqld_ld_preload"
    [ -n "$LD_PRELOAD" ] && new_text="$new_text $LD_PRELOAD"
    text="${text}LD_PRELOAD="`shell_quote_string "$new_text"`' '
  fi

  if [ -n "$mysqld_ld_library_path" ]; then
    new_text="$mysqld_ld_library_path"
    [ -n "$LD_LIBRARY_PATH" ] && new_text="$new_text:$LD_LIBRARY_PATH"
    text="${text}LD_LIBRARY_PATH="`shell_quote_string "$new_text"`' '
  fi

  echo "$text"
}

# set_malloc_lib LIB
# - If LIB is empty, do nothing and return
# - If LIB is 'tcmalloc', look for tcmalloc shared library in $malloc_dirs.
#   tcmalloc is part of the Google perftools project.
# - If LIB is an absolute path, assume it is a malloc shared library
#
# Put LIB in mysqld_ld_preload, which will be added to LD_PRELOAD when
# running mysqld.  See ld.so for details.
set_malloc_lib() {
  # This list is kept intentionally simple.
  malloc_dirs="/usr/lib /usr/lib64 /usr/lib/i386-linux-gnu /usr/lib/x86_64-linux-gnu"
  malloc_lib="$1"

  if [ "$malloc_lib" = tcmalloc ]; then
    malloc_lib=
    for libdir in $(echo $malloc_dirs); do
      for flavor in _minimal '' _and_profiler _debug; do
        tmp="$libdir/libtcmalloc$flavor.so"
        #log_notice "DEBUG: Checking for malloc lib '$tmp'"
        [ -r "$tmp" ] || continue
        malloc_lib="$tmp"
        break 2
      done
    done

    if [ -z "$malloc_lib" ]; then
      log_error "no shared library for --malloc-lib=tcmalloc found in $malloc_dirs"
      exit 1
    fi
  fi

  # Allow --malloc-lib='' to override other settings
  [ -z  "$malloc_lib" ] && return

  case "$malloc_lib" in
    /*)
      if [ ! -r "$malloc_lib" ]; then
        log_error "--malloc-lib can not be read and will not be used"
        exit 1
      fi

      # Restrict to a the list in $malloc_dirs above
      case "$(dirname "$malloc_lib")" in
        /usr/lib) ;;
        /usr/lib64) ;;
        /usr/lib/i386-linux-gnu) ;;
        /usr/lib/x86_64-linux-gnu) ;;
        *)
          log_error "--malloc-lib must be located in one of the directories: $malloc_dirs"
          exit 1
          ;;
      esac
      ;;
    *)
      log_error "--malloc-lib must be an absolute path or 'tcmalloc'; " \
        "ignoring value '$malloc_lib'"
      exit 1
      ;;
  esac

  add_mysqld_ld_preload "$malloc_lib"
}

#########################################################
# 正式工作开始了！！！
#########################################################

#########################################################
# 1. 查找base目录和mysqld所在目录
#########################################################
if echo '/opt/mysql-5.7/share' | grep '^/opt/mysql-5.7' > /dev/null
then
  # 一口气用了三个替换，分别为：
  # 第一步：将/opt/mysql-5.7/share去除开始的basedir，转换为/share
  # 第二步：将/share开头的/转换为空，也就是share
  # 第三步：在share开头加上./，结果即：./share
  relpkgdata=`echo '/opt/mysql-5.7/share' | sed -e 's,^/opt/mysql-5.7,,' -e 's,^/,,' -e 's,^,./,'`
else
  # pkgdatadir is not relative to prefix
  relpkgdata='/opt/mysql-5.7/share'
fi

case "$0" in
  /*)
  MY_PWD='/opt/mysql-5.7'
  ;;
  *)
  MY_PWD=`dirname $0`
  MY_PWD=`dirname $MY_PWD`
  ;;
esac

# 查找mysqld可执行文件，分别判断了libexec和sbin目录，找不到就使用bin目录
if test -n "$MY_BASEDIR_VERSION" -a -d "$MY_BASEDIR_VERSION"
then
  # BASEDIR is already overridden on command line.  Do not re-set.

  # Use BASEDIR to discover le.
  if test -x "$MY_BASEDIR_VERSION/libexec/mysqld"
  then
    ledir="$MY_BASEDIR_VERSION/libexec"
  elif test -x "$MY_BASEDIR_VERSION/sbin/mysqld"
  then
    ledir="$MY_BASEDIR_VERSION/sbin"
  else
    ledir="$MY_BASEDIR_VERSION/bin"
  fi
elif test -f "$relpkgdata"/english/errmsg.sys -a -x "$MY_PWD/bin/mysqld"
then
  MY_BASEDIR_VERSION="$MY_PWD"		# Where bin, share and data are
  ledir="$MY_PWD/bin"			# Where mysqld is
# Check for the directories we would expect from a source install
elif test -f "$relpkgdata"/english/errmsg.sys -a -x "$MY_PWD/libexec/mysqld"
then
  MY_BASEDIR_VERSION="$MY_PWD"		# Where libexec, share and var are
  ledir="$MY_PWD/libexec"		# Where mysqld is
elif test -f "$relpkgdata"/english/errmsg.sys -a -x "$MY_PWD/sbin/mysqld"
then
  MY_BASEDIR_VERSION="$MY_PWD"		# Where sbin, share and var are
  ledir="$MY_PWD/sbin"			# Where mysqld is
# Since we didn't find anything, used the compiled-in defaults
else
  MY_BASEDIR_VERSION='/opt/mysql-5.7'
  ledir='/opt/mysql-5.7/bin'
fi


#########################################################
# 2. 查找base目录和mysqld所在目录
#########################################################
# 可以从my_print_defaults脚本中获得默认的读取my.cnf顺序，如下
#   Default options are read from the following files in the given order:
#   /etc/mysql/my.cnf /etc/my.cnf ~/.my.cnf
# 2.1 查找下BASEDIR目录
if test -d $MY_BASEDIR_VERSION/data/mysql
then
  DATADIR=$MY_BASEDIR_VERSION/data
# Next try where the source installs put it
elif test -d $MY_BASEDIR_VERSION/var/mysql
then
  DATADIR=$MY_BASEDIR_VERSION/var
# Or just give up and use our compiled-in default
else
  DATADIR=/opt/data
fi

if test -z "$MYSQL_HOME"
then
  MYSQL_HOME=$MY_BASEDIR_VERSION
fi
export MYSQL_HOME

# 2.2 找到my_print_defaults所在目录，并打印[mysqld]和[mysqld_safe]中的配置项，然后与从命令行
#     传入的参数合并，优先级为
if test -x "$MY_BASEDIR_VERSION/bin/my_print_defaults"
then
  print_defaults="$MY_BASEDIR_VERSION/bin/my_print_defaults"
elif test -x `dirname $0`/my_print_defaults
then
  print_defaults="`dirname $0`/my_print_defaults"
elif test -x ./bin/my_print_defaults
then
  print_defaults="./bin/my_print_defaults"
elif test -x /opt/mysql-5.7/bin/my_print_defaults
then
  print_defaults="/opt/mysql-5.7/bin/my_print_defaults"
elif test -x /opt/mysql-5.7/bin/mysql_print_defaults
then
  print_defaults="/opt/mysql-5.7/bin/mysql_print_defaults"
else
  print_defaults="my_print_defaults"
fi

# 这个函数可以将一个指定的参数附加到$arg中(在此同时执行了转义操作)
append_arg_to_args () {
  args="$args "`shell_quote_string "$1"`
}
args=

# 通过SET_USER变量判断来源，2：初始值；1：配置文件或者命令行中配置；0：没有配置
SET_USER=2
# 解析配置文件中的参数，只显示[mysqld]和[server]分组中的内容
parse_arguments `$print_defaults $defaults --loose-verbose mysqld server`
if test $SET_USER -eq 2
then
 SET_USER=0
fi
# 如上，又读取[safe_mysqld]和[mysqld_safe]分组中配置项
parse_arguments `$print_defaults $defaults --loose-verbose mysqld_safe safe_mysqld`
# 用命令行输入选项，也就是$@，从而可以覆盖配置文件中的选项
parse_arguments PICK-ARGS-FROM-ARGV "$@"

# Sort out date command from $timestamp_format early so we'll start off
# with correct log messages.
case "$timestamp_format" in
    UTC|utc)       DATE="date -u +%Y-%m-%dT%H:%M:%S.%06NZ";;
    SYSTEM|system) DATE="date +%Y-%m-%dT%H:%M:%S.%06N%:z";;
    HYPHEN|hyphen) DATE="date +'%Y-%m-%d %H:%M:%S'";;
    LEGACY|legacy) DATE="date +'%y%m%d %H:%M:%S'";;
    *)             DATE="date -u +%Y-%m-%dT%H:%M:%S.%06NZ";
                   log_error "unknown data format $timestamp_format, using UTC";;
esac

# 2.3 查找plugin目录
# Use user-supplied argument
if [ -n "${PLUGIN_DIR}" ]; then
  plugin_dir="${PLUGIN_DIR}"
else
  # Try to find plugin dir relative to basedir
  for dir in lib64/mysql/plugin lib64/plugin lib/mysql/plugin lib/plugin
  do
    if [ -d "${MY_BASEDIR_VERSION}/${dir}" ]; then
      plugin_dir="${MY_BASEDIR_VERSION}/${dir}"
      break
    fi
  done
  # Give up and use compiled-in default
  if [ -z "${plugin_dir}" ]; then
    plugin_dir='/opt/mysql-5.7/lib/plugin'
  fi
fi
plugin_dir="${plugin_dir}${PLUGIN_VARIANT}"

# A pid file is created for the mysqld_safe process. This file protects the
# server instance resources during race conditions.
safe_pid="$DATADIR/mysqld_safe.pid"
if test -f $safe_pid
then
  PID=`cat "$safe_pid"`
  if kill -0 $PID > /dev/null 2> /dev/null
  then
    if ps wwwp $PID | grep -v mysqld_safe | grep -- $MYSQLD > /dev/null
    then
      log_error "A mysqld_safe process already exists"
      exit 1
    fi
  fi
  if [ ! -h "$safe_pid" ]; then
    rm -f "$safe_pid"
  fi
  if test -f "$safe_pid"
  then
    log_error "Fatal error: Can't remove the mysqld_safe pid file"
    exit 1
  fi
fi

# Insert pid proerply into the pid file.
ps -e | grep  [m]ysqld_safe | awk '{print $1}' | sed -n 1p > $safe_pid
# End of mysqld_safe pid(safe_pid) check.



#########################################################
# log日志方式设置，共stdin,file,syslog三种
#########################################################

# 如果是写入到syslog，则判断logger工具是否可用
if [ $want_syslog -eq 1 ]
then
  my_which logger > /dev/null 2>&1
  if [ $? -ne 0 ]
  then
    log_error "--syslog requested, but no 'logger' program found.  Please ensure
     that 'logger' is in your PATH, or do not specify the --syslog option to mysqld_safe."
    if [ ! -h "$safe_pid" ]; then
      rm -f "$safe_pid"                 # Clean Up of mysqld_safe.pid file.
    fi
    exit 1
  fi
fi
if [ $want_syslog -eq 1 ]
then
  if [ -n "$syslog_tag" ]
  then
    # Sanitize the syslog tag
    syslog_tag=`echo "$syslog_tag" | sed -e 's/[^a-zA-Z0-9_-]/_/g'`
    syslog_tag_mysqld_safe="${syslog_tag_mysqld_safe}-$syslog_tag"
    syslog_tag_mysqld="${syslog_tag_mysqld}-$syslog_tag"
  fi
  log_notice "Logging to syslog."
  logging=syslog
fi
# 如果是将日志记录到文件中
if [ -n "$err_log" -o $want_syslog -eq 0 ]
then
  if [ -n "$err_log" ]
  then
    # 如果没有.err后缀，则为err_log添加一个；此时，mysqld和mysqld_safe会写入到
    # 同一个日志文件中。如果是"--log-error=foo."则认为文件名是有效的

    # 这里是识别文件名的字符总数量(包括.)，如果没有设置后缀或者以'/'结尾则返回0
    if expr "$err_log" : '.*\.[^/]*$' > /dev/null
    then
        :
    else
      err_log="$err_log".err
    fi

    case "$err_log" in
      /* ) ;;
      * ) err_log="$DATADIR/$err_log" ;;
    esac
  else
    # 默认设置的错误日志名称
    err_log=$DATADIR/`hostname`.err
  fi

  # 同时会将该参数添加到mysqld启动的参数中
  append_arg_to_args "--log-error=$err_log"

  # 提示日志写入到的文件名称
  log_notice "Logging to '$err_log'."
  if [ $want_syslog -eq 1 ]
  then
    logging=both
  else
    logging=file # 正式把logging改成file使用错误日志来记录日志
  fi

  if [ ! -f "$err_log" ]; then                  # if error log already exists,
    touch "$err_log"                            # we just append. otherwise,
    chmod "$fmode" "$err_log"                   # fix the permissions here!
  fi
fi


# 设置--user选项
USER_OPTION=""
if test -w / -o "$USER" = "root" # 根目录是否可写，或者当前用户为root
then
  if test "$user" != "root" -o $SET_USER = 1
  then
    USER_OPTION="--user=$user"
  fi
  # 创建错误日志，并将日志授权给指定的用户
  if [ $want_syslog -eq 0 -a ! -h "$err_log" ]; then
    touch "$err_log"
    chown $user "$err_log"
  fi
  # 这里它还对当前用户做了ulimit设置，包括可以打开的文件数量--open_files-limit选项
  if test -n "$open_files"
  then
    ulimit -n $open_files
  fi
fi

if test -n "$open_files"
then
  append_arg_to_args "--open-files-limit=$open_files"
fi

safe_mysql_unix_port=${mysql_unix_port:-${MYSQL_UNIX_PORT:-/tmp/mysql.sock}}
# 确保 $safe_mysql_unix_port 目录是存在的
# Make sure that directory for $safe_mysql_unix_port exists
mysql_unix_port_dir=`dirname $safe_mysql_unix_port`
if [ ! -d $mysql_unix_port_dir ]
then
  if [ ! -h $mysql_unix_port_dir ]; then
    mkdir $mysql_unix_port_dir
    chown $user $mysql_unix_port_dir
    chmod 755 $mysql_unix_port_dir
  fi
fi

# 如果用户没有制定mysqld程序的名称，这里就默认赋值为mysqld
# If the user doesn't specify a binary, we assume name "mysqld"
if test -z "$MYSQLD"
then
  MYSQLD=mysqld
fi

# 下面几段分别是对 mysqld ， pid ， port文件选项的检查和设置，省略100个字
if test ! -x "$ledir/$MYSQLD"
then
  log_error "The file $ledir/$MYSQLD
does not exist or is not executable. Please cd to the mysql installation
directory and restart this script from there as follows:
./bin/mysqld_safe&
See http://dev.mysql.com/doc/mysql/en/mysqld-safe.html for more information"
  if [ ! -h "$safe_pid" ]; then
    rm -f "$safe_pid"                 # Clean Up of mysqld_safe.pid file.
  fi
  exit 1
fi

if test -z "$pid_file"
then
  pid_file="$DATADIR/`hostname`.pid"
else
  case "$pid_file" in
    /* ) ;;
    * )  pid_file="$DATADIR/$pid_file" ;;
  esac
fi
append_arg_to_args "--pid-file=$pid_file"

if test -n "$mysql_unix_port"
then
  append_arg_to_args "--socket=$mysql_unix_port"
fi
if test -n "$mysql_tcp_port"
then
  append_arg_to_args "--port=$mysql_tcp_port"
fi

# 接下来是关于优先级的设置
if test $niceness -eq 0
then
  NOHUP_NICENESS="nohup"
else
  NOHUP_NICENESS="nohup nice -$niceness"
fi

# Using nice with no args to get the niceness level is GNU-specific.
# This check could be extended for other operating systems (e.g.,
# BSD could use "nohup sh -c 'ps -o nice -p $$' | tail -1").
# But, it also seems that GNU nohup is the only one which messes
# with the priority, so this is okay.
# 将当前的默认优先级设置为0
if nohup nice > /dev/null 2>&1
then
  # normal_niceness记载默认的调度优先级
    normal_niceness=`nice`
  # nohup_niceness记载使用nohup执行方式的调度优先级
    nohup_niceness=`nohup nice 2>/dev/null`

    numeric_nice_values=1
  # 这个for是为了检查$normal_niceness $nohup_niceness两个变量值的合法性
    for val in $normal_niceness $nohup_niceness
    do
        case "$val" in
            -[0-9] | -[0-9][0-9] | -[0-9][0-9][0-9] | \
             [0-9] |  [0-9][0-9] |  [0-9][0-9][0-9] )
                ;;
            * )
                numeric_nice_values=0 ;;
        esac
    done

  # 这个判断结构很重要
  # 它保证了使用nohup执行的mysqld程序在调度优先级上不会低于直接执行mysqld程序的方式
    if test $numeric_nice_values -eq 1
    then
        nice_value_diff=`expr $nohup_niceness - $normal_niceness`
        if test $? -eq 0 && test $nice_value_diff -gt 0 && \
            nice --$nice_value_diff echo testing > /dev/null 2>&1
        then
            # nohup increases the priority (bad), and we are permitted
            # to lower the priority with respect to the value the user
            # might have been given
      # 进入分支说明$nohup_niceness的值比$normal_niceness大，即nohup执行方式调度优先级比正常执行方式低
      # 这是不希望看到的，所以下面就人为的提升了nohup的优先级（降低niceness的值）
            niceness=`expr $niceness - $nice_value_diff`
            NOHUP_NICENESS="nice -$niceness nohup"
        fi
    fi
else
  # 下面是测试nohup在当前系统中是否可用，不可用的话就置空NOHUP_NICENESS
    if nohup echo testing > /dev/null 2>&1
    then
        :
    else
        # nohup doesn't work on this system
        NOHUP_NICENESS=""
    fi
fi

# Try to set the core file size (even if we aren't root) because many systems
# don't specify a hard limit on core file size.
# 指定core file大小，即使非root用户也尝试设置
if test -n "$core_file_size"
then
  ulimit -c $core_file_size
fi


# 如果已经存在一个pid文件，则检查是否有已经启动的mysqld进程，如果已经启动则退出
if test -f "$pid_file"
then
  PID=`cat "$pid_file"`
  # 检查进程是否存在
  if kill -0 $PID > /dev/null 2> /dev/null
  then
    if ps wwwp $PID | grep -v mysqld_safe | grep -- $MYSQLD > /dev/null
    then    # The pid contains a mysqld process
      log_error "A mysqld process already exists"
      if [ ! -h "$safe_pid" ]; then
        rm -f "$safe_pid"                 # Clean Up of mysqld_safe.pid file.
      fi
      exit 1
    fi
  fi
  if [ ! -h "$pid_file" ]; then
      rm -f "$pid_file"
  fi
  if test -f "$pid_file"
  then
    log_error "Fatal error: Can't remove the pid file:
$pid_file
Please remove it manually and start $0 again;
mysqld daemon not started"
    if [ ! -h "$safe_pid" ]; then
      rm -f "$safe_pid"                 # Clean Up of mysqld_safe.pid file.
    fi
    exit 1
  fi
fi

#
# Uncomment the following lines if you want all tables to be automatically
# checked and repaired during startup. You should add sensible key_buffer
# and sort_buffer values to my.cnf to improve check performance or require
# less disk space.
# Alternatively, you can start mysqld with the "myisam-recover" option. See
# the manual for details.
#
# echo "Checking tables in $DATADIR"
# $MY_BASEDIR_VERSION/bin/myisamchk --silent --force --fast --medium-check $DATADIR/*/*.MYI
# $MY_BASEDIR_VERSION/bin/isamchk --silent --force $DATADIR/*/*.ISM

# Does this work on all systems?
#if type ulimit | grep "shell builtin" > /dev/null
#then
#  ulimit -n 256 > /dev/null 2>&1		# Fix for BSD and FreeBSD systems
#fi


#########################################################
# 开始拼接执行语句
#########################################################
cmd="`mysqld_ld_preload_text`$NOHUP_NICENESS"

# 检查一下命令以及参数，如果需要则进行转义操作
for i in  "$ledir/$MYSQLD" "$defaults" "--basedir=$MY_BASEDIR_VERSION" \
  "--datadir=$DATADIR" "--plugin-dir=$plugin_dir" "$USER_OPTION"
do
  cmd="$cmd "`shell_quote_string "$i"`
done
cmd="$cmd $args"
# Avoid 'nohup: ignoring input' warning
test -n "$NOHUP_NICENESS" && cmd="$cmd < /dev/null"

log_notice "Starting $MYSQLD daemon with databases from $DATADIR"

# variable to track the current number of "fast" (a.k.a. subsecond) restarts
fast_restart=0
# maximum number of restarts before trottling kicks in
max_fast_restarts=5
# flag whether a usable sleep command exists
have_sleep=1


#########################################################
# 后台循环，如果mysqld宕，会尝试自动拉起服务进程
#########################################################
while true
do
  # 一些安全性检查，如果如下文件存在则删除
  if [ ! -h "$safe_mysql_unix_port" ]; then
    rm -f "$safe_mysql_unix_port"
  fi
  if [ ! -h "$pid_file" ]; then
    rm -f "$pid_file"
  fi
  if [ ! -h "$pid_file.shutdown" ]; then
     rm -f  "$pid_file.shutdown"
  fi

  start_time=`date +%M%S`

  # 传入$cmd参数的值，会使用eval命令启动mysqld
  eval_log_error "$cmd"

  if [ $want_syslog -eq 0 -a ! -f "$err_log" -a ! -h "$err_log" ]; then
    touch "$err_log"                    # hypothetical: log was renamed but not
    chown $user "$err_log"              # flushed yet. we'd recreate it with
    chmod "$fmode" "$err_log"           # wrong owner next time we log, so set
  fi                                    # it up correctly while we can!

  end_time=`date +%M%S`

  # 如果正常关闭，也就是检查PID文件被正常删除，则退出分支，不尝试重启进程
  if test ! -f "$pid_file"		# This is removed if normal shutdown
  then
    break
  fi

  # 在进程宕之后，如果不希望重启则直接退出mysqld_safe
  if test -f "$pid_file.shutdown"	# created to signal that it must stop
  then
    log_notice "$pid_file.shutdown present. The server will not restart."
    break
  fi


  # sanity check if time reading is sane and there's sleep
  if test $end_time -gt 0 -a $have_sleep -gt 0
  then
    # 如果在一秒以内完成重启，则尝试睡眠1秒
    if test $end_time -eq $start_time
    then
      fast_restart=`expr $fast_restart + 1`
      if test $fast_restart -ge $max_fast_restarts
      then
        log_notice "The server is respawning too fast. Sleeping for 1 second."
        sleep 1
        sleep_state=$?
        if test $sleep_state -gt 0
        then
          log_notice "The server is respawning too fast and no working sleep command. Turning off trottling."
          have_sleep=0
        fi

        fast_restart=0
      fi
    else
      fast_restart=0
    fi
  fi

  # mysqld_safe已经启动的处理方法，保证只有一个mysqld_safe程序启动
  if true && test $KILL_MYSQLD -eq 1
  then
    # 统计启动的mysqld进程的数目:A)正常为0；B)进程hang，为1；C)有历史进程hang，为多个
    numofproces=`ps xaww | grep -v "grep" | grep "$ledir/$MYSQLD\>" | grep -c "pid-file=$pid_file"`

    log_notice "Number of processes running now: $numofproces"
    I=1
    while test "$I" -le "$numofproces"
    do
      # 过滤启动的MySQL服务进程，通过sed -n '$p'每次只获取最后一行记录
      PROC=`ps xaww | grep "$ledir/$MYSQLD\>" | grep -v "grep" | grep "pid-file=$pid_file" | sed -n '$p'`

      # 上述过滤的第一列为进程ID，使用T变量来获取进程ID
      for T in $PROC
      do
        break
      done

      # 因为已经hang了，直接kill -9强制删除
      if kill -9 $T
      then
        log_error "$MYSQLD process hanging, pid $T - killed"
      else
        break
      fi

      # 每干掉一个mysqld就把I加一，如果有N个mysqld进程，则执行N次kill -9命令
      I=`expr $I + 1`
    done
  fi

  log_notice "mysqld restarted"
done

if [ ! -h "$pid_file.shutdown" ]; then
  rm -f "$pid_file.shutdown"
fi

log_notice "mysqld from pid file $pid_file ended"

if [ ! -h "$safe_pid" ]; then
  rm -f "$safe_pid"                       # Some Extra Safety. File is deleted
fi                                        # once the mysqld process ends.
{% endhighlight %}

简单来说，mysqld_safe 会执行如下命令，其中参数会转换为绝对路径。

{% highlight text %}
$ nohup mysqld ... < /dev/null >> localhost.err 2>&1
{% endhighlight %}

{% highlight text %}
{% endhighlight %}
