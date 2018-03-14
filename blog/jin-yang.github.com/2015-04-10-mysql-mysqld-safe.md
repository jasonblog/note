---
title: MySQL 啟動腳本
layout: post
comments: true
language: chinese
category: [mysql,database]
keywords: mysql,database,啟動,腳本
description: mysqld_safe 是一個 shell 腳本，通常用來啟動 MySQL 服務進程，在這篇文章中，我們看下該腳本具體做了什麼。
---

mysqld_safe 是一個 shell 腳本，通常用來啟動 MySQL 服務進程，在這篇文章中，我們看下該腳本具體做了什麼。


<!-- more -->


## 簡介

該 shell 腳本，詳細交代了 MySQL 的啟動流程：查找 MySQL 相關目錄；解析配置文件；調用 mysqld 程序來啟動實例。常見的配置項有：

{% highlight text %}
--help
    查看幫助。

--no-defaults
    不要讀任何配置項文件。如果給出，必須首選該選項。
--defaults-extra-file=path
    除了通用選項文件所讀取的選項文件名。如果給出，必須首選該選項。
--defaults-file=path
    讀取的代替通用選項文件的選項文件名。如果給出，必須首選該選項。

--basedir=path
    指定 MySQL 安裝目錄的路徑。
--datadir=path
    數據目錄的路徑。

--syslog, --skip-syslog
    是否使用syslog記錄mysqld_safe的日誌。如果是skip-syslog，會寫到文件中，默認是hostname.err。
--log-error
    指定錯誤日誌的文件路徑，需要同時設置--skip-syslog 。
{% endhighlight %}

接下來看看 mysqld_safe 是如何執行的。

### 執行流程簡介

如下是其簡單的處理流程。

##### 1. 檢查系統和選項

強制設置四個選項，並檢查其有效性，包括了默認配置、basedir、datadir、plugin-dir 。

##### 2. 檢查MyISAM表

默認是註釋掉的，也可以在啟動前檢查 MyISAM 表。

##### 3. 監控服務

在 MySQL 服務器異常宕機時，mysqld_safe 默認會自動重啟服務，而正常關閉則不會重啟服務，那麼它是如何判斷 MySQL 是否是正常關閉的呢？能否控制不自動重啟呢？

如果 MySQL 服務正常關閉，默認會刪除 PID 文件，mysqld_safe 會通過判斷是否存在該文件來決定是否要重新拉起 MySQL 服務；當然，如果想即使 MySQL 服務宕機也不啟動文件，也可以新建一個名為 PID 文件名+ ```.shutdown``` 的文件即可，例如 /tmp/mysql.pid.shutdown 。

在此，還有些異常處理：

1. 如果在 MySQL 服務在一秒以內重啟，則說明有異常，此時會通過 ```sleep 1``` 休眠一秒。
2. 在發現 MySQL 宕後，默認會檢查是否有 mysqld 進程 hanging ，如果有，則直接通過 ```kill -9``` 強制關閉。

##### 4. 記錄日誌

將 mysqld 的錯誤消息發送到數據目錄中的 host_name.err 文件，或者寫入到 syslog 中。

<!--
6. 將mysqld_safe的屏幕輸出發送到數據目錄中的host_name.safe文件
-->

## 源碼解析

該腳本實際上最終會通過 ```eval_log_error()``` 函數啟動 MySQL 服務，並將輸出日誌記錄到文件或者 syslog 中，這個函數應該是 mysqld_safe 腳本的核心處理函數。

{% highlight bash %}
#!/bin/sh

#----- 首先是一些變量參數的定義
KILL_MYSQLD=1;              # 試圖kill多餘的mysqld_safe程序，1表示需要kill
MYSQLD=                     # mysqld二進制可執行文件的目錄名稱
niceness=0                  # 進程的調度優先級
mysqld_ld_preload=          # 通常指定libjemalloc.so.1所在路徑
mysqld_ld_library_path=     # 庫目錄

#----- 下面的變量用於設置日誌，默認不打開錯誤日誌，不使用syslog
logging=init                # 日誌記錄狀態，init(輸出到終端)，可以查看log_generic()
want_syslog=0               # 是否要使用syslog
syslog_tag=                 # 如果使用，則需要增加的標示
user='mysql'                # 啟動時使用的用戶，也就是指定--user的選項值
pid_file=                   # pid文件的路徑
err_log=                    # 錯誤日誌的路徑
timestamp_format=UTC        # 使用UTC時間格式

#----- 定義的syslog中標誌位，在後面需要寫入日誌到syslog中時使用
syslog_tag_mysqld=mysqld
syslog_tag_mysqld_safe=mysqld_safe
syslog_facility=daemon

