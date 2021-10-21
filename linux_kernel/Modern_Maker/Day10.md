## Day 10：I2C Driver (Part 1) - 使用 Device Tree 來找 Driver

前面提到：Device Tree 可以提供硬體的資訊，而 *driver* 可以借助這個資訊來「找到」對應的硬體。所以可以互相搭配來幫 Device Tree 上描述到的硬體來找到對應的 *driver*：

1. Device Tree 方面：任務是更動 Device Tree，想辦法在目前的 Device Tree 加上一些東西告訴他說「現在多了這個硬體」。或是用專業的術語：在 Device Tree 上新增某個硬體對應的節點，並且在 `compatible` 的屬性中，加上硬體相容的相關資訊。而這件事情可以用 device tree overlay 來做到。
2. Driver 方面：在 *driver* 中加上他該支援的硬體的相關資訊。這個結構會是個 `of_device_id` 的陣列，到時候要加到 `device` 這個結構的 `of_match_table` 中。

這樣一來，在配對的時候，就可以同時比對 1. 跟 2. 的資訊去知道哪個 *driver* 需要負責處理哪個 *device tree* 中的節點。而這就是接下來要做的實驗。

接下來的情況是：假想現在要 `i2c1` 這個 I2C 上的 `0x8` 位置有個 Arduino 。然後不知道為什麼我就是很想弄成 I2C Driver，要看看能不能幫這個 Arduino 弄一個 i2c driver 出來。為了簡單，現在這個驅動程式只會幫 Arduino 佔用一個 i2c 的位址，其他什麼都不會做。

## Step 1：Device Tree 方面

要新增節點，除了暴力改原來的 Device Tree 之外，另外一個方法就是使用 *Device Tree Overlay*。前面講到：Device Tree 後面的東西可以疊在前面的上面，現在這邊就是要利用這個特性來「加上」新的硬體描述。這樣就可以更有彈性地調整與維護 Device Tree，也比較不用擔心原來的東西被改到爛掉。

### Step 1 - 0：原先的 Device Tree

在開始之前，可以看看一下原先的 *device tree* 長什麼樣子。這可以去查看 `/proc/device-tree` 這個資料夾。這個資料夾的階層會對應 *device tree* 的階層，所以如果用 `tree` 這個程式去看這個資料夾的話：

```shell
$ tree /proc/device-tree | less
```

就會發現這個資料夾其實就是把 *device tree* 中每一個節點及屬性，都用資料夾與檔案顯示出來 (就像 `sysfs` 那樣)。以 `i2c1` 的部分為例，大致上像下面這樣：

```shell
│   ├── i2c@7e804000
│   │   ├── #address-cells
│   │   ├── clock-frequency
│   │   ├── clocks
│   │   ├── compatible
│   │   ├── interrupts
│   │   ├── name
│   │   ├── phandle
│   │   ├── pinctrl-0
│   │   ├── pinctrl-names
│   │   ├── reg
│   │   ├── #size-cells
│   │   └── status
```

為什麼知道 `i2c@7e804000` 跟 `i2c1` 是同一個東西呢？這可以去 `/proc/device-tree/aliases` 查：

```shell
$ ls /proc/device-tree/aliases
```

就會發現裡面是：

```shell
audio      gpio   i2c_arm  mmc     serial0  spi2
aux        i2c    i2c_vc   mmc0    serial1  thermal
axiperf    i2c0   i2s      mmc1    soc      uart0
dma        i2c1   intc     name    sound    uart1
ethernet0  i2c10  leds     random  spi0     usb
fb         i2c2   mailbox  sdhost  spi1     watchdog
```

接著 `cat i2c1`，就會發現：

```shell
/soc/i2c@7e804000
```

事實上，alias 就是 *device tree* 冒號前面的那個東西。以下面這個例子為例， `i2c@7e804000` 這個節點的 alias 就是 `i2c1`。這個 alias 可以方便之後在 Device Tree 的其他部分去直接參考他。

```c
i2c1: i2c@7e804000 {
    ...
};
```

而接下來的任務，就是要想辦法在上面加上一個節點。

### Step 1 - 1：Device Tree Overlay

現在這個問題是這樣：在 *device tree* 中，i2c 部分的硬體描述可能是像這樣的：

```c
i2c1: i2c@7e804000 {
    ...
};
```

然後現在希望達成的結果的是：這裡面中多出想要加入的硬體。比如說在 `0x8` 上面希望多一塊 Arduino。也就是最後的結果大概像這樣：

```c
i2c1: i2c@7e804000 {
    ...
    arduino-i2c@8 {
        ...
        reg = <0x8>;
    };
};
```

### Step 1 - 2：寫出要疊上去的部分

要做到這件事情，要把要疊的東西放在不同的 `fragment@n{ ... };` 中，其中 `n` 依序從 0 開始往上增加。比如說現在要新增一個 `arduino-i2c` 佔用 i2c 的 `0x8` 位址，那就把 `target` 設定成 `&i2c1`，並且把要疊上去的東西寫在 `__overlay__` 當中：

