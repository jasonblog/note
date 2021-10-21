## Day 9：暖身運動 - 媽！我在核心裡面了！第一個核心模組

雖然說前面的實驗已經知道 I2C 的函式庫多半是用 `i2c-dev` 這個字元驅動程式去控制，不過沒有弄一次真的 i2c driver 感覺有點遺憾。所以這邊就弄一個非常簡單的 i2c driver。這個 driver 目前的作用只有一個，就是佔用 i2c 的某個 slave 的位址，然後 `probe` 的時候會跟你說他 `probe` 到東西了。

不過在看 I2C Device 之前，先來做一點跟核心模組有關的暖身。這方面可以去看 [Bootlin](https://bootlin.com/doc/training/linux-kernel/) 提供的詳細教學。非常值得參考！

## Step 0：安裝標頭檔



因為要編譯核心模組必須使用核心的 header。如果是用自己編譯的核心，在這之前先進去那個核心原始碼的最上層，然後把新的 *header* 安裝到現在的 Raspberry Pi 中：

```shell
$ sudo make headers_install INSTALL_HDR_PATH=/usr
```

或者如果是預設的 Raspbery Pi OS，可以直接安裝現成的套件：

```shell
$ sudo apt install raspberrypi-kernel-headers
```

## Step 1：Linux Kernel Module 簡介

*Linux kernel module* 就是一個可以插入核心執行的一個小程式片段。使用時機比如說不同裝置需要不同的驅動程式，這時就可以依照目前的硬體配置動態地載入對應的模組。

linux kernel module 有時候會直接被稱為「裝置驅動程式」，不過我個人認為這個稱呼很容易跟 *linux device model* 中的 `device_driver` 這個概念搞混。關於 *linux device model* 的資訊，可以去文件中的 [*Driver Model*](https://www.kernel.org/doc/html/latest/driver-api/driver-model/index.html) 中的 [*Linux Kernel Device Model*](https://www.kernel.org/doc/html/latest/driver-api/driver-model/overview.html) 一節找到相關內容。

簡單地來說，這是 Linux 看待硬體與驅動程式的模型。高階的觀點粗略地看，有 3 大類主要的物件：*bus*, *device*, 與 *driver*：

1. 硬體用 *device* 稱之。
2. *device* 與 *bus* 連接。
3. 同時，*bus* 上面會有很多的 *driver*
4. 當有新的 *driver* 或 *device* 加入到 *bus* 上時，*bus* 必需幫 *driver* 跟 *device* 進行配對。而提供這個配對的實作，就是 *bus* 的驅動程式需要去實作的東西 (通常這個東西稱作 `match`)。
5. 配對成功之後，配對到的 *driver* 要幫忙進行初始化 (比如說安排 buffer 接收從 *device* 來的資訊)。這個初始化的動作稱為 `probe`，是實作 *driver* 時要提供的。

那要怎麼說一個東西「是個 *device*」「是個 *driver*」「是個 *bus*」呢？Linux 中用了類似物件導向的「繼承」概念：如果某個結構體裡面包含 `device_driver` 這個東西，就類似這個結構「繼承」了 `device_driver`; 而若裡面有 `device` 這個結構體，就繼承了 `device` 這個結構; 而如果具備 `bus_type` 這個結構體，就繼承了 `bus_type`。

舉例來說，`struct i2c_driver` 上面就是其中一個例子，裡面有一個 `struct device_driver`：

```c
struct i2c_driver {
	unsigned int class;
	int (*probe)(struct i2c_client *client, const struct i2c_device_id *id);
	int (*remove)(struct i2c_client *client);
	int (*probe_new)(struct i2c_client *client);
	void (*shutdown)(struct i2c_client *client);
	void (*alert)(struct i2c_client *client, enum i2c_alert_protocol protocol,
		      unsigned int data);
	int (*command)(struct i2c_client *client, unsigned int cmd, void *arg);
    struct device_driver driver;
	const struct i2c_device_id *id_table;
	int (*detect)(struct i2c_client *client, struct i2c_board_info *info);
	const unsigned short *address_list;
	struct list_head clients;
	bool disable_i2c_core_irq_mapping;
};
```

又比如說 `i2c_client` 之中，就有一個 `struct device dev`：

```c
struct i2c_client {
	unsigned short flags;		/* div., see below		*/
#define I2C_CLIENT_PEC		0x04	/* Use Packet Error Checking */
#define I2C_CLIENT_TEN		0x10	/* we have a ten bit chip address */
					/* Must equal I2C_M_TEN below */
#define I2C_CLIENT_SLAVE	0x20	/* we are the slave */
#define I2C_CLIENT_HOST_NOTIFY	0x40	/* We want to use I2C host notify */
#define I2C_CLIENT_WAKE		0x80	/* for board_info; true iff can wake */
#define I2C_CLIENT_SCCB		0x9000	/* Use Omnivision SCCB protocol */
					/* Must match I2C_M_STOP|IGNORE_NAK */

	unsigned short addr;		/* chip address - NOTE: 7bit	*/
					/* addresses are stored in the	*/
					/* _LOWER_ 7 bits		*/
	char name[I2C_NAME_SIZE];
	struct i2c_adapter *adapter;	/* the adapter we sit on	*/
	struct device dev;		/* the device structure		*/
	int init_irq;			/* irq set at initialization	*/
	int irq;			/* irq issued by device		*/
	struct list_head detected;
#if IS_ENABLED(CONFIG_I2C_SLAVE)
	i2c_slave_cb_t slave_cb;	/* callback for slave mode	*/
#endif
};
```

而這些 `device`、`device_driver` 與 `bus_type` 之間的形成的關係，就直接變成 `sysfs` 中看到的那些階層。具體的實作細節，可以參考 *Greg KH* 在 2016 年的 *Kernel Recipe* 中的演講 [*The Linux Driver Model*](https://youtu.be/AdPxeGHIZ74) 得到更多細節。

> 對於物件導向的理論來說，這可能稱不上是物件導向：因為這是「*has a* 」的關係，而不是物件導向繼承中認定的的「*is a*」的關係。不過不管是 Bootlin 的訓練或是核心文件中都會看到他們稱呼這個叫做繼承。

> 為了方便，下面用斜體字寫的 *driver*、*device*，都是指在 Linux Device Model 中的 *driver* 與 *device*; 而用「模組」或「核心模組」則表示那些可以透過 `insmod` 載入核心的程式片段; 「裝置驅動程式」「驅動程式」這類用語會盡量避免。

### Step 1 - 1：模組基本要件

回到核心模組，他其實就只是一塊可以塞進核心的程式。要弄一個模組出來，只要提供 `init` 跟 `exit` 兩個函數就可以了，然後把他們註冊成這個這個模組插入時初始化與卸載時的清理函數就可以了：

```c
static int __init dummy_init(void)
{
    /*...*/
}

static void __exit dummy_pdrv_exit(void)
{
    /*...*/
}

module_init(dummy_pdrv_init);
module_exit(dummy_pdrv_exit);
MODULE_LICENSE("...");
```

比如說，向下面這個樣子：

```c
# include <linux/module.h>
# include <linux/kernel.h>
# include <linux/init.h>

static int __init dummy_drv_init(void)
{
    pr_info("Initializing module.\n");
    return 0;
}

static void __exit dummy_drv_exit(void)
{
    pr_info("Unloading module.\n");
    return;
}

module_init(dummy_drv_init);
module_exit(dummy_drv_exit);

MODULE_LICENSE("GPL");
```

### Step 1 - 2：Makefile

假定上面這個檔案叫做 `dummy_drv.c`，那麼他的 `Makefile` 會變成下面這個樣子：

```makefile
PWD := $(shell pwd)
KVERSION := $(shell uname -r)
KERNEL_DIR := /lib/modules/$(shell uname -r)/build

MODULE_NAME = dummy_drv
obj-m := $(MODULE_NAME).o

all:
        make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
        make -C $(KERNEL_DIR) M=$(PWD) clean
```

雖然說文件有解釋到比如 `obj-m` 是什麼意思 (還有 `obj-y` 之類的)、`M` 出現的用意是什麼。但我不得不說如果之前沒有看範例，很難想像核心的 Makefile 會跟平常的樣子這麼不同。

### Step 1 - 3：載入模組

編譯完之後，會得到一個 `dummy_drv.ko`，這時可以用 `insmod` 把他載入：

```shell
$ sudo insmod dummy_drv.ko
```

如果去看 `dmesg`：

```shell
$ dmesg
```

就會發現剛剛的初始化函數被執行了，所以印出了東西啦：

```shell
[ 4578.757460] Initializing module.
```

而想要列出目前有哪些核心模組，可以用 `lsmod`。比如說：

```shell
$ lsmod | grep dummy_drv
```

就會找到剛剛載入進去的模組：

```shell
dummy_drv              16384  0
```

最後，把這個模組移除：

```shell
$ sudo rmmod dummy_drv
```

在結束的時候，就會發現剛剛 `module_exit` 註冊的函數被執行了：

```shell
[ 4739.557045] Unloading module.
```

## Step 2：Platform Driver

剛剛是最基本的核心模組，幾乎什麼功能都沒有，只是載入的時候說聲嗨，結束的時候說聲掰。以這個為基礎，可以再把他稍微加一點點功能。雖然這還是沒功能，但因為 `platform_driver` 是最像 I2C driver 的東西，所以就簡單用它來舉例。

> 如果想要看他有什麼具體的應用，[這裡](https://youtu.be/MdgmyOHldZg) 有一個例子。

### Step 2 - 1：基本要件

除了把一小段程式放到核心裡面印東西之外，現在來試試看讓他多一點點功能：多「註冊」一個 *platform_driver*。而要做到這件事情，就是要把適當的東西填入 [`platform_driver`](https://elixir.bootlin.com/linux/latest/source/include/linux/platform_device.h#L200) 這個結構體中，並且把他「註冊」給核心：

```c
struct platform_driver {
	int (*probe)(struct platform_device *);
	int (*remove)(struct platform_device *);
	void (*shutdown)(struct platform_device *);
	int (*suspend)(struct platform_device *, pm_message_t state);
	int (*resume)(struct platform_device *);
	struct device_driver driver;
	const struct platform_device_id *id_table;
	bool prevent_deferred_probe;
};
```

> 可以發現他裡面有一個 `struct device_driver`，表示他在 Linux Driver Model 中是某個 driver。

雖然看起來有很多東西，但是並不是所有的東西都要填。目前只要填：

1. `probe`：當發現這個 `platform_driver` 可以作為某個 `device` 的 *driver* 時，就會呼叫這個函數來進行初始化。
2. `remove`：跟 `probe` 相反。那個 *device* 被移除時，要執行的清理動作。

所以現在就來把它加進去吧！就加入最簡單的印東西：

```makefile
# include <linux/module.h>
# include <linux/kernel.h>
# include <linux/init.h>
+# include <linux/platform_device.h>

+static int dummy_probe(struct platform_device *pdev)
+{
+    pr_info("Dummy device is being probed.\n");
+    return 0;    
+}

+static int dummy_remove(struct platform_device *pdev)
+{
+    pr_info("Dummy device is removing.\n");
+    return 0;
+}

+static struct platform_driver dummy_drv = {
+    .probe = dummy_probe,
+    .remove = dummy_remove,
+    .driver = {
+        .name = "dummy device 0.1",
+        .owner = THIS_MODULE,
+    },
+};

static int __init dummy_pdrv_init(void)
{
    pr_info("Dummy platform_driver installing.\n");
    platform_driver_register(&dummy_drv);
    pr_info("finishing inittializing platform_driver.\n");
    return 0;
}

static void __exit dummy_pdrv_exit(void)
{
    pr_info("Unregistering dummy platform driver.\n");
    platform_driver_unregister(&dummy_drv);
    pr_info("Unregistered dummy platform driver.\n");
    return;
}

module_init(dummy_pdrv_init);
module_exit(dummy_pdrv_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A trivial module for platform driver");
```

### Step 2 - 2：編譯並載入模組

編譯用的 Makefile 跟剛剛大致相同，只是我把模組的名城換成了 `dummy_platform_drv`。一樣把他載入進核心：

```shell
$ sudo insmod dummy_platform_drv.ko
```

安裝之後，看看多了什麼新的東西：

```shell
$ dmesg
```

就會發現出現像下面這樣的輸出：

```bash
[ 2314.583880] dummy_platform_drv: loading out-of-tree module taints kernel.
[ 2314.584305] Dummy platform_driver installing.
[ 2314.584480] finishing inittializing platform_driver.
```

這時可以發現：除了初始化的函數之外...`probe` 根本沒有被用到啊！這聽起來滿合理的，因為這邊完全沒有提供這個 *driver* 支援哪些硬體的資訊，所以很理所當然沒人知道哪個 *device* 要配給這個 *driver*，所以更不會有配對成功之後要做什麼的問題。那問題就是：要怎麼去幫 *driver* 跟 *device* 做配對？

其中一個答案是這樣：假定一開始在 *device tree* 就有這個硬體的資訊，而在 *driver* 中也揭露他負責服務哪些硬體。那麼 *driver* 寫出來註冊進去之後，*bus* 只要用某種機制去比對 *device tree* 與 *driver* 提供的資訊，就可以知道哪些 *driver* 該配對哪些硬體了！這就 *device tree* 的用處之一。