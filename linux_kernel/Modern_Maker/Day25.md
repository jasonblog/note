## Day 25：IIO (Part 3) - 變數命名大師？第一個 IIO 驅動程式！

這篇文章簡單講述 IIO 的用法，並且寫一些顯然的驅動程式來檢驗看看裡面的功能。

## Step 1：配置一個 iio_dev

首先要用 `iio_device_alloc` 配置一個 `struct iio_dev`。接著初始化他，包含提供 `iio_chan_spec` 與 `iio_info` 等資訊，最後交給 `iio_device_register` 註冊。所以第一步就是先配置一個 `iio_dev`，這邊使用 [*devres*](https://www.kernel.org/doc/Documentation/driver-model/devres.txt) 系列的函數，以方便管理：

```c
struct device *dev = pdev-> dev;
struct iio_dev *iio;
iio = devm_iio_device_alloc(dev, 0);
```

這個函數在 [*drivers/iio/industrialio-core.c*](https://elixir.bootlin.com/linux/latest/source/drivers/iio/industrialio-core.c#L1565) 中有說明：

*"Managed iio_device_alloc. iio_dev allocated with this function is automatically freed on driver detach."*

其中，第二個變數是幫配置的空間，可以用 `iio_priv(struct iio_dev*)` 這個函數取回。比如說可以定義：

```c
struct dummy_iio_dev {
    struct dev *dev;
    struct gpio_desc *gpio;
};
```

然後把 `iio_dev` 的私有資料拿來配置給這個結構：

```c
iio = devm_iio_device_alloc(dev, sizeof(dummy_iio_dev));
dummy = iio_priv(iio);
dummy->dev = dev;
dummy->gpiod = devm_gpiod_get_index(dev, "dummy", 0, GPIOD_IN);
```

之後的函數中如果有 `iio_dev` 這個結構，就可以直接用 `iio_priv(iio_dev)` 去找到 `dummy_iio_dev` 這個結構。

## Step 2：這個感測器有多少 Channel？

每一個 *channel* 都代表一種提供資料的方法。比如說如果只有一個 *channel*，而且類型是 `IIO_PROXIMITY`：

```c
static const struct iio_chan_spec dummy_chan_spec[] = {
    {
        .type = IIO_PROXIMITY,
	.info_mask_separate = BIT(IIO_CHAN_INFO_PROCESSED),
    },
};
```

那麼就會在 `sys/bus/iio/devices/iio:deviceX/` 底下建立一個 `in_proximity_input` 名稱的節點：

```shell
$ ls /sys/bus/iio/devices/iio\:device1/
dev                 name   subsystem
in_proximity_input  power  uevent
```

如果這個感測器有多個數值可以給使用這讀取，那麼可以把 `indexed` 設成 `1`，然後用 `channels` 把這幾個 *channel* 的編號給出來。比如說：

```c
static const struct iio_chan_spec dummy_channels[] = {
    {
        .type = IIO_PROXIMITY,
        .indexed = 1,
        .channel = 0,
	    .info_mask_separate = BIT(IIO_CHAN_INFO_PROCESSED),
    },
    {
        .type = IIO_PROXIMITY,
        .indexed = 1,
        .channel = 1,
	    .info_mask_separate = BIT(IIO_CHAN_INFO_PROCESSED),
    },
};
```

這時候在 `sysfs` 對應的資料夾，就會有對應編號的檔案：

```shell
$ ls /sys/bus/iio/devices/iio\:device1/
dev                  name       uevent
in_proximity0_input  power
in_proximity1_input  subsystem
```

除此之外，也有一些後綴可以用。比如說加速規有 `x`, `y`, `z` 3 個方向的加速度。如果想要加上這個資訊，可以把 `modified` 設成 `1`，然後把 `channel2` 指定成 [*include/uapi/linux/iio/types.h*](https://elixir.bootlin.com/linux/latest/source/include/uapi/linux/iio/types.h#L52) 中的 `enum iio_modifier` 的成員。這樣就可以在 `sysfs` 中的檔案增加後綴。比如說，如果提供的 `iio_channel_spec` 如下：

```c
static const struct iio_chan_spec dummy_channels[] = {
    {
        .type = IIO_PROXIMITY,
        .modified = 1,
        .channel2 = IIO_MOD_X,
	    .info_mask_separate = BIT(IIO_CHAN_INFO_PROCESSED),
    },
    {
        .type = IIO_PROXIMITY,
        .modified = 1,
        .channel2 = IIO_MOD_Y,
	    .info_mask_separate = BIT(IIO_CHAN_INFO_PROCESSED),
    },
};
```

那麼就會發現裡面多出了 `x` 跟 `y` 兩個不同的檔案：

```shell
$ ls /sys/bus/iio/devices/iio\:device1/
dev                   name       uevent
in_proximity_x_input  power
in_proximity_y_input  subsystem
```

除此之外，這邊的 `info_mask_*` 中的資訊是提示 *userspace* 的使用者這裡的數值是什麼樣的數值。比如說是某個量測到的，需要再轉換的數值？一個需要某個偏移量來修正的數值？或是已經換算好的某個物理量？等等。

## Step 3：實作讀取/寫入動作

接下來要實作「當 *userspace* 對某個 *channel* `read`時，該生出什麼資料給他？」這裡要實作一個原型像下面這樣的函數：

```c
static int read_raw (struct iio_dev *iio, struct iio_chan_spec const *chan
		int *val0, int *val1, long mask)
{
    /* ... */
    
    (*val0) = some_value;
    return IIO_VAL_INT;
}
```

在這個函數中，`chan` 這個變數，就是指現在正在對哪個 *channel* 讀取。因此任務就是依照不同的 *channel* 給出不同的輸出。

1. 依照給定的 `chan` 跟 `mask` ，把預期要給使用者讀取的數值，存放在 `val0` 跟 `val1` 指向的位置。

2. 用回傳值指定這個數值的資料型態：比如說如果回傳 `IIO_VAL_INT`，那就表示 `*val0` 當中存放的是 `int`; 又比如如果回傳 `IIO_VAL_INT_PLUS_NANO`，那麼回傳的值就會是個整數部分為 `*val0`，小數部分為 `(*val1)*10e-9` 形式的定點數。

    而有哪些數值型態可以在 [*include/linux/iio/types.h*](https://elixir.bootlin.com/linux/latest/source/include/linux/iio/types.h#L21) 當中找到。

舉例來說，如果實作的

```c
static int dummy_read_raw (struct iio_dev *iio, struct iio_chan_spec const *chan,
		int *val0, int *val1, long mask)
{
    *val0 = 99999;
    *val1 = 11111;
    return IIO_VAL_INT;
}
```

那麼在讀取 `sysfs` 的檔案時，就只會看到 `*val0` 的數值，因此就是個整數：

```shell
$ cat /sys/bus/iio/devices/iio\:device1/in_proximity_input 
99999
```

而如果換成 `IIO_VAL_INT_PLUS_NANO`：

```c
static int dummy_read_raw (struct iio_dev *iio, struct iio_chan_spec const *chan,
		int *val0, int *val1, long mask)
{
    *val0 = 99999;
    *val1 = 11111;
    return IIO_VAL_INT_PLUS_NANO;
}
```

使用者在 `sysfs` 讀取到的數值就會是以 `*val0` 為整數部分，`(*val1)*10e-9` 為小數部分的數值。以上面這個例子也就是：

```shell
$ cat /sys/bus/iio/devices/iio\:device1/in_proximity_input
99999.000011111
```

## Step 4：填寫 iio_info

實作好上面的 `read_raw` (或其他 [`iio_info`](https://www.kernel.org/doc/html/v4.12/driver-api/iio/core.html#c.iio_info) 中的函數) 後，把他們填進 `struct iio_info` 中：

```c
struct iio_info dummy_iio_info = {
    .read_raw = dummy_read_raw,
};
```

## Step 5：填寫與註冊 iio_dev

把剛剛的 `struct iio_chan_spec` 跟 `struct iio_info` 填進 `struct iio_dev` 中，最後使用 `iio_device_register` 系列的函數註冊這個 `iio_dev` (這邊是使用 `devm_iio_device_register`)：

```c
static int dummy_iio_probe(struct platform_device *pdev)
{

    ...
    struct iio_dev *iio;
    iio = devm_iio_device_alloc(dev, 0);

    iio -> name = pdev->name;
    iio -> info = &dummy_iio_info;
    iio -> modes = INDIO_DIRECT_MODE;
    iio -> channels = dummy_channels;
    iio -> num_channels = ARRAY_SIZE(dummy_channels);
    return devm_iio_device_register(dev, iio);
}
```

在其他的狀況下，有可能要另外實作 `remove` 做資源清理。但這邊的狀況中，因為資源配置都是用 `devm*` 系列的函數做資源配置，所以在移除之後會自動處理清理的問題，因此就沒有實作 `remove`。

## 完整程式

雖然上面沒有講到 *device tree*，但畢竟要有裝置讓 `probe` 可以發生。這邊就修改 GPIO 那邊的裝置樹 (僅有修改名稱與輸入輸出而已)，包含 `pinctrl` 的部分。

### DT overlay

```python
/dts-v1/;
/plugin/;
/ {
    compatible="brcm,brcm2835";
    fragment@0 {
        target = <&gpio>;
        __overlay__ {
            dummy_gpio: dummy_gpio_pins {
                brcm,pins = <0x11>;
                brcm,function = <0x0>;
                brcm,pull = <0x1>;
            };
        };
    };
    fragment@1 {
        target-path = "/";
        __overlay__ {
            dummy_iio: dummy_iio_device {
                dummy-gpios = <&gpio 0x11 0x0>;
                compatible = "dummy_iio";
                status = "ok";
                pinctrl-0 = <&dummy_gpio>;
                pinctrl-names = "default";
            };
        };
    };
};
```

### dummy_iio.c

在這個模組中，裡面定義了 6 個 *channel*，分別去觀察在 `*val0` 與 `*val1` 相同，而`iio_info` 中的 `write_raw` 回傳數值不同時，分別會對使用者從 `sysfs` 中觀察到的結果有什麼影響：

```c
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/iio/consumer.h>
#include <linux/iio/iio.h>
#include <linux/of.h>

#define IIO_CHANNEL_DEFINE(num)    {\
        .type = IIO_PROXIMITY,\
        .indexed = 1,\
        .channel = (num),\
	    .info_mask_separate = BIT(IIO_CHAN_INFO_PROCESSED),\
    }\

static const struct iio_chan_spec dummy_channels[] = {
    IIO_CHANNEL_DEFINE(0),
    IIO_CHANNEL_DEFINE(1),
    IIO_CHANNEL_DEFINE(2),
    IIO_CHANNEL_DEFINE(3),
    IIO_CHANNEL_DEFINE(4),
    IIO_CHANNEL_DEFINE(5),
};

static int dummy_read_raw (struct iio_dev *iio, struct iio_chan_spec const *chan,
		int *val0, int *val1, long mask)
{
    *val0 = 9876;
    *val1 = 1234;
    switch(chan -> channel) {
        case 0:
            return IIO_VAL_INT;
        case 1:
            return IIO_VAL_INT_PLUS_MICRO;
        case 2:
            return IIO_VAL_INT_PLUS_MICRO_DB;
        case 3:
            return IIO_VAL_INT_MULTIPLE;
        case 4:
            return IIO_VAL_FRACTIONAL;
        case 5:
           return IIO_VAL_FRACTIONAL_LOG2;
        default:
           return 0;
    }
}

struct iio_info dummy_iio_info = {
    .read_raw = dummy_read_raw,
};

static int dummy_iio_probe(struct platform_device *pdev)
{
    struct device *dev = &(pdev-> dev);
    struct iio_dev *iio;

    iio = devm_iio_device_alloc(dev, 0);
    if (!iio) {
        dev_err(dev, "Failed to allocate IIO/.\n");
	return -ENOMEM;
    }
    iio -> name = pdev->name;
    iio -> info = &dummy_iio_info;
    iio -> modes = INDIO_DIRECT_MODE;
    iio -> channels = dummy_channels;
    iio -> num_channels = ARRAY_SIZE(dummy_channels);

    return devm_iio_device_register(dev, iio);
}

static const struct of_device_id dummy_ids[] = {
    {.compatible = "dummy_iio",},
    {}
};

static struct platform_driver dummy_driver = {
    .driver = {
        .name = "dummy driver",
	.of_match_table = dummy_ids,
    },
    .probe = dummy_iio_probe
};
MODULE_LICENSE("GPL");
module_platform_driver(dummy_driver);
```

### Makefile

```makefile
PWD := $(shell pwd)
KVERSION := $(shell uname -r)
KERNEL_DIR := /lib/modules/$(shell uname -r)/build

MODULE_NAME = dummy_iio
obj-m := $(MODULE_NAME).o

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
```

## 結果與觀察

修改裝置樹的部分參考前面的文章，這邊就不贅述。編譯模組並載入之後，去對應的 `iio` 裝置節點查看檔案。以這邊為例，是在 `iio:device1`：

```shell
$ cd /sys/bus/iio/devices/iio:device1
```

首先列出底下的檔案：

```shell
$ ls
```

可以發現裡面有 6 個對應的 `in_proximityN_input`，其中 `N` 是從 0 ~ 5 的整數：

```shell
$ ls
dev                  in_proximity3_input  power
in_proximity0_input  in_proximity4_input  subsystem
in_proximity1_input  in_proximity5_input  uevent
in_proximity2_input  name
```

如果列出所有的數值結果：

```shell
$ cat in_proximity{0..5}_input
9876
9876.001234
9876.001234 dB
9876 1234 
8.003241491
0.000000000
```

1. `IIO_VAL_INT` 剛剛描述過。
2. `IIO_VAL_INT_PLUS_MICRO` 則類似 `IIO_VAL_INT_PLUS_NANO`，只是 `val1` 的單位變成由「奈」變成「微」;
3. `IIO_VAL_INT_PLUS_MICRO_DB` 只是把數值後面多加上分貝
4. `IIO_VAL_INT_MULTIPLE` 則是回傳兩個整數值;
5. `IIO_VAL_FRACTIONAL` 回傳的是 (`*val0/*val1`)。
6. `IIO_VAL_FRACTIONAL_LOG2` 回傳的是 `*val0 >>(*val1)`

其中，關於 `IIO_VAL_FRACTIONAL_LOG2`，如果把 `*val1` 改成 `4` 的話，會出現類似以下的輸出：

```shell
$ cat in_proximity{0..5}_input
9876
9876.000004
9876.000004 dB
9876 4 
2469.000000000
617.250000000
```

其中，最後一個數值 `617.250000000` 恰好就是 9876/16。更多的實作細節可以在 [drivers/iio/inkern.c](https://elixir.bootlin.com/linux/latest/source/drivers/iio/inkern.c#L584) 找到。