```c
/dts-v1/;
/plugin/;

/ {
    compatible = "brcm,bcm2835";
    fragment@0 {
        target = <&i2c1>;
        __overlay__ {
            #address-cells = <1>;
            #size-cells = <0>;
            arduino-i2c@8 {
                compatible = "arduino";
                reg = <0x8>;
            };
        };
    };
};
```

這邊除了提供位址是 `0x8` 之外，另外一件重要的事情是 **`compatible` 的字串。等一下在 \*driver\* 中的時候，就會以這個字串為配對的依據。** 寫完之後，假定這個檔案叫做 `arduino-i2c.dts`，接下來就用 `dtc` 編譯成 `.dtbo` 檔：

```shell
$ dtc -@ -I dts -O dtb -o arduino-i2c.dtbo arduino-i2c.dts
```

然後移動到 `/boot/overlays/` 裡面：

```shell
$ sudo cp arduino-i2c.dtbo /boot/overlays/
```

### Step 3 - 3：修改 /boot/config.txt

最後，修改 `/boot/config.txt`，把 `dtoverlay` 多加上要 overlay 的 `.dtbo` 檔。以這邊為例，就是 `arduino-i2c`，所以用 `vim` 編輯：

```shell
$ sudo vim /boot/config.txt
```

然後在檔案中，新增下面的部分：

```shell
# Uncomment this to enable infrared communication.
#dtoverlay=gpio-ir,gpio_pin=17
#dtoverlay=gpio-ir-tx,gpio_pin=18

+# Uncomment this to use arduino i2c transfer
+dtoverlay=arduino-i2c
+dtdebug=1

# Additional overlays and parameters are documented /boot/overlays/README
```

這邊把 `dtdebug` 也打開，這樣就可以用 `vcdbg` 這個工具除錯。

> 按：如果有需要的話，可以先把原先的 `config.txt` 備份起來

### Step 3 - 4：重新開機

重新開機之後，可以用 `vcdbg` 來看看出現了什麼訊息：

```shell=
$ sudo vcdbg log msg
```

如果成功的話，就會出現 `Open overlay ...` 之類的訊息，像下面這樣：

```shell
[...]
001812.784: dtdebug:   override i2c_arm: string target 'status'
001818.938: brfs: File read: 1911 bytes
+001833.006: dtdebug: Opened overlay file 'overlays/arduino-i2c.dtbo'
001833.503: brfs: File read: /mfs/sd/overlays/arduino-i2c.dtbo
001839.485: Loaded overlay 'arduino-i2c'
001839.570: dtparam: audio=on
[...]
```

(失敗的話會出現類似 `001368.106: dtdebug: Failed to open overlay file "..."` 的訊息)

而這時如果去 `/ptoc/device-tree` 看的話：

```shell
$ tree /proc/device-tree | less
```

就會發現多出了剛剛新增的節點：

```shell
│   ├── i2c@7e804000
│   │   ├── #address-cells
│   │   ├── arduino-i2c@8
│   │   │   ├── compatible
│   │   │   ├── name
│   │   │   └── reg
│   │   ├── clock-frequency
│   │   ├── clocks
│   │   ├── compatible
│   │   ├── interrupts
│   │   ├── name
│   │   ├── phandle
│   │   ├── pinctrl-0
│   │   ├── pinctrl-names
│   │   ├── reg
│   │   ├── #size-cells
│   │   └── status
```

## Step 2：I2C Client Driver 方面

現在已經知道 i2c1 中上面有掛一個 `compatible` 是 `arduino` 的硬體了，接下來的 *driver* 就要想辦法說「我可以給 `compatible` 裡面有 `arduino` 的東西用！」。而這個資訊要記錄在 `of_device_id` 這個結構中。

### Step 2 - 1：提供 of_device_id

這時把前面 *device tree overlay* 時，填寫的 `compatible` 裡面的字串寫進去。並且注意最後面要多留一個空白的：

```c
static struct of_device_id dummy_id_tables [] = {
    { .compatible="arduino", },
    { }
};
```