trap '' 1 2 3 15            # 不允許程序在終端上被kill，包括掛起、中斷、退出、系統終止
trap '' 13                  # 也包括SIGPIPE，可以通過kill -l查看數字對應的信號量

#----- 設置默認權限為770，可以查看umask(2)，為了兼容所以處理的時候比較複雜
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

#----- 使用的默認配置文件，可以在命令行中指定
defaults=
case "$1" in
  --no-defaults|--defaults-file=*|--defaults-extra-file=*)
   defaults="$1"; shift
   ;;
esac

#----- 使用--help選項時輸出的使用幫助信息
usage () {
        cat <<EOF
Usage: $0 [OPTIONS]
  --no-defaults              Don't read the system defaults file
  ... ...
EOF
        exit 1
}

#----- 相當於which，檢索$PATH中的路徑，打印出命令的全路徑
my_which ()        # 該函數只在my_which logger用到，也就是轉換為/usr/bin/logger
{
  save_ifs="${IFS-UNSET}"     # 保存當前的分隔符，用於後面恢復IFS
  IFS=:                       # 使用:來分割PATH中的路徑
  ret=0
  for file                    # 等同於for file in $*
  do
    for dir in $PATH
    do
      if [ -f "$dir/$file" ]
      then
        echo "$dir/$file"
        continue 2            # 跳出外層循環
      fi
    done

	ret=1  #signal an error
	break
  done

  if [ "$save_ifs" = UNSET ]  # 重置IFS變量
  then
    unset IFS
  else
    IFS="$save_ifs"
  fi
  return $ret  # Success
}

#----- 通用的日誌輸出，被log_error和log_notice函數調用
log_generic () {
  priority="$1"        # 日誌級別，有daemon.error和daemon.notice兩種類別
  shift

  msg="`eval $DATE` mysqld_safe $*"     # 設置日誌格式，時間+mysqld_safe+日誌
  echo "$msg"
  case $logging in
    init) ;;                            # 只輸出到命令行，不記錄到日誌
    file) echo "$msg" >> "$err_log" ;;  # 輸出到錯誤日誌文件
    # 用logger命令將日誌記錄到系統日誌syslog
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

#----- 記錄日誌的初始化操作，會根據日誌類新記錄一些初始化日誌
eval_log_error () {
  cmd="$1"                              # 最後的eval命令會解析$cmd中的值並執行命令
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

  #!!!可以如下命令查看真正執行的命令!!!
  #echo "Running mysqld: [$cmd]"
  eval "$cmd"
}

#----- 轉義函數，用於在非"a-z"、"A-Z"、"0-9"、'/'、'_'、'.'、'='、'-'的特殊字符前加上轉義字符"\"
shell_quote_string() {
  echo "$1" | sed -e 's,\([^a-zA-Z0-9/_.=-]\),\\\1,g' # sed中的\1代表引用前面\(\)中匹配的值
}

#----- 該函數用於解析配置文件中的選項，並賦值給相應的變量，無法識別則轉遞給服務器
parse_arguments() {
  pick_args=
  if test "$1" = PICK-ARGS-FROM-ARGV
  then
    pick_args=1
    shift
  fi

  for arg do        # 取出參數值
    val=`echo "$arg" | sed -e "s;--[^=]*=;;"`
    optname=`echo "$arg" | sed -e 's/^\(--[^=]*\)=.*$/\1/'`
    optname_subst=`echo "$optname" | sed 's/_/-/g'`
    arg=`echo $arg | sed "s/^$optname/$optname_subst/"`
    case "$arg" in  # 將參數值傳遞給對應的變量
      --basedir=*) MY_BASEDIR_VERSION="$val" ;;
      --datadir=*) DATADIR="$val" ;;
      --pid-file=*) pid_file="$val" ;;
      --plugin-dir=*) PLUGIN_DIR="$val" ;;
      --user=*) user="$val"; SET_USER=1 ;;

      # 有些值已經在my.cnf配置文件的[mysqld_safe]組下設置，可被命令行覆蓋
      --log-error=*) err_log="$val" ;;
      --port=*) mysql_tcp_port="$val" ;;
      --socket=*) mysql_unix_port="$val" ;;

      # 如下的配置項必須在配置文件的[mysqld_safe]組中設置
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
      --timezone=*) TZ="$val"; export TZ; ;;   # 生效了一下時區設置

      --help) usage ;;                         # 調用了usage函數，輸出幫助信息

      *)
        if test -n "$pick_args"
        then
          append_arg_to_args "$arg"            # 將其他命令行參數值附加到$arg的後面
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
# 正式工作開始了！！！
#########################################################

