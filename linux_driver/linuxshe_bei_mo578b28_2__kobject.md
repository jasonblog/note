# Linux設備模型(2)_Kobject


##1. 前言

Kobject是Linux設備模型的基礎，也是設備模型中最難理解的一部分（可參考Documentation/kobject.txt的表述）。因此有必要先把它分析清楚。

##2. 基本概念

由“Linux設備模型(1)_基本概念”可知，Linux設備模型的核心是使用Bus、Class、Device、Driver四個核心數據結構，將大量的、不同功能的硬件設備（以及驅動該硬件設備的方法），以樹狀結構的形式，進行歸納、抽象，從而方便Kernel的統一管理。
而硬件設備的數量、種類是非常多的，這就決定了Kernel中將會有大量的有關設備模型的數據結構。這些數據結構一定有一些共同的功能，需要抽象出來統一實現，否則就會不可避免的產生冗餘代碼。這就是Kobject誕生的背景。
目前為止，Kobject主要提供如下功能：

- 通過parent指針，可以將所有Kobject以層次結構的形式組合起來。

- 使用一個引用計數（reference count），來記錄Kobject被引用的次數，並在引用次數變為0時把它釋放（這是Kobject誕生時的唯一功能）。

- 和sysfs虛擬文件系統配合，將每一個Kobject及其特性，以文件的形式，開放到用戶空間（有關sysfs，會在其它文章中專門描述，本文不會涉及太多內容）。

注1：在Linux中，Kobject幾乎不會單獨存在。它的主要功能，就是內嵌在一個大型的數據結構中，為這個數據結構提供一些底層的功能實現。 

注2：Linux driver開發者，很少會直接使用Kobject以及它提供的接口，而是使用構建在Kobject之上的設備模型接口。


## 3. 代碼解析

###3.1 在Linux Kernel source code中的位置

在Kernel源代碼中，Kobject由如下兩個文件實現：
- include/linux/kobject.h
- lib/kobject.c

其中kobject.h為Kobject的頭文件，包含所有的數據結構定義和接口聲明。kobject.c為核心功能的實現。

### 3.2 主要的數據結構

在描述數據結構之前，有必要說明一下Kobject, Kset和Ktype這三個概念。
Kobject是基本數據類型，每個Kobject都會在"/sys/“文件系統中以目錄的形式出現。
Ktype代表Kobject（`嚴格地講，是包含了Kobject的數據結構`）的屬性操作集合（由於通用性，多個Kobject可能共用同一個屬性操作集，因此把Ktype獨立出來了）。 
`注3：在設備模型中，ktype的命名和解釋，都非常抽象，理解起來非常困難，後面會詳細說明。`

Kset是一個特殊的Kobject（因此它也會在"/sys/“文件系統中以目錄的形式出現），它用來集合相似的Kobject（這些Kobject可以是相同屬性的，也可以不同屬性的）。

- 首先看一下Kobject的原型

```c
/* Kobject: include/linux/kobject.h line 60 */
struct kobject {
    const char* name;
    struct list_head    entry;
    struct kobject*      parent;
    struct kset*     kset;
    struct kobj_type*    ktype;
    struct sysfs_dirent* sd;
    struct kref     kref;
    unsigned int state_initialized: 1;
    unsigned int state_in_sysfs: 1;
    unsigned int state_add_uevent_sent: 1;
    unsigned int state_remove_uevent_sent: 1;
    unsigned int uevent_suppress: 1;
};
```

name，該Kobject的名稱，同時也是sysfs中的目錄名稱。由於Kobject添加到Kernel時，需要根據名字註冊到sysfs中，之後就不能再直接修改該字段。如果需要修改Kobject的名字，需要調用kobject_rename接口，該接口會主動處理sysfs的相關事宜。
entry，用於將Kobject加入到Kset中的list_head。

parent，指向parent kobject，以此形成層次結構（在sysfs就表現為目錄結構）。

kset，該kobject屬於的Kset。可以為NULL。如果存在，且沒有指定parent，則會把Kset作為parent（別忘了Kset是一個特殊的Kobject）。

