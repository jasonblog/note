# Page Table size太大之解決


page的大小決定了映射的粒度(granularity):
 
 如果我們把page切1MB，那64MB的記憶體就需要64MB/1MB的PTE，每個PTE是32bits(4bytes)，64x4=256bytes頁表項空間。4G就是4096x6=16384=16K
 
 
 ## Page Table size太大之解決
 
 ###[法一]Multilevel paging(多層的分頁)

將page table再予以分頁
###例1：single level paging
假設
- page size：1024 Bytes
- Logical Address：32 Bits
- page table entry：4 bytes
- ∵d佔10 bits
- ∴p佔32-10=22 bits
⇒page table size = 4 byte * 222個entries = 16 MB ⇒ Too Large!!

####例2：Two-Level paging
Two-Level paging

4K pages = 12-bit offset; 1 master page = 4K/4 = 1K entries = 10bits Secondary page table size = 32 - 12 - 10 = 10 bits = 1K entries * 4 = 4K


####例3：3-Level paging

###[法二]Hashing Page Table(雜湊)

Def：

- 將Logical Address中的p(page#)經由Hashing function計算取得Hashing Table(page table)的Bucket Address

- 而在每個Bucket中，皆以linked list串連具相同Hashing address的page number及對應的frame number

Node structure為：
- 因此，我們會去linked list中search符合的page number之節點。取得f，然後與d相加得出physical address
圖示：
###[法三]Invert Page Table(反轉分頁表)

- Def：以physical memory為對象，建立一個表格(若有m個Frames，Table Entry有m格)
每個entry記錄此頁框被哪個process的哪個page所佔用
以<Process id, Page No>
    - Logical Address→Physical Address圖示
    
    - 優點：大幅降低page table size
    - 缺點：
        - searching Invert page table非常耗時
        - 無法支援Memory sharing



### example
邏輯位址有 32 bits 、page size 4kB、每一個 table entry 佔 4 bytes
求 page table size?

[Page][Offset] = 32 bits
Offset 的長度，要能定址一個 page 大小( 4KB = 2^12 byte) ，
所以 Offset 長度為 12 bits 邏輯位址有 32bits 扣去 Offset 的 12 bits ，
還剩 20bits，這表示一個 process 最多用 2^20 pages ，
所以系統中 page table 最多有 2^20 個 entry

page table size 最大會是： 2^20 * 4bytes(每個 entry 大小) = 4MB



####In `singlelevel pagetable` you need the whole table to access even a small amount of data(less memory references). i.e 2^20 pages each PTE occupying 4bytes as you assumed.


Space required to access any data is `2^20 * 4bytes = 4MB`

####Paging pages is `multi-level paging`.In multilevel paging it is more specific, you can with the help of multi-level organization decide which specific page among the 2^20 pages your data exists, and select it . So here you need only that specific page to be in the memory while you run the process.

In the `2 level case` that you discussed you need 1st level pagetable and then 1 of the 2^10 pagetables in second level. 

So, 
1st level size = 2^10 * 4bytes = `4KB`<br> 
2nd level we need only 1 among the 2^10 pagetables = so size is 2^10 * 4bytes = `4KB`<br>

Total size required is now : `4KB + 4KB = 8KB`.

Final comparision is `4MB vs 8KB` .