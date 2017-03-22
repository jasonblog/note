# Linux 的檔案系統



在 UNIX/Linux 的使用者的腦海中，檔案系統是一種邏輯概念，而非實體的裝置。這種邏輯概念包含『檔案』、『目錄』、『路徑』、『檔案屬性』等等。我們可以用物件導向的方式將這些邏輯概念視為物件，表格 1 就顯示了這些物件的範例與意義。

表格 1. 檔案系統中的基本邏輯概念

<table class="wiki-content-table">
<tbody><tr>
<th>概念</th>
<th>範例</th>
<th>說明</th>
</tr>
<tr>
<td>路徑</td>
<td>/home/ccc/hello.txt</td>
<td>檔案在目錄結構中的位置</td>
</tr>
<tr>
<td>目錄</td>
<td>/home/ccc/</td>
<td>資料夾中所容納的項目索引 (包含子目錄或檔案之屬性與連結)</td>
</tr>
<tr>
<td>檔案</td>
<td>Hello!\n…</td>
<td>檔案的內容</td>
</tr>
<tr>
<td>屬性</td>
<td>-rwxr-xr— …ccc None 61 Jun 25&nbsp;12:17 README.txt</td>
<td>檔案的名稱、權限、擁有者、修改日期等資訊</td>
</tr>
</tbody></table>

Linux 檔案系統的第一層目錄如表格 2 所示，認識這些目錄才能有效的運用 Linux 的檔案系統，通常使用者從命令列登入後會到達個人用戶的主目錄，像是使用者 ccc 登入後就會到 /home/ccc 目錄當中。目錄 /dev 所代表的是裝置 (device)，其中每一個子目錄通常代表一個裝置，這些裝置可以被掛載到 /mount 資料夾下，形成一顆邏輯目錄樹。

表格 2. Linux 檔案系統的第一層目錄

<table class="wiki-content-table">
<tbody><tr>
<th>目錄</th>
<th>全名</th>
<th>說明</th>
</tr>
<tr>
<td>/bin</td>
<td>Binary</td>
<td>存放二進位的可執行檔案</td>
</tr>
<tr>
<td>/dev</td>
<td>Device</td>
<td>代表設備，存放裝置相關檔案</td>
</tr>
<tr>
<td>/etc</td>
<td>Etc…</td>
<td>存放系統管理與配置檔案，像是服務程式 httpd 與 host.conf 等檔案。</td>
</tr>
<tr>
<td>/home</td>
<td>Home</td>
<td>用戶的主目錄，每個使用者在其中都會有一個子資料夾，例如用戶 ccc 的資料夾為 /home/ccc/</td>
</tr>
<tr>
<td>/lib</td>
<td>Library</td>
<td>包含系統函式庫與動態連結函式庫</td>
</tr>
<tr>
<td>/sbin</td>
<td>System binary</td>
<td>系統管理程式，通常由系統管理員使用</td>
</tr>
<tr>
<td>/tmp</td>
<td>Temp</td>
<td>暫存檔案</td>
</tr>
<tr>
<td>/root</td>
<td>Root directory</td>
<td>系統的根目錄，通常由系統管理員使用</td>
</tr>
<tr>
<td>/mnt</td>
<td>Mount</td>
<td>用戶所掛載上去的檔案系統，通常放在此目錄下</td>
</tr>
<tr>
<td>/proc</td>
<td>Process</td>
<td>一個虛擬的目錄，代表整個記憶體空間的映射區，可以透過存取此目錄取得系統資訊。</td>
</tr>
<tr>
<td>/var</td>
<td>Variable</td>
<td>存放各種服務的日誌等檔案</td>
</tr>
<tr>
<td>/usr</td>
<td>User</td>
<td>龐大的目錄，所有的使用者程式與檔案都放在底下，像是 /usr/src 中就存放了 Linux 核心的原始碼， 而 /usr/bin 則存放所有的開發工具環境，像是 javac, java, gcc, perl 等。(若類比到 MS. Windows，此資料夾就像是 C:\Program Files)</td>
</tr>
</tbody></table>

然而，檔案畢竟是儲存在區塊裝置中的資料，要如何將這些概念化為區塊資料的組合，必須依賴某些資料結構。為了能將目錄、檔案、屬性、路徑這些物件儲存在區塊當中。這些區塊必須被進一步組織成更巨大的單元，這種巨型單元稱為分割 (Partition)。

