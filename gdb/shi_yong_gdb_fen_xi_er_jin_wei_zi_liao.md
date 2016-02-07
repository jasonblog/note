# 使用GDB分析二進位資料


假設我們的程式碼是 stat.c，取自 stat() example。

- stat.c

```c
stat() example

Script started on Fri May 02 12:54:12 1997
sh-2.00$ cat statfile.c
/* statfile - get file status using stat(path, buf) system call.
   Synopsis:
      include files:  <sys/types.h>
                      <sys/stat.h>

     int stat(path, buf)
     char * path;
     struct stat * buf;

From /usr/include/sys/stat.h, we have the following description of the
stat structure, used by stat and fstat.

struct  stat
{
	dev_t	st_dev;			 ID of device containing a directory
					   entry for this file.  File serial
					   no + device ID uniquely identify 
					   the file within the system 
	ino_t	st_ino;			 File serial number 
	mode_t	st_mode;		 File mode; see #define's below 
	nlink_t	st_nlink;		 Number of links 
	uid_t	st_uid;			 User ID of the file's owner 
	gid_t	st_gid;			 Group ID of the file's group 
	dev_t	st_rdev;		 ID of device 
					   This entry is defined only for 
					   character or block special files 
	off_t	st_size;		 File size in bytes 
	time_t	st_atime;		 Time of last access 
	int	st_spare1;
	time_t	st_mtime;		 Time of last data modification 
	int	st_spare2;
	time_t	st_ctime;		 Time of last file status change 
	int	st_spare3;
					 Time measured in seconds since 
					   00:00:00 GMT, Jan. 1, 1970 
	uint_t	st_blksize;		 Size of block in file 
        int    st_blocks;                # blocks allocated for file 
        uint_t  st_flags;                user defined flags for file 
        uint_t  st_gen;                  file generation number 

};

*****************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>

void report(char *, struct stat *);

void main(int argc, char * argv[])
/* Print status of files on argument list. */
{
   struct stat status_buf;
   if (argc < 2)
   {
      fprintf(stderr, "statfile file1 ...\n");
      exit(-1);
   }
   while(--argc)
   {
      if( stat(*++argv, &status_buf) < 0 )
         perror(*argv);
      else
         report(*argv, &status_buf);
   }
}

/* Possible access modes... one for each octal value. */
char * accesses[] = {"...", "..x", ".w.", ".wx", "r..", "r.x", "rw.", "rwx"};

void report( char * name, struct stat * buffer)
/* Decode and present the status information. */
{
   int i;
   struct passwd * passent;
   ushort mode = buffer -> st_mode;
   printf("\n %s :\n", name);
   printf(" Last access : %s\n", ctime( &(buffer -> st_atime)) );
   printf(" Last modification : %s\n", ctime( &(buffer -> st_mtime)));
   printf(" Last status change : %s\n", ctime( &(buffer -> st_ctime)));
   printf(" Current file size : %ld \n", buffer -> st_size);

   /* type dev_t is int*/
   printf(" Directory entry is on device %d.\n", buffer -> st_dev);

   /* type ino_t is unsigned  */
   printf(" Inode number is %lu\n", buffer -> st_ino);

   /* Identify the owner by number and by name. */
   passent = getpwuid(buffer -> st_uid);
   if(passent != NULL)
      printf("The owner of the file is #%d - %s\n",
          buffer -> st_uid, passent -> pw_name);
   else
      printf(" The owner of the file is #%d - unknown\n", buffer -> st_uid);

   printf(" Access mode 0%o: ", mode);
   for(i = 6; i >= 0; i -=3)
      printf("%s", accesses[(mode >> i) & 7]);
   printf("\n");
}


sh-2.00$ statfile pi*

 pipe3a.c :
 Last access : Wed Apr 30 08:09:06 1997

 Last modification : Sat Apr  1 07:53:41 1995

 Last status change : Wed Jan 29 10:05:55 1997

 Current file size : 10046 
 Directory entry is on device 8393735.
 Inode number is 59931
The owner of the file is #20 - rossa
 Access mode 0100640: rw.r.....

 pipe3b.c :
 Last access : Wed Apr 30 08:09:06 1997

 Last modification : Sat Apr  1 11:47:31 1995

 Last status change : Wed Jan 29 10:05:55 1997

 Current file size : 10838 
 Directory entry is on device 8393735.
 Inode number is 59932
The owner of the file is #20 - rossa
 Access mode 0100640: rw.r.....

 pipeblock.c :
 Last access : Wed Apr 30 08:09:06 1997

 Last modification : Thu Apr  6 08:20:33 1995

 Last status change : Wed Jan 29 10:05:55 1997

 Current file size : 641 
 Directory entry is on device 8393735.
 Inode number is 59935
The owner of the file is #20 - rossa
 Access mode 0100640: rw.r.....

 pipes.ex :
 Last access : Wed Apr 30 08:09:06 1997

 Last modification : Thu Apr  6 12:32:15 1995

 Last status change : Wed Jan 29 10:05:55 1997

 Current file size : 3707 
 Directory entry is on device 8393735.
 Inode number is 59937
The owner of the file is #20 - rossa
 Access mode 0100640: rw.r.....
sh-2.00$ exit
exit

script done on Fri May 02 12:54:40 1997
```