ktype，該Kobject屬於的kobj_type。每個Kobject必須有一個ktype，或者Kernel會提示錯誤。

sd，該Kobject在sysfs中的表示。

kref，"struct kref”類型（在include/linux/kref.h中定義）的變量，為一個可用於原子操作的引用計數。

state_initialized，指示該Kobject是否已經初始化，以在Kobject的Init，Put，Add等操作時進行異常校驗。

state_in_sysfs，指示該Kobject是否已在sysfs中呈現，以便在自動註銷時從sysfs中移除。

state_add_uevent_sent/state_remove_uevent_sent，記錄是否已經向用戶空間發送ADD uevent，如果有，且沒有發送remove uevent，則在自動註銷時，補發REMOVE uevent，以便讓用戶空間正確處理。

uevent_suppress，如果該字段為1，則表示忽略所有上報的uevent事件。

`注4：Uevent提供了“用戶空間通知”的功能實現，通過該功能，當內核中有Kobject的增加、刪除、修改等動作時，會通知用戶空間。有關該功能的具體內容，會在其它文章詳細描述。`


- Kset的原型為


```c
/* include/linux/kobject.h, line 159 */
struct kset {
    struct list_head list;
    spinlock_t list_lock;
    struct kobject kobj;
    const struct kset_uevent_ops* uevent_ops;
};
```

list/list_lock，用於保存該kset下所有的kobject的鏈表。

kobj，該kset自己的kobject（kset是一個特殊的kobject，也會在sysfs中以目錄的形式體現）。

uevent_ops，該kset的uevent操作函數集。當任何Kobject需要上報uevent時，都要調用它所從屬的kset的uevent_ops，添加環境變量，或者過濾event（kset可以決定哪些event可以上報）。因此，如果一個kobject不屬於任何kset時，是不允許發送uevent的。

- Ktype的原型為

```c
/* include/linux/kobject.h, line 108 */
struct kobj_type {
    void (*release)(struct kobject* kobj);
    const struct sysfs_ops* sysfs_ops;
    struct attribute** default_attrs;
    const struct kobj_ns_type_operations* (*child_ns_type)(struct kobject* kobj);
    const void* (*namespace)(struct kobject* kobj);
};
```

release，通過該回調函數，可以將包含該種類型kobject的數據結構的內存空間釋放掉。

sysfs_ops，該種類型的Kobject的sysfs文件系統接口。

default_attrs，該種類型的Kobject的atrribute列表（所謂attribute，就是sysfs文件系統中的一個文件）。將會在Kobject添加到內核時，一併註冊到sysfs中。

child_ns_type/namespace，和文件系統（sysfs）的命名空間有關，這裡不再詳細說明。



##總結，Ktype以及整個Kobject機制的理解。 
Kobject的核心功能是：保持一個引用計數，當該計數減為0時，自動釋放（由本文所講的kobject模塊負責） Kobject所佔用的meomry空間。這就決定了Kobject必須是動態分配的（只有這樣才能動態釋放）。 

而Kobject大多數的使用場景，是內嵌在大型的數據結構中（如Kset、device_driver等），因此這些大型的數據結構，也必須是動態分配、動態釋放的。那麼釋放的時機是什麼呢？是內嵌的Kobject釋放時。但是Kobject的釋放是由Kobject模塊自動完成的（在引用計數為0時），那麼怎麼一併釋放包含自己的大型數據結構呢？ 

這時Ktype就派上用場了。我們知道，Ktype中的release回調函數負責釋放Kobject（甚至是包含Kobject的數據結構）的內存空間，那麼Ktype及其內部函數，是由誰實現呢？是由上層數據結構所在的模塊！因為只有它，才清楚Kobject嵌在哪個數據結構中，並通過Kobject指針以及自身的數據結構類型，找到需要釋放的上層數據結構的指針，然後釋放它。 