#########################################################
# 1. 查找base目錄和mysqld所在目錄
#########################################################
if echo '/opt/mysql-5.7/share' | grep '^/opt/mysql-5.7' > /dev/null
then
  # 一口氣用了三個替換，分別為：
  # 第一步：將/opt/mysql-5.7/share去除開始的basedir，轉換為/share
  # 第二步：將/share開頭的/轉換為空，也就是share
  # 第三步：在share開頭加上./，結果即：./share
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

# 查找mysqld可執行文件，分別判斷了libexec和sbin目錄，找不到就使用bin目錄
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
# 2. 查找base目錄和mysqld所在目錄
#########################################################
# 可以從my_print_defaults腳本中獲得默認的讀取my.cnf順序，如下
#   Default options are read from the following files in the given order:
#   /etc/mysql/my.cnf /etc/my.cnf ~/.my.cnf
# 2.1 查找下BASEDIR目錄
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

# 2.2 找到my_print_defaults所在目錄，並打印[mysqld]和[mysqld_safe]中的配置項，然後與從命令行
#     傳入的參數合併，優先級為
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

# 這個函數可以將一個指定的參數附加到$arg中(在此同時執行了轉義操作)
append_arg_to_args () {
  args="$args "`shell_quote_string "$1"`
}
args=

# 通過SET_USER變量判斷來源，2：初始值；1：配置文件或者命令行中配置；0：沒有配置
SET_USER=2
# 解析配置文件中的參數，只顯示[mysqld]和[server]分組中的內容
parse_arguments `$print_defaults $defaults --loose-verbose mysqld server`
if test $SET_USER -eq 2
then
 SET_USER=0
fi
# 如上，又讀取[safe_mysqld]和[mysqld_safe]分組中配置項
parse_arguments `$print_defaults $defaults --loose-verbose mysqld_safe safe_mysqld`
# 用命令行輸入選項，也就是$@，從而可以覆蓋配置文件中的選項
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

# 2.3 查找plugin目錄
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
# log日誌方式設置，共stdin,file,syslog三種
#########################################################

# 如果是寫入到syslog，則判斷logger工具是否可用
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
# 如果是將日誌記錄到文件中
if [ -n "$err_log" -o $want_syslog -eq 0 ]
then
  if [ -n "$err_log" ]
  then
    # 如果沒有.err後綴，則為err_log添加一個；此時，mysqld和mysqld_safe會寫入到
    # 同一個日誌文件中。如果是"--log-error=foo."則認為文件名是有效的

    # 這裡是識別文件名的字符總數量(包括.)，如果沒有設置後綴或者以'/'結尾則返回0
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
    # 默認設置的錯誤日誌名稱
    err_log=$DATADIR/`hostname`.err
  fi

  # 同時會將該參數添加到mysqld啟動的參數中
  append_arg_to_args "--log-error=$err_log"

  # 提示日誌寫入到的文件名稱
  log_notice "Logging to '$err_log'."
  if [ $want_syslog -eq 1 ]
  then
    logging=both
  else
    logging=file # 正式把logging改成file使用錯誤日誌來記錄日誌
  fi

  if [ ! -f "$err_log" ]; then                  # if error log already exists,
    touch "$err_log"                            # we just append. otherwise,
    chmod "$fmode" "$err_log"                   # fix the permissions here!
  fi
fi


# 設置--user選項
USER_OPTION=""
if test -w / -o "$USER" = "root" # 根目錄是否可寫，或者當前用戶為root
then
  if test "$user" != "root" -o $SET_USER = 1
  then
    USER_OPTION="--user=$user"
  fi
  # 創建錯誤日誌，並將日誌授權給指定的用戶
  if [ $want_syslog -eq 0 -a ! -h "$err_log" ]; then
    touch "$err_log"
    chown $user "$err_log"
  fi
  # 這裡它還對當前用戶做了ulimit設置，包括可以打開的文件數量--open_files-limit選項
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
# 確保 $safe_mysql_unix_port 目錄是存在的
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

# 如果用戶沒有制定mysqld程序的名稱，這裡就默認賦值為mysqld
# If the user doesn't specify a binary, we assume name "mysqld"
if test -z "$MYSQLD"
then
  MYSQLD=mysqld
fi

# 下面幾段分別是對 mysqld ， pid ， port文件選項的檢查和設置，省略100個字
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

# 接下來是關於優先級的設置
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
# 將當前的默認優先級設置為0
if nohup nice > /dev/null 2>&1
then
  # normal_niceness記載默認的調度優先級
    normal_niceness=`nice`
  # nohup_niceness記載使用nohup執行方式的調度優先級
    nohup_niceness=`nohup nice 2>/dev/null`

    numeric_nice_values=1
  # 這個for是為了檢查$normal_niceness $nohup_niceness兩個變量值的合法性
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

  # 這個判斷結構很重要
  # 它保證了使用nohup執行的mysqld程序在調度優先級上不會低於直接執行mysqld程序的方式
    if test $numeric_nice_values -eq 1
    then
        nice_value_diff=`expr $nohup_niceness - $normal_niceness`
        if test $? -eq 0 && test $nice_value_diff -gt 0 && \
            nice --$nice_value_diff echo testing > /dev/null 2>&1
        then
            # nohup increases the priority (bad), and we are permitted
            # to lower the priority with respect to the value the user
            # might have been given
      # 進入分支說明$nohup_niceness的值比$normal_niceness大，即nohup執行方式調度優先級比正常執行方式低
      # 這是不希望看到的，所以下面就人為的提升了nohup的優先級（降低niceness的值）
            niceness=`expr $niceness - $nice_value_diff`
            NOHUP_NICENESS="nice -$niceness nohup"
        fi
    fi
else
  # 下面是測試nohup在當前系統中是否可用，不可用的話就置空NOHUP_NICENESS
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
# 指定core file大小，即使非root用戶也嘗試設置
if test -n "$core_file_size"
then
  ulimit -c $core_file_size
fi


# 如果已經存在一個pid文件，則檢查是否有已經啟動的mysqld進程，如果已經啟動則退出
if test -f "$pid_file"
then
  PID=`cat "$pid_file"`
  # 檢查進程是否存在
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
# 開始拼接執行語句
#########################################################
cmd="`mysqld_ld_preload_text`$NOHUP_NICENESS"

# 檢查一下命令以及參數，如果需要則進行轉義操作
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
# 後臺循環，如果mysqld宕，會嘗試自動拉起服務進程
#########################################################
while true
do
  # 一些安全性檢查，如果如下文件存在則刪除
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

  # 傳入$cmd參數的值，會使用eval命令啟動mysqld
  eval_log_error "$cmd"

  if [ $want_syslog -eq 0 -a ! -f "$err_log" -a ! -h "$err_log" ]; then
    touch "$err_log"                    # hypothetical: log was renamed but not
    chown $user "$err_log"              # flushed yet. we'd recreate it with
    chmod "$fmode" "$err_log"           # wrong owner next time we log, so set
  fi                                    # it up correctly while we can!

  end_time=`date +%M%S`

  # 如果正常關閉，也就是檢查PID文件被正常刪除，則退出分支，不嘗試重啟進程
  if test ! -f "$pid_file"		# This is removed if normal shutdown
  then
    break
  fi

  # 在進程宕之後，如果不希望重啟則直接退出mysqld_safe
  if test -f "$pid_file.shutdown"	# created to signal that it must stop
  then
    log_notice "$pid_file.shutdown present. The server will not restart."
    break
  fi


  # sanity check if time reading is sane and there's sleep
  if test $end_time -gt 0 -a $have_sleep -gt 0
  then
    # 如果在一秒以內完成重啟，則嘗試睡眠1秒
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

  # mysqld_safe已經啟動的處理方法，保證只有一個mysqld_safe程序啟動
  if true && test $KILL_MYSQLD -eq 1
  then
    # 統計啟動的mysqld進程的數目:A)正常為0；B)進程hang，為1；C)有歷史進程hang，為多個
    numofproces=`ps xaww | grep -v "grep" | grep "$ledir/$MYSQLD\>" | grep -c "pid-file=$pid_file"`

    log_notice "Number of processes running now: $numofproces"
    I=1
    while test "$I" -le "$numofproces"
    do
      # 過濾啟動的MySQL服務進程，通過sed -n '$p'每次只獲取最後一行記錄
      PROC=`ps xaww | grep "$ledir/$MYSQLD\>" | grep -v "grep" | grep "pid-file=$pid_file" | sed -n '$p'`

      # 上述過濾的第一列為進程ID，使用T變量來獲取進程ID
      for T in $PROC
      do
        break
      done

      # 因為已經hang了，直接kill -9強制刪除
      if kill -9 $T
      then
        log_error "$MYSQLD process hanging, pid $T - killed"
      else
        break
      fi

      # 每幹掉一個mysqld就把I加一，如果有N個mysqld進程，則執行N次kill -9命令
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

簡單來說，mysqld_safe 會執行如下命令，其中參數會轉換為絕對路徑。

{% highlight text %}
$ nohup mysqld ... < /dev/null >> localhost.err 2>&1
{% endhighlight %}

{% highlight text %}
{% endhighlight %}