在 MS. Windows 中，分割是以 A: B: C: D: 這樣的概念形式呈現的。一個分割概念再 Windows 中通常稱為『槽』。由於歷史的因素，通常 A: B: 槽代表軟碟機，而 C:槽代表第一顆硬碟，D: E: …. 槽則可能是光碟、硬碟、或隨身碟等等。

但是並非一個槽就代表單一的裝置，有時一個裝置會包含好幾個分割，像是許多人都會將主硬碟進一步分割成兩個 Partition，形成 C: D: 兩個槽，但實際上卻儲存在同一個硬碟裝置中，Linux 中的 Partition 的概念也與 Windows 類似，但是命名方式卻有所不同。

在Linux 中並沒有槽的概念，而是直接將裝置映射到 /dev 資料夾的某個檔案路徑中。舉例而言，第一顆硬碟的第一個分割通常稱為 /dev/hda1，第二個分割則為 /dev/hda2, …。而第二顆硬碟的第一個分割區則為 /dev/hdb1，…。軟碟則被映射到 /dev/sda1, /dev/sda2, …./dev/sdb1, ….，以此類推。

在 Linux 中，我們可以利用 mount 這個指令，將某個分割 (槽) 掛載到檔案系統的某個節點中，這樣就不需要知道某個資料夾 (像是 /mnt) 到底是何種檔案系統，整個檔案系統形成一顆與硬體無關的樹狀結構。舉例而言，mount -t ext2 /dev/hda3 /mnt 這個指令可以將Ext2格式的硬碟分割區 /dev/hda3 掛載到 /mnt 目錄下。而 mount -t iso9600 -o ro /dev/cdrom /mnt/cdrom 這樣的指令則可將 iso9600 格式的光碟 /dev/cdrom 以唯讀的方式掛載到 /mnt/cdrom路徑當中。當然，我們也可以用 unmount 指令將這些掛載上去的裝置移除。

當我們使用 ls -all 這樣的指令以列出資料夾中的目錄結構時，看到的就是這些概念所反映出的資訊。圖 1 就顯示了 ls 指令所呈現出的資訊與所對應的概念 。


![](images/LinuxFileDirectory.jpg)

圖 1 UNIX/Linux 中的檔案與目錄概念

圖 1 中的權限欄位，第一個字元代表項目標記，目錄會以d標記，而檔案會以 - 標記，後面九個字元分為三群，分別是檔案擁有者權限、群組成員權限、一般人權限等等。例如，檔案 README.txt 的權限欄為 -rwxr-x-r，可以被分為三個一組，改寫為 owner(rwx)+group(r-x)+others(r)。這代表該檔案的使用者具有讀取 Read (r)、寫入 Write (w) 與執行 eXecute (x) 的權限，群組成員則只能讀取與執行，而其他人則只能讀取。

擁有權限者可以修改檔案的屬性，像是 chown 指令可以改變檔案的擁有者，chgrp 指令可以改變檔案的所屬群組，chmod 可以改變檔案的存取權限等。


##Linux 的檔案程式設計

而對於 Linux 程式設計師而言，關心的則是如何用程式對這些『物件』進行操作，像是開檔、讀檔、關檔、改變屬性等動作。表格 3 顯示了如何利用程式對這些物件進行操作的一些基本方法。

表格 3. 程式對檔案系統的基本操作

<table class="wiki-content-table">
<tbody><tr>
<th>物件</th>
<th>範例</th>
<th>說明</th>
</tr>
<tr>
<td>檔案</td>
<td>fd = open(“/myfile”…)</td>
<td>開關檔案</td>
</tr>
<tr>
<td>檔案</td>
<td>write, read, lseek</td>
<td>讀寫檔案</td>
</tr>
<tr>
<td>屬性</td>
<td>stat(“/myfile”, &amp;mybuf)</td>
<td>修改屬性</td>
</tr>
<tr>
<td>目錄</td>
<td>DIR *dh = opendir(“/mydir”)</td>
<td>開啟目錄</td>
</tr>
<tr>
<td>目錄</td>
<td>struct dirent *ent = readdir(dh)</td>
<td>讀取目錄</td>
</tr>
</tbody></table>

作業系統必須支援程式設計師對這些物件的操作，程式可以透過系統呼叫 (像是 open(), read(), write(), lseek(), stat(), opendir(), readdir(), 等函數) 操控檔案系統。而檔案系統的主要任務也就是支持這些操作，讓應用程式得以不需要處理區塊的問題，而是處理『路徑』、『目錄』與『檔案』。