講到這裡，就清晰多了。所以，每一個內嵌Kobject的數據結構，例如kset、device、device_driver等等，都要實現一個Ktype，並定義其中的回調函數。同理，sysfs相關的操作也一樣，必須經過ktype的中轉，因為sysfs看到的是Kobject，而真正的文件操作的主體，是內嵌Kobject的上層數據結構！ 


順便提一下，Kobject是面向對象的思想在Linux kernel中的極致體現，但C語言的優勢卻不在這裡，所以Linux kernel需要用比較巧妙（也很囉嗦）的手段去實現，

---

##3.3 功能分析

###3.3.1 Kobject使用流程


Kobject大多數情況下（有一種例外，下面會講）會嵌在其它數據結構中使用，其使用流程如下：

- 定義一個struct kset類型的指針，並在初始化時為它分配空間，添加到內核中
- 根據實際情況，定義自己所需的數據結構原型，該數據結構中包含有Kobject
- 定義一個適合自己的ktype，並實現其中回調函數
- 在需要使用到包含Kobject的數據結構時，動態分配該數據結構，並分配Kobject空間，添加到內核中
- 每一次引用數據結構時，調用kobject_get接口增加引用計數；引用結束時，調用kobject_put接口，減少引用計數
- 當引用計數減少為0時，Kobject模塊調用ktype所提供的release接口，釋放上層數據結構以及Kobject的內存空間
 
上面有提過，有一種例外，Kobject不再嵌在其它數據結構中，可以單獨使用，這個例外就是：開發者只需要在sysfs中創建一個目錄，而不需要其它的kset、ktype的操作。這時可以直接調用kobject_create_and_add接口，分配一個kobject結構並把它添加到kernel中。

### 3.3.2 Kobject的分配和釋放
前面講過，Kobject必須動態分配，而不能靜態定義或者位於堆棧之上，它的分配方法有兩種。

####1. 通過kmalloc自行分配（一般是跟隨上層數據結構分配），並在初始化後添加到kernel。這種方法涉及如下接口：

```c
/* include/linux/kobject.h, line 85 */
extern void kobject_init(struct kobject* kobj, struct kobj_type* ktype);
extern __printf(3, 4) __must_check
int kobject_add(struct kobject* kobj, struct kobject* parent,
                const char* fmt, ...);
extern __printf(4, 5) __must_check
int kobject_init_and_add(struct kobject* kobj,
                         struct kobj_type* ktype, struct kobject* parent,
                         const char* fmt, ...);

```


kobject_init，初始化通過kmalloc等內存分配函數獲得的struct kobject指針。主要執行邏輯為：

- 確認kobj和ktype不為空
- 如果該指針已經初始化過（判斷kobj->state_initialized），打印錯誤提示及堆棧信息（但不是致命錯誤，所以還可以繼續）
- 初始化kobj內部的參數，包括引用計數、list、各種標誌等
- 根據輸入參數，將ktype指針賦予kobj->ktype

kobject_add，將初始化完成的kobject添加到kernel中，參數包括需要添加的kobject、該kobject的parent（用於形成層次結構，可以為空）、用於提供kobject name的格式化字符串。主要執行邏輯為：
- 確認kobj不為空，確認kobj已經初始化，否則錯誤退出
- 調用內部接口kobject_add_varg，完成添加操作

kobject_init_and_add，是上面兩個接口的組合，不再說明。
==========================內部接口======================================
kobject_add_varg，解析格式化字符串，將結果賦予kobj->name，之後調用kobject_add_internal接口，完成真正的添加操作。

kobject_add_internal，將kobject添加到kernel。主要執行邏輯為：

- 校驗kobj以及kobj->name的合法性，若不合法打印錯誤信息並退出
- 調用kobject_get增加該kobject的parent的引用計數（如果存在parent的話）
- 如果存在kset（即kobj->kset不為空），則調用kobj_kset_join接口加入kset。同時，如果該kobject沒有parent，卻存在kset，則將它的parent設為kset（kset是一個特殊的kobject），並增加kset的引用計數
- 通過create_dir接口，調用sysfs的相關接口，在sysfs下創建該kobject對應的目錄
- 如果創建失敗，執行後續的回滾操作，否則將kobj->state_in_sysfs置為1