編譯並執行程式:
```c
gcc -o stat -g stat.c
./stat stat.c
```

預期會看到以下輸出:

```c
...
 Current file size : 1712 
 Directory entry is on device 2065.
 Inode number is 393676
The owner of the file is #1000 - jserv
 Access mode 0100664: rw.rw.r..
 
 ```
 
 ## 用 GDB 觀察
 
 ```c
 $ gdb -q stat
Reading symbols from stat...done.
(gdb) list 20
15              fprintf(stderr, "statfile file1 ...\n");
16              exit(-1);
17           }
18           while(--argc)
19           {
20              if( stat(*++argv, &status_buf) < 0 )
21                 perror(*argv);
22              else
23                 report(*argv, &status_buf);
24          
```

把中斷點設定在輸出之前

```c
(gdb) break 23
Breakpoint 1 at 0x40086e: file stat.c, line 23.
(gdb) run stat.c
Starting program: /tmp/stat stat.c

Breakpoint 1, main (argc=1, argv=0x7fffffffe0d0) at stat.c:23
23                 report(*argv, &status_buf);
```
 
觀察 status_buf 這個 struct stat 的內容:

```c
(gdb) print &status_buf
$1 = (struct stat *) 0x7fffffffdf40
(gdb) print status_buf
$2 = {st_dev = 2065, st_ino = 393676, st_nlink = 1, st_mode = 33204, 
  st_uid = 1000, st_gid = 1000, __pad0 = 0, st_rdev = 0, st_size = 1712, 
  st_blksize = 4096, st_blocks = 8, st_atim = {tv_sec = 1454821389, 
    tv_nsec = 333331710}, st_mtim = {tv_sec = 1454821384, 
    tv_nsec = 649439778}, st_ctim = {tv_sec = 1454821384, 
    tv_nsec = 649439778}, __glibc_reserved = {0, 0, 0}}
(gdb) print sizeof(struct stat)
$3 = 144
```

準備將 struct stat 的內容輸出到外部檔案


```c
(gdb) print 0x7fffffffdf40 + 144
$4 = 140737488347088
(gdb) dump memory output-file 0x7fffffffdf40 0x7fffffffdfd0
```

注意到十進位的 140737488347088 就等於 0x7fffffffdfd0

按下 `Ctrl-D`，結束 gdb。接著再重新執行 stat 執行檔一次。這次執行時不用加參數 (argument)，因為我們只在意 struct stat 的內容：


```c
$ gdb -q stat
Reading symbols from stat...done.
(gdb) break main
Breakpoint 1 at 0x4007ee: file stat.c, line 11.
(gdb) run
Starting program: /tmp/stat 

Breakpoint 1, main (argc=1, argv=0x7fffffffe0e8) at stat.c:11
11        {
(gdb) list
6        
7        void report(char *, struct stat *);
8        
9        void main(int argc, char * argv[])
10        /* Print status of files on argument list. */
11        {
12           struct stat status_buf;
13           if (argc < 2)
14           {
15              fprintf(stderr, "statfile file1 ...\n");
```