早期的 Linux 直接將檔案系統 (像是 Ext2檔案系統) 放入核心當中，但是在這樣的方式下，Linux Kernel 與檔案系統綁得太緊密了，會造成無法抽換檔案系統的困境，因此在後來的 Linux 增加了一個稱為 VFS 的虛擬檔案系統，以便讓檔案系統可以很容易的抽換。

接著，我們先看看 Linux 真實檔案系統的一個經典範例 - Ext2檔案系統，然後再來介紹這個虛擬檔案系統 VFS 的運作方式。

##Ext2 檔案系統

Ext2 檔案系統的結構如圖 2 所示，一個 Ext2 檔案系統可被分為數個區塊群 (Block Group)，每個區塊群都由超級區塊 (superblock) 所引導，超級區塊會透過索引結構 (inode) 連結到資料區塊，Ext2 檔案系統的任務就是有效的組織 inode，讓尋找與更新的動作得以快速的進行。

![](images/LinuxExt2.jpg)

圖 2. Ext2 檔案系統的儲存結構

索引節點inode 是從 UNIX 早期就使用的檔案系統組織結構，Ext2 也使用inode組織區塊，以形成樹狀的區塊結構。inode 是一種相當奇特的樹狀結構，除了記錄目錄的相關資訊之外，其直接連結會連接到目標的資料區塊，而間接連結則可連結到其他的 inode，包含雙層與三層的連結區域，因此可以很快速的擴展開來。有利於減少磁碟的讀取次數。圖 3 顯示了 inode 的索引方式，而圖 4 則顯示了 inode 索引節點的內部結構。

![](images/LinuxInodeIndex.jpg)

圖 3 使用inode對裝置中的區塊進行索引

![](images/LinuxInode.jpg)

圖 4 索引節點inode的內部結構

Ext2 利用 inode 建構出磁碟的目錄結構與索引系統，於是可以從超級區塊連接到裝置的資料區塊。在 Linux 原始碼中，Ext2檔案系統的函數都撰寫成 ext2_xxx() 的格式，因此很容易辨認。Ext2的資料結構定義在 include/linux/ext2_fs.h 標頭檔中，而其函數則定義在include/linux/ext2.h 可以在 Linux 原始碼中的 fs/ext2/ 資料夾中找到。

Ext2 的主要物件有區塊群 (block group), 超級區塊 (super_block), 索引結構 (inode) 與目錄項 (dir_entry) 等。您可以在 Linux 的原始碼中找到這些資料結構的定義，表格 4 顯示了這些物件與其在 Linux 原始碼中的位置，有興趣的讀者可以自行參考。

表格 4 Ext2 檔案系統中的重要物件

<table class="wiki-content-table">
<tbody><tr>
<th>物件</th>
<th>資料結構</th>
<th>宣告檔案 (Linux 2.6.29.4原始碼)</th>
</tr>
<tr>
<td>區塊群</td>
<td>struct ext2_group_desc {…}</td>
<td>/include/linux/ext2_fs.h</td>
</tr>
<tr>
<td>超級區塊</td>
<td>struct ext2_super_block {…}<br>
struct ext2_sb_info {…}</td>
<td>/include/linux/ext2_fs.h<br>
/include/linux/ext2_fs_sb.h</td>
</tr>
<tr>
<td>索引結構</td>
<td>struct ext2_inode {…}<br>
struct ext2_inode_info {…}</td>
<td>/include/linux/ext2_fs.h<br>
/fs/ext2/ext2.h</td>
</tr>
<tr>
<td>目錄項</td>
<td>struct ext2_dir_entry {…}<br>
struct ext2_dir_entry2 {…}</td>
<td>/include/linux/ext2_fs.h<br>
/include/linux/ext2_fs.h</td>
</tr>
</tbody></table>

##Linux 當中的 VFS 虛擬檔案系統

在傳統的UNIX系統中，檔案系統是固定的實體檔案系統。但在Linux當中，為了統合各個檔案系統，因而加上了一層虛擬檔案系統 (Virtual File System: VFS) ，VFS 是一組檔案操作的抽象介面，我們可以將任何的真實檔案系統，透過 VFS 掛載到 Linux 中。

