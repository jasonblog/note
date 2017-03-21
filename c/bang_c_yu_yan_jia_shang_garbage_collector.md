# 幫 C 語言加上 Garbage Collector


前幾天讀 Dragon Book 的時候突發奇想，何不自己實作 Conservative Garbage Collector？

一般來說 Tracing Garbage Collector 分成以下三個步驟：
先從 call stack 或者 global variables 的 references (或者 pointers) 蒐集 object 的位址作為 root set，並標記 root set 當中的 object。
檢視 root set 當中的 object。如果 object 也包含 reference，而且這個 reference 指向的 object' 沒有被標記過，就標記並檢視 object'。(不斷遞迴直到沒有新的 object 被標記)
清除並回收沒有被標記的 object。
然而為了有效率地計算 root set 與走訪被標記的 objects，通常我們會需要編譯器加入一些額外的資訊。而且程式語言必需是 type-safe 的！著名的例子如 Java 或者是 Haskell、Common Lisp 等等。而 C/C++ 就不適合當 Garbage Collector 的目標語言，因為 C/C++ 支援 Pointer Arithmetic、Union Type 等語言構件，使得我們幾乎不可能在 C/C++ 之上實作 (Accurate) Garbage Collector。

不過 Conservative Garbage Collector 的做法不太一樣。Conservative Garbage Collector 的想法是：我不去分辨誰是 Pointer Type 誰是 Integer Type，只要「看起來」像是 Pointer 的 bit pattern，我就把他當成 Pointer。這有一個好處：我們不需要編譯器的幫忙也可以寫 Garbage Collector，所以可以和其他的程式碼混用；然而也有一個壞處：有時候 Conservative Garbage Collector 過於保守，以至於無法回收一些可以回收的記憶體。以下我就以一個小程式展現 Conservative Garbage Collector 的基本概念。

首先我們的第一個問題是如何找到 root set？我們可以取 main 函式 argc 的位址，另外在 gc_cleanup_ 函式隨意傳入一個整數，再取該整數的位址。所有還可能會用到的 pointer 就會在這二個整數之間：


```c
void *stack_top;

void gc_init(int *argc_ptr) {
  stack_top = argc_ptr;
}

void gc_cleanup_(int stack_indicator) {
  void *stack_bottom = &stack_indicator; 
  /* Stack 的有效範圍就介於 [stack_bottom, stack_top] 之間 */
}

int main(int argc, char **argv) {
  gc_init(&argc);
}
```

當然我們還必需考慮 alignment 的問題，所以要把 stack_top 與 stack_bottom 對齊到 sizeof(void *) 的某倍。這可以用以下二個巨集來達成：

```c
#define FLOOR_ALIGN_TO_WORD(X) \
    (((uintptr_t)(X)) / sizeof(void *) * sizeof(void *))

#define CEIL_ALIGN_TO_WORD(X) \
    (((uintptr_t)(X) + sizeof(void *) - 1) / \
        sizeof(void *) * sizeof(void *))
```

接著我們要準備一些空間來當我們的 Heap，所以我們在 gc_init 之中加入一些程式碼：

```c
static char *heap_begin = NULL;
static char *heap_free = NULL;
static char *heap_end = NULL;

/* Initialize the internal data structure of the
   garbage collector */
void gc_init(int *argc_ptr) {
    /* We assume that argc is the variable in the
       stack having the highest address. */
    stack_top = (void **)CEIL_ALIGN_TO_WORD(argc_ptr);

    /* Allocate the heap using mmap. */
    heap_begin = (char *)mmap(0, GC_HEAP_SIZE,
                              PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS,
                              -1, 0);

    if (!heap_begin || heap_begin == MAP_FAILED) {
        fprintf(stderr,
                "Unable to allocate the heap (size=%lu).\n",
                (unsigned long)GC_HEAP_SIZE);
        exit(EXIT_FAILURE);
    }

    heap_free = heap_begin;
    heap_end = heap_begin + GC_HEAP_SIZE;
}
```

接著我們需要一些資料結構用以管理我們分配出去的記憶體。做為一個簡單的範例，我只用了一個陣列依序記錄分配出去的記憶體，我的陣列會從 heap_end 開始往 heap_begin 的方向生長。

當然這是一個很沒有效率的作法，比較有效率的方法是使用類似 buddy system 之類的資料結構來管理 heap。

```c
enum {
    ALLOC_STATUS_UNKNOWN,
    ALLOC_STATUS_TOUCHED,
    ALLOC_STATUS_REFERRED,
    ALLOC_STATUS_DEALLOCATED,
};

typedef struct alloc_record_ {
    char *addr;
    size_t size;
    unsigned int status;
} alloc_record;
```

為了操作 alloc_record 資料結構，我寫了三個函式，其功能分述如下（程式碼就不再贅述）：

```c
static alloc_record *find_alloc_record(char *addr) {
  /* 給定一個位址，找出它是屬於哪一次 allocation */
}

static void insert_alloc_record(char *addr, size_t size) {
  /* 在 alloc_record 的尾端加上一筆記錄 */
}

static void *allocate_deallocated_block(size_t size) {
  /* 重新把 deallocated 過的位址配置給不同的人 */
}
```

接下來是 Malloc 的程式碼。我們先嘗試直接從 heap_free 配置記憶體，如果記憶體不夠，再嘗試使用別人 deallocate 的記憶體。如果以上二者都沒有用，我們就必需進行 Garbage Collection。(備註：gc_cleanup(); 即 gc_cleanup_(0);)