kobj_kset_join，負責將kobj加入到對應kset的鏈表中。


這種方式分配的kobject，會在引用計數變為0時，由kobject_put調用其ktype的release接口，釋放內存空間，具體可參考後面有關kobject_put的講解。

####2. 使用kobject_create創建
Kobject模塊可以使用kobject_create自行分配空間，並內置了一個ktype（dynamic_kobj_ktype），用於在計數為0是釋放空間。代碼如下：


```c
/* include/linux/kobject.h, line 96 */
extern struct kobject* __must_check kobject_create(void);
extern struct kobject* __must_check kobject_create_and_add(const char* name,
        struct kobject* parent);

```

```c
/* lib/kobject.c, line 605 */
static void dynamic_kobj_release(struct kobject* kobj)
{
    pr_debug("kobject: (%p): %s\n", kobj, __func__);
    kfree(kobj);
}

static struct kobj_type dynamic_kobj_ktype = {
    .release    = dynamic_kobj_release,
    .sysfs_ops  = &kobj_sysfs_ops,
};

```

kobject_create，該接口為kobj分配內存空間，並以dynamic_kobj_ktype為參數，調用kobject_init接口，完成後續的初始化操作。

kobject_create_and_add，是kobject_create和kobject_add的組合，不再說明。

dynamic_kobj_release，直接調用kfree釋放kobj的空間。

### 3.3.3 Kobject引用計數的修改

通過kobject_get和kobject_put可以修改kobject的引用計數，並在計數為0時，調用ktype的release接口，釋放佔用空間。

```c
/* include/linux/kobject.h, line 103 */
extern struct kobject *kobject_get(struct kobject *kobj);
extern void kobject_put(struct kobject *kobj);
```

kobject_get，調用kref_get，增加引用計數。
kobject_put，以內部接口kobject_release為參數，調用kref_put。kref模塊會在引用計數為零時，調用kobject_release。
==========================內部接口======================================
kobject_release，通過kref結構，獲取kobject指針，並調用kobject_cleanup接口繼續。
kobject_cleanup，負責釋放kobject佔用的空間，主要執行邏輯如下：

- 檢查該kobject是否有ktype，如果沒有，打印警告信息
- 如果該kobject向用戶空間發送了ADD uevent但沒有發送REMOVE uevent，補發REMOVE uevent
- 如果該kobject有在sysfs文件系統註冊，調用kobject_del接口，刪除它在sysfs中的註冊
- 調用該kobject的ktype的release接口，釋放內存空間
- 釋放該kobject的name所佔用的內存空間

###3.3.4 Kset的初始化、註冊
Kset是一個特殊的kobject，因此其初始化、註冊等操作也會調用kobject的相關接口，除此之外，會有它特有的部分。另外，和Kobject一樣，kset的內存分配，可以由上層軟件通過kmalloc自行分配，也可以由Kobject模塊負責分配，具體如下。

```c
/* include/linux/kobject.h, line 166 */
extern void kset_init(struct kset* kset);
extern int __must_check kset_register(struct kset* kset);
extern void kset_unregister(struct kset* kset);
extern struct kset* __must_check kset_create_and_add(const char* name,
        const struct kset_uevent_ops* u,
        struct kobject* parent_kobj);

```

kset_init，該接口用於初始化已分配的kset，主要包括調用kobject_init_internal初始化其kobject，然後初始化kset的鏈表。需要注意的時，如果使用此接口，上層軟件必須提供該kset中的kobject的ktype。

kset_register，先調用kset_init，然後調用kobject_add_internal將其kobject添加到kernel。


kset_unregister，直接調用kobject_put釋放其kobject。當其kobject的引用計數為0時，即調用ktype的release接口釋放kset佔用的空間。

kset_create_and_add，會調用內部接口kset_create動態創建一個kset，並調用kset_register將其註冊到kernel。

==========================內部接口======================================
kset_create，該接口使用kzalloc分配一個kset空間，並定義一個kset_ktype類型的ktype，用於釋放所有由它分配的kset空間。