由於 VFS只是個虛擬的檔案系統，並不負責組織磁碟結構，因此，所有的組織動作都交由真實的檔案系統處理。VFS 所負責的操作都是在記憶體當中的部分，包含目錄結構的快取等功能，這樣就能增快存取的速度。

VFS 是一個軟體層的介面，負責處理檔案的處理請求，像是讀取、寫入、複製、刪除等。由於各種檔案系統的格式並不相同，所以 VFS 並不直接處理檔案格式，而是規定這些處理請求的介面及操作語意，然後交由真實的檔案系統 (像是 EXT2) 去處理。圖 5 顯示了 Linux 核心、VFS 與真實檔案系統之間的架構關係。


![](images/LinuxVFS.jpg)

圖 5 Linux 的檔案系統結構

真實檔案系統是在檔案結構的組織下，利用區塊裝置驅動模組所建構出來的。在Linux 作業系統中，允許安裝各式各樣的檔案系統，像是 BSD、FAT32、NTFS、EXT2、EXT3、JFS、JFS2、ReiserFS 等，這些檔案系統透過統一的虛擬檔案系統介面 (Virtual File System : VFS)，就能被掛載到 Linux 作業系統中。

甚至，有些非實體檔案系統，也可以透過 VFS 掛載進來，像是根檔案系統 (rootfs), 記憶體對應檔案系統 (proc), 網路檔案系統 (sockfs) 等，都可以透過 VFS掛載進來，由 VFS 進行管理與操控。

為了要將檔案系統掛載到 Linux 當中，Linux 仍然利用『註冊與反向呼叫機制』 (Register and Callback) 作為 VFS 的主要設計方式。實體檔案系統 (像是Ext2) 會向 VFS 進行註冊，以便將反向呼叫用的函數指標傳遞給 Linux 系統，接著 Linux 系統就可以在是當的時機，透過這些函數指標呼叫實體檔案系統的函數。

為了組織 VFS 的這些註冊函數，Linux 採用了一個類似物件導向的方式，將函數以物件的方式組織起來。由於VFS 的設計深受UNIX與 Ext2 的影響，因此在使用的術語及結構安排上都使用由UNIX/Ext2 所遺留下來的檔案系統之術語。

受到 UNIX/Ext2 的影’響，VFS系統也是由超級區塊與 inode 物件所構成的，另外還有目錄項 (dentry) 與檔案 (file) 等物件，分別對應到目錄中的子項與檔案等物件 。

超級區塊 (Superblock) 在邏輯上對應到一個檔案系統分割區 (Partition)，而索引節點 (inode) 則對應到目錄結構 (directory structure)，目錄中包含很多子項目，可能是檔案或資料夾，這些子項目稱為目錄項 (dentry)，這些項目其中有些代表檔案 (file)，透過 inode 與 dentry 就可以取得磁碟中的資料區塊 (data block)，這些區塊就是檔案的內容。

於是，VFS 可以再 inode 中新增、修改、刪除、查詢 dentry (mkdir, rmdir, …)，然後取得或設定 dentry 的屬性 (chmod, chowner, …)，或者利用inode找到檔案 (open)，然後再對檔案進行操作 (read, write, …)。表格 5 顯示了 VFS 系統中的重要函數原型，讀者應可看出這些物件間的關係。

表格 5. VFS 中的代表性函數原型

<table class="wiki-content-table">
<tbody><tr>
<th>操作</th>
<th>函數原型</th>
</tr>
<tr>
<td>開檔</td>
<td>int (*open) (struct inode *, struct file *);</td>
</tr>
<tr>
<td>建立資料夾</td>
<td>int (*mkdir) (struct inode *,struct dentry *,int);</td>
</tr>
<tr>
<td>設定屬性</td>
<td>int (*setattr) (struct dentry *, struct iattr *);</td>
</tr>
<tr>
<td>追蹤連結</td>
<td>void * (*follow_link) (struct dentry *, struct nameidata *);</td>
</tr>
</tbody></table>

所有掛載後的分割都會被放在 vfsmntlist 這個 Linux Kernel 的串列變數中，串列中的每一個元素為 vfsmount結構，該結構代表一個分割，其中的 mnt_sb 欄即是該分割的超級區塊 (super_block)，超級區塊中有個 s_inodes 欄位指向 inode 節點串列，inode 也透過 i_sb 欄位指回超級區塊。