```c
/* Allocate a block with given bytes. */
void *gc_malloc(size_t size) {
    /* Make sure that all of returned address is aligned */
    size = CEIL_ALIGN_TO_WORD(size);

    /* Try to allocate if free space available */
    if (is_free_space_avail(size)) {
        char *result = heap_free;
        heap_free += size;
        insert_alloc_record(result, size);
        return result;
    }

    /* Try to allocate from deallocated address. */
    void *result = allocate_deallocated_block(size);
    if (result) {
        return result;
    }

    /* Try to collect garbage and retry. */
    while (gc_cleanup()) {
        void *result = allocate_deallocated_block(size);
        if (result) {
            return result;
        }
    }

    return NULL;
}
```

當然有 Malloc 就要有 Free，不過值得注意的是：為了提高 Conservative Garbage Collector 的回收率，減少「應該 deallocate 而沒有 deallocate」的情況，我們應該要抹除整個 object 與傳入的 Pointer：

```c
/* Deallocate and wipe the allocated block. */
static void deallocate(alloc_record *record) {
    /* Mark this block as deallocated */
    record->status = ALLOC_STATUS_DEALLOCATED;

    /* Wipe the memory block for cleaning the possible
       pointer address */
    memset(record->addr, '\0', record->size);
}

/* Deallocate the given address and wipe the pointer. */
void gc_free_(void **addr_ptr) {
    alloc_record *record = find_alloc_record(*addr_ptr);
    if (!record) {
        fprintf(stderr, "Memory corrupted\n");
        return;
    }

    deallocate(record);
    *addr_ptr = NULL;
}
```

最後就是最關鍵的 Garbage Collection 的程式碼 gc_cleanup：

```c
/* Garbage collection and deallocate unused memory. */
size_t gc_cleanup_(int stack_indicator) {
    alloc_record *records =
        (alloc_record *)heap_end - alloc_record_count;

    size_t i, j;

    /* Mark alloc record as UNKNOWN */
    for (i = 0; i < alloc_record_count; ++i) {
        if (records[i].status != ALLOC_STATUS_DEALLOCATED) {
            records[i].status = ALLOC_STATUS_UNKNOWN;
        }
    }

    /* Scan the stack */
    void **stack_bottom =
        (void **)CEIL_ALIGN_TO_WORD(&stack_indicator);
    assert(stack_top != NULL);
    assert(stack_bottom <= stack_top);
    scan_and_touch(stack_bottom, (void **)stack_top);

    /* Scan the heap */
    scan_touched_objects();

    /* Deallocate and reset the status */
    static unsigned int gc_count = 0;
    static char const sep[] =
    "-------------------------------------------------------";

    fprintf(stderr, "%s\n", sep);
    fprintf(stderr,
            "GARBAGE COLLECTION ROUND #%u\n", ++gc_count);

    size_t deallocated_count = 0;
    for (j = alloc_record_count, i = j - 1; j > 0; --i, --j) {
        if (records[i].status == ALLOC_STATUS_UNKNOWN) {
            fprintf(stderr,
                    "  Reclaim [addr: %p, size: %lu]\n",
                    records[i].addr,
                    (unsigned long)records[i].size);

            deallocate(&records[i]);
            ++deallocated_count;
        }
    }

    fprintf(stderr, "%s\n\n", sep);

    return deallocated_count;
}

/* Scan for address between [begin, end) */
static void scan_and_touch(void *begin_, void *end_) {
    void **begin = (void **)FLOOR_ALIGN_TO_WORD(begin_);
    void **end = (void **)FLOOR_ALIGN_TO_WORD(end_);

    for (; begin < end; ++begin) {
        char *addr = (char *)*begin;

        if (addr >= heap_begin && addr < heap_free) {
            alloc_record *record = find_alloc_record(addr);

            if (record && record->status ==
                    ALLOC_STATUS_UNKNOWN) {
                record->status = ALLOC_STATUS_TOUCHED;
            }
        }
    }
}

/* Scan the touched objects */
static void scan_touched_objects() {
    alloc_record *records =
        (alloc_record *)heap_end - alloc_record_count;

    size_t count, i;

    do {
        count = 0;
        for (i = 0; i < alloc_record_count; ++i) {
            if (records[i].status != ALLOC_STATUS_TOUCHED) {
                /* Not in touched state, skip it. */
                continue;
            }

            /* Mark as referred. */
            records[i].status = ALLOC_STATUS_REFERRED;

            /* Scan this object */
            scan_and_touch(
                (void **)(records[i].addr),
                (void **)(records[i].addr + records[i].size));

            count++;
        }
    } while (count > 0);
}
```

以上程式碼就是一個具體而微的 Conservative Garbage Collector，完整的程式碼與測試程式可以從這裡下載：conservativegc.h , conservativegc.c , test_tree.c , test_many.c 。當然這個 Conservative Garbage Collector 還少了很多東西：包括計算 static storage 的 root set（在 Linux 之下可以檢查 etext 與 end 二者之間的值），還有更有效率的 allocation policy 等等。

備註：如果你真的有在 C/C++ 使用 Garbage Collector 的需求可以參考 libgc，一個由 Boehm、Demers、Weiser 等前輩撰寫 (他們是提出 Conservative Garbage Collector 的重要前輩)，並被移植到多個平台的 Conservative Garbage Collector 函式庫。