如果這時候去觀察 struct stat 的內容，當然是沒什麼意義：

```c
(gdb) print status_buf
$1 = {st_dev = 140737347973624, st_ino = 140737354101952, st_nlink = 0, 
  st_mode = 0, st_uid = 0, st_gid = 0, __pad0 = 0, st_rdev = 0, st_size = 0, 
  st_blksize = 140737488347384, st_blocks = 1, st_atim = {
    tv_sec = 140737488347168, tv_nsec = 140737354101952}, st_mtim = {
    tv_sec = 0, tv_nsec = 1}, st_ctim = {tv_sec = 4197021, tv_nsec = 4196064}, 
  __glibc_reserved = {0, 4196944, 4196064}}
```

比方說 `st_mode = 0, st_uid = 0, st_gid = 0` 這根本不符合現在的環境。

但我們可以之前輸出的檔案 output-file 還原記憶體內容:

```c
(gdb) print &status_buf
$2 = (struct stat *) 0x7fffffffdf60
(gdb) restore output-file binary 0x7fffffffdf60
Restoring binary file output-file into memory (0x7fffffffdf60 to 0x7fffffffdff0
```
這時候再來觀察一次:

```c
(gdb) print status_buf
$3 = {st_dev = 2065, st_ino = 393676, st_nlink = 1, st_mode = 33204, 
  st_uid = 1000, st_gid = 1000, __pad0 = 0, st_rdev = 0, st_size = 1712, 
  st_blksize = 4096, st_blocks = 8, st_atim = {tv_sec = 1454821389, 
    tv_nsec = 333331710}, st_mtim = {tv_sec = 1454821384, 
    tv_nsec = 649439778}, st_ctim = {tv_sec = 1454821384, 
    tv_nsec = 649439778}, __glibc_reserved = {0, 0, 0}}
```

這時候就有意義了，比方說 `st_uid = 1000, st_gid = 1000` 就是我自己。

所以這時我們就可以作以下操作：

```c
(gdb) print status_buf.st_uid
$4 = 1000
(gdb) print status_buf.st_atim
$5 = {tv_sec = 1454821389, tv_nsec = 333331710}
```

要計算 offset 也行，這也是在其他程式語言中，讀取檔案的 seek 偏移量。

```c
(gdb) print &status_buf.st_atim
$6 = (struct timespec *) 0x7fffffffdfa8
(gdb) print &status_buf.st_size
$7 = (__off_t *) 0x7fffffffdf90
(gdb) print 0x7fffffffdfa8 - 0x7fffffffdf90
$8 = 24
```


## 用工具或程式語言擷取二進位內容


基於前述的實驗，我們要分析輸出的檔案就容易了。比方說我們先計算 st_size 的偏移量


```c
(gdb) print 0x7fffffffdf90 - 0x7fffffffdf60
$1 = 48
(gdb) whatis status_buf->st_size
type = __off_t
(gdb) print sizeof(status_buf->st_size)
$9 = 8
```

用 hexdump 來對照檔案內容：
```c
(gdb) shell hexdump output-file
0000000 0811 0000 0000 0000 01cc 0006 0000 0000
0000010 0001 0000 0000 0000 81b4 0000 03e8 0000
0000020 03e8 0000 0000 0000 0000 0000 0000 0000
0000030 06b0 0000 0000 0000 1000 0000 0000 0000
0000040 0008 0000 0000 0000 d00d 56b6 0000 0000
0000050 3cfe 13de 0000 0000 d008 56b6 0000 0000
0000060 aa22 26b5 0000 0000 d008 56b6 0000 0000
0000070 aa22 26b5 0000 0000 0000 0000 0000 0000
0000080 0000 0000 0000 0000 0000 0000 0000 0000
0000090
```


status_buf->st_size 距離 status_buf 開頭的偏移量為 48，而 10 進位的 48 就是 16 進位的 0x30，要注意上面的 hexdump 單位都是 16 進位，所以我們目標放在這行的前 8 bytes:

```c
0000030 | 06b0 0000 0000 0000 1000 0000 0000 0000
```

顯然 0x000006b0 就是 10 進位的 1712，也就是 status_buf.st_size 的內容。