當檔案系統想要掛載到 Linux 當中時，會將一個稱為 read_super 的函數，傳遞給 VFS，於是 VFS 就可以透過下列的結構，將檔案系統串接起來，形成檔案系統串列。在需要使用該檔案系統時，再透過read_super 將 super_block載入記憶體中 ，

super_block, inode, file, dentry 結構中都有個 op 欄位，該欄位儲存了一堆反向呼叫函數，可以用來呼叫真實檔案系統 (像是 Ext2) 中的操作函數，以便取得硬碟中的資料，或者將資料寫回硬碟當中。範例 1 顯示了 Linux 原始碼當中這些物件的結構與操作函數，詳細閱讀有助於進一步理解 VFS 檔案系統的設計方式。

範例 1. 虛擬檔案系統VFS的Linux 原始碼節錄

```c
  檔案：Linux 2.6.29.4 原始檔 include/linux/fs.h
649   struct inode {                                                       索引節點inode結構       
650    struct hlist_node i_hash;                                           雜湊表格
651    struct list_head i_list;                                            inode 串列
652    struct list_head i_sb_list;                                         超級區塊串列
653    struct list_head i_dentry;                                          目錄項串列
      …                                                                    …
675    const struct inode_operations *i_op;                                inode 的操作函數
676    const struct file_operations *i_fop;                                檔案的操作函數
677    struct super_block *i_sb;                                           指向超級區塊
…     …                                                                    
714   };                                                                   
…     …                                                                    
839   struct file {                                                        檔案物件的結構
…     …                                                                    
847       struct path        f_path;                                            路徑
848   #define f_dentry    f_path.dentry                                        
849   #define f_vfsmnt    f_path.mnt                                         
850       const struct file_operations    *f_op;                             檔案的操作函數
…     …                                                                    
875   };                                                                   
…     …                                                                    
1132  struct super_block {                                                 超級區塊的結構
1133   struct list_head s_list;                                            區塊串列
1134   dev_t s_dev;                                                        設備代號
1135   unsigned long s_blocksize;                                          區塊大小
…     …                                                                    …
1139   struct file_system_type *s_type;                                    真實檔案系統
1140   const struct super_operations *s_op;                                超級區塊操作函數
…     …                                                                    …
1157   struct list_head s_inodes;                                          整個inode串列
1158   struct list_head s_dirty;                                           修改過的inode串列
…     …                                                                    
1206  };                                                                   
…     …                                                                    
1310  struct file_operations {                                             檔案的操作函數
1311   struct module *owner;                                                 檔案擁有者
…     …包含一群檔案操作的函數指標，列舉如下…                               
…     llseek(), read(), write(), aio_read(), aio_write(),                  
…     readdir(), poll(), ioctl(), unlocked_ioctl(),                        
…     compat_ioctl(), mmap(), open(), flush(), release(),                  
…     fsync(), aio_fsync(), lock(), sendpage(),                            
…     get_unmapped_area(), check_flags(), flock(),                         
…     splice_write(), splice_read(), setlease()                            
1337  };                                                                   
1338                                                                       
1339  struct inode_operations {                                            inode 的操作函數
…     …包含一群inode操作的函數指標，列舉如下…                              …
…     create(), lookup(), link(), unlink(), symlink(), mkdir(),            
…     rmdir(), mknod(), rename(), readlink(), follow_link(),               
…     put_link(), truncate(), permission(), setattr(),                     
…     setxattr(), getxattr(), listxattr(), removexattr(),                  
…     trunctate_range(), fallocate(), filemap()                            
1366  };                                                                   

1382  struct super_operations {                                            超級區塊操作函數
…     …包含一群超級區塊操作的函數指標，列舉如下…                           
…     alloc_inode(), destroy_inode(), dirty_inode(),                       
…      write_inode(), drop_inode(), delete_inode(),                        
…     put_super(), write_super(), sunc_fs(), freeze_fs(),                  
…     unfreeze_fs(), statfs(), remount_fs(), clear_inode(),                
…     unmount_begin(), show_options(), show_stats(),                       
…     quota_read(),quota_write(),bdev_try_to_free_page()                   
1407  };                                                                   
…     ….                                                                   
1565  struct file_system_type {                                            檔案系統物件
1566    const char *name;                                                  檔案系統名稱
1567    int fs_flags;                                                      旗標
1568    int (*get_sb) (struct file_system_type *, int,                     超級區塊取得函數
1569       const char *, void *, struct vfsmount *);                       
1570    void (*kill_sb) (struct super_block *);                            超級區塊刪除函數
1571    struct module *owner;                                              
1572    struct file_system_type * next;                                    下一個檔案系統
1573    struct list_head fs_supers;                                        超級區塊串列
…     …                                                                    
1582  };
```
以下顯示了 dentry 的操作函數的相關檔案結構