這個結構可以在 [include/linux/mod_devicetable.h](https://elixir.bootlin.com/linux/latest/source/include/linux/mod_devicetable.h#L260) 中找到。結構大概長成這樣：

```c
/*
 * Struct used for matching a device
 */
struct of_device_id {
	char	name[32];
	char	type[32];
	char	compatible[128];
	const void *data;
};
```

除了 `compatible` 這個屬性之外，另一個重要的東西是 `data`：這裡面可以存某個裝置需要用的資料。可以想像如果 `compatible` 不只有一個，那麼就可以依照不同的硬體，去提供不同的資料。

### Step 2 - 2：註冊 of_device_id

接下來要把這個東西用 `MODULE_DEVICE_TABLE` 註冊：

```c
MODULE_DEVICE_TABLE(of, dummy_id_tables);
```

### Step 2 - 3：填進 i2c_driver 結構中

之後要把這個 `of_device_id` 填進 `i2c_driver` 的 `struct device` 的 `of_match_table` 裡面。所以到時候就會出現類似這樣的東西：

```c
static struct i2c_driver dummy_drv = {
    .probe = dummy_probe,
    .remove = dummy_remove,
    .driver = {
    	.name = "dummy device 0.1",
	.owner = THIS_MODULE,
	.of_match_table = dummy_id_tables,
    },
};
```

不過，這個東西除了填進 `of_match_table` 之外，還要填進 `probe` 跟 `remove`。因為 `probe` 跟 `remove` 都還沒有做，所以現在先把他們做出來。

### Step 2 - 4：實作 probe 跟 remove

把 `probe` 跟 `remove` 兩個函數實作出來。因為現在只是要做一個「佔某一個 I2C 位子」的 *driver*，為了方便這邊就只印東西出來：

```c
static int dummy_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    pr_info("Dummy device is being probed.\n");
    return 0;    
}

static int dummy_remove(struct i2c_client *client)
{
    pr_info("Dummy device is removing.\n");
    return 0;
}
```

### Step 2 - 5：完整的程式碼

這時，完整的程式大概就會像這樣：

```c
# include <linux/module.h>
# include <linux/kernel.h>
# include <linux/init.h>
# include <linux/i2c.h>

static int dummy_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    pr_info("Dummy device is being probed.\n");
    return 0;    
}

static int dummy_remove(struct i2c_client *client)
{
    pr_info("Dummy device is removing.\n");
    return 0;
}

static struct of_device_id dummy_id_tables [] = {
    { .compatible="arduino", },
    { }
};

MODULE_DEVICE_TABLE(of, dummy_id_tables);

static struct i2c_driver dummy_drv = {
    .probe = dummy_probe,
    .remove = dummy_remove,
    .driver = {
    	.name = "dummy device 0.1",
	.owner = THIS_MODULE,
	.of_match_table = dummy_id_tables,
    },
};

module_i2c_driver(dummy_drv);
MODULE_LICENSE("GPL");
```

這邊可能會納悶為什麼唯有 `module_init` 跟 `module_exit`，可是多了 `module_i2c_driver`。這是因為 `module_i2c_driver` 自動就會幫忙做這件事。如果這整個模組初始化跟結束的時候都沒有其他的事情要做，那麼可以用他。可以參考 [include/linux/i2c.h](https://elixir.bootlin.com/linux/latest/source/include/linux/i2c.h#L914) 中的敘述：

```c
/**
 * module_i2c_driver() - Helper macro for registering a modular I2C driver
 * @__i2c_driver: i2c_driver struct
 *
 * Helper macro for I2C drivers which do not do anything special in module
 * init/exit. This eliminates a lot of boilerplate. Each module may only
 * use this macro once, and calling it replaces module_init() and module_exit()
 */
#define module_i2c_driver(__i2c_driver) \
	module_driver(__i2c_driver, i2c_add_driver, \
			i2c_del_driver)
```

### Step 2 - 6：Makefile

Makefile 其實跟之前的核心模組相差不大，只是程式名稱跟模組的名稱有變而已。不過為了完整還是放上來：

```shell
PWD := $(shell pwd)
KVERSION := $(shell uname -r)
KERNEL_DIR := /lib/modules/$(shell uname -r)/build

MODULE_NAME = dummy_i2c_drv
obj-m := $(MODULE_NAME).o

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
```

### Step 2 - 7：編譯、插入、確認結果

使用 `make` 進行編譯：

```shell
$ make
```

這時候裡面會出現一個 `dummy_i2c_drv.ko`。這個就是核心模組。在尚未插入前，`i2cdetect` 的結果是像下面這樣的：沒有人在用 I2C：

```shell
$ i2cdetect -y 1
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- -- 
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
70: -- -- -- -- -- -- -- --                         
```

現在把這個模組載入：

```shell
$ sudo insmod dummy_i2c_drv.ko 
```

接著再用一次 `i2cdetect`，就會發現剛剛 Arduino 的那個 `0x8` 位址現在被一個核心模組佔用住了：

```shell
$ i2cdetect -y 1
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- UU -- -- -- -- -- -- -- 
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
70: -- -- -- -- -- -- -- --                         
```

而如果這時查看 `dmesg`：

```shell
$ dmesg
```

也會發現裡面出現了 `probe` 成功的訊息：

```shell
[ 1329.705486] dummy_i2c_drv: loading out-of-tree module taints kernel.
[ 1547.898723] Dummy device is being probed.
```

最後，把這個模組移除：

```shell=
$ sudo rmmod dummy_i2c_drv 
```

接著再一次 `i2cdetect`，就會發現沒有核心模組佔用這個 I2C 了：

```shell
$ i2cdetect -y 1
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- -- 
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
70: -- -- -- -- -- -- -- --                      
```

最後，看看移除的時候，`dmesg` 出現了什麼訊息：

```shell
$ dmesg
```

這時就發現了剛剛 `remove` 中要印出來的東西：

```shell
[ 4205.630400] Dummy device is removing.
```