```c
…    檔案：Linux 2.6.29.4 原始檔 include/linux/fs.h …
89   struct dentry {                                            目錄項dentry物件      
…    …                                                          
94       struct inode *d_inode;                                  指向 inode
…    …                                                          
100      struct hlist_node d_hash;                               雜湊表
101      struct dentry *d_parent;                                父目錄
102      struct qstr d_name;                                     目錄項名稱
103                                                             
104      struct list_head d_lru;        /* LRU list */              最近最少使用串列
…    …                                                          (快取的取代策略)
115      struct dentry_operations *d_op;                         dentry 的操作函數
116      struct super_block *d_sb;                               指向超級區塊
…    …                                                          
120  };                                                         
…    …                                                          
134  struct dentry_operations {                                 dentry的操作函數
…    …包含一群 dentry 操作的函數指標，列舉如下…                 
…    d_revalidate(), d_hash(), d_compare(), d_delete(),
…    d_release(), d_iput(), ddname()
142  };
```

## VFS 如何接上真實檔案系統

由於 VFS 的設計與 Ext2 相當類似，因此在實作對應上相當容易，像是在 ext2 檔案系統中，就直接將超級區塊相關的所有函數指標，全數塞入到 ext2_sops 這個型態為 super_operations 的變數中。然後在ext2_fill_super 這個函數中，將些函數塞入到 Linux VFS 超級區塊的 s_op 欄位中，就完成了連接的動作，於是 Linux 核心就可以透過超級區塊中的這些函數指標，操作 Ext2 檔案系統的超級區塊了。

範例 2. 將 Ext2 連接到 VFS 上的程式原始碼片段


```c
 檔案：Linux 2.6.29.4 原始檔 fs/ext2/super.c
…    …
300    static const struct super_operations ext2_sops = {
301     .alloc_inode = ext2_alloc_inode,
302     .destroy_inode = ext2_destroy_inode,
303     .write_inode = ext2_write_inode,
304     .delete_inode = ext2_delete_inode,
305     .put_super = ext2_put_super,
306     .write_super = ext2_write_super,
307     .statfs  = ext2_statfs,
308     .remount_fs = ext2_remount,
309     .clear_inode = ext2_clear_inode,
310     .show_options = ext2_show_options,
311    #ifdef CONFIG_QUOTA
312     .quota_read = ext2_quota_read,
313     .quota_write = ext2_quota_write,
314    #endif
315    };
…    …
739    static int ext2_fill_super(struct super_block *sb, void *data, int silent)
…    …
…     sb->s_op = &ext2_sops;
1050
```

於是，Linux 就可以利用 VFS 中的物件結構，對任何的檔案系統 (像是 Ext2, NTFS等) 進行操作。必須注意的是，掛載到 Linux VFS 上的檔案系統未必像 Ext2 這樣與 VFS 在設計理念上完全吻合，但是只要經過適當的封裝之後，仍然可以順利的掛載上去。像是 NTFS 就使用了 B+ Tree 的結構，並沒有使用 inode 結構，但是仍然可以被掛載到 VFS 中。

透過VFS中的物件，Linux 可以呼叫真實檔案系統 (像是 Ext2) 的 VFS 介面函數，以便完成檔案操作的功能。舉例而言，Linux 中的系統呼叫 sys_stat() 函數，其實作方法如範例 3 所示，但是為了簡短起見，該函數的資料宣告部分已被去除，程式也被簡化了。

範例 3. Linux 使用 VFS 操作檔案系統的範例

```c
sys_stat(path, buf) {                              取得檔案屬性
  dentry = namei(path);                            透過路徑取得目錄項
  if ( dentry == NULL ) return -ENOENT;            

  inode = dentry->d_inode;                         透過目錄項取得 inode
  rc =inode->i_op->i_permission(inode);            執行i_permission() 操作
  if  ( rc ) return -EPERM;                        取得操作權

  rc = inode->i_op->i_getattr(inode, buf);         執行 i_getattr() 
  dput(dentry);                                    取得目錄項屬性傳回
  return rc;                                       
}
```



