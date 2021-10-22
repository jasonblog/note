## Day 15：SPI Driver (Part 1) - DT Overlay

這篇主要介紹怎麼樣用 DT Overlay 去把 SPI 的硬體跟驅動程式配對起來。

## 術語：Protocol Driver 與 Controller Driver

就像 I2C 有 *adapter* 跟 *client* 類似，SPI 中針對 *master* 與 *master* 底下的節點也兩種不同的驅動：一種是幫掛在 SPI Bus 上的節點寫驅動程式，另外一個就是寫 Bus 本身的驅動程式。在 SPI 文件的術語中，前者叫做 *SPI protocol driver*，也就是接下來要做的; 後者叫做 *SPI master controller driver*。

至於為什麼要叫做 *protocol driver*？在原始程式碼的 [include/linux/spi/spi.h](https://elixir.bootlin.com/linux/latest/source/include/linux/spi/spi.h#L278) 中有提到：

*"This represents the kind of device driver that uses SPI messages to interact with the hardware at the other end of a SPI link. It's called a "protocol" driver because **it works through messages rather than talking directly to SPI hardware** (which is what the underlying SPI controller driver does to pass those messages)."*

## Protocol Driver (Part 1)

其實跟 I2C 一樣：在 device tree 中給資料、在 driver 中給 OF match table，然後生出 `probe` 跟 `remove`。可以參考文件中的 [*How do I write an “SPI Protocol Driver”?*](https://www.kernel.org/doc/html/latest/spi/spi-summary.html#how-do-i-write-an-spi-protocol-driver) 部分。而對應的 API 可以查 [*The Linux driver implementer’s API guide*](https://www.kernel.org/doc/html/latest/driver-api/index.html) 中的 [*Serial Peripheral Interface (SPI)*](https://www.kernel.org/doc/html/latest/driver-api/spi.html?highlight=spi#serial-peripheral-interface-spi) 一節。

```c
static struct spi_driver CHIP_driver = {
        .driver = {
                .name           = "CHIP",
                .owner          = THIS_MODULE,
                .pm             = &CHIP_pm_ops,
        },

        .probe          = CHIP_probe,
        .remove         = CHIP_remove,
};
```

其中，各個函數的 prototype 可以參考 [include/linux/spi/spi.h](https://elixir.bootlin.com/linux/latest/source/include/linux/spi/spi.h#L278) 中的 `struct spi_driver`：

```c
struct spi_driver {
	const struct spi_device_id *id_table;
	int			(*probe)(struct spi_device *spi);
	int			(*remove)(struct spi_device *spi);
	void			(*shutdown)(struct spi_device *spi);
	struct device_driver	driver;
};
```

### Step 0：顯然的例子

在還沒配對以前，先用簡單的例子看看大致上的架構怎麼樣。一個最簡單的程式大概像下面這樣：

```c
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/printk.h>

static int dummy_probe(struct spi_device *spi)
{
    pr_info("Dummy SPI device being probed.\n");
    return 0;
}

static int dummy_remove(struct spi_device *spi)
{
    pr_info("Dummy SPI device being removed.\n");
    return 0;
}

static struct spi_driver dummy_spi_driver = {
    .driver = {
        .name = "dummy_spi_driver",
	.owner = THIS_MODULE,
    },
    .probe = dummy_probe,
    .remove = dummy_remove,
};

module_spi_driver(dummy_spi_dri);
```

看起來跟 I2C 幾乎一樣。而 Makefile 也是，頂多只是檔案名稱換了而已：

```makefile
PWD := $(shell pwd)
KVERSION := $(shell uname -r)
KERNEL_DIR := /lib/modules/$(shell uname -r)/build

MODULE_NAME = dummy_spi_module
obj-m := $(MODULE_NAME).o

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
```

執行完 `make` 之後，用 `insmod` 把編譯出來的 `.ko` 檔載入，然後再開 `dmesg` 會發現出現了訊息：

```
[ 1596.657242] dummy_spi_module: loading out-of-tree module taints kernel.
```

很明顯的，因為沒有在這個程式中實作任何與配對有關的機制，所以 `probe` 跟 `remove` 都不會被執行到。為了可以讓他們兩個進行 match，所以接下來就在 *device tree* 中加入一些東西：

### Step 1：DT Overlay - 加上 Arduino

雖然一開始不知道這個 DT overlay 中要處理什麼，但是可以先看看 `spi0` 中，其他節點的例子做參考：

```python
spi@7e204000 {
         ...
         spidev@1 {
                 compatible = "spidev";
                 #address-cells = < 0x01 >;
                 #size-cells = < 0x00 >;
                 phandle = < 0x68 >;
                 reg = < 0x01 >;
                 spi-max-frequency = < 0x7735940 >;
         };

         spidev@0 {
                 compatible = "spidev";
                 #address-cells = < 0x01 >;
                 #size-cells = < 0x00 >;
                 phandle = < 0x67 >;
                 reg = < 0x00 >;
                 spi-max-frequency = < 0x7735940 >;
         };
};
```

這樣看起來，最重要的是 `compatible`、`reg` 跟 `spi-max-frequncy`。事實上，一個節點中必須提供什麼屬性，可以文件中查到。比如說 SPI 就可以在： [*Documentation/devicetree/bindings/spi/spi-controller.yaml*](https://elixir.bootlin.com/linux/latest/source/Documentation/devicetree/bindings/spi/spi-controller.yaml)中，[查到](https://elixir.bootlin.com/linux/latest/source/Documentation/devicetree/bindings/spi/spi-controller.yaml#L139) 下面這樣的資訊：

```
required:
    - compatible
    - reg
```

其中，`compatible` 之前就碰過了，是用來搓合 *device* 跟 *driver* 的字串; 而文件中又有說到：`reg` 是 *Chip Select* 的編號。所以這樣就解決了兩個問題。

接下來的問題是傳輸速度。這個就要去查 Arduino 的 SPI 是以什麼速度傳輸。這方面可以參考 Arduino 文件中的[敘述](https://www.arduino.cc/en/Reference/SPISetClockDivider)，以面有提到：

*"Sets the SPI clock divider relative to the system clock. On AVR based boards, the dividers available are 2, 4, 8, 16, 32, 64 or 128. The default setting is SPI_CLOCK_DIV4, which sets the SPI clock to one-quarter the frequency of the system clock (4 Mhz for the boards at 16 MHz)."*

因此，可以知道預設是 4MHz，也就是 `0x3d0900`。因此就把他也記上去。所以第一個部分大概就會像這樣：

```python
...
    fragment@1 {
        target = <&spi0>;
        __overlay__ {
            #address-cells = <1>;
            #size-cells = <0>;
            arduino-spi@0 {
                compatible="arduino";
                reg = <0x00>;
                spi-max-frequency = <0x3d0900>;
            };
        };
    };
...
```

### Step 2：DT Overlay - 停用一個 spidev

除了上述的性質之外，另外可以發現一件事情就發現 `soidev` 佔用了兩個 *Chip Select*，但在之前的觀察中，知道 `spi0` 只有兩個 *Chip Select*，所以就要停用一個。而停用的方法就是去找 `spidev@0` 的 *label*，然後把他的 `status` 屬性設成 `disable`。也就是像下面這樣：

```python
...
    fragment@0 {
        target = <&spidev0>;
        __overlay__ {
            status = "disabled";
        };
     };
...
```

`spidev` 跟 `i2c-dev` 類似，是一個揭露給 usersapce 的 SPI 介面。一個有趣的地方是， [`spidev.c`](https://elixir.bootlin.com/linux/latest/source/drivers/spi/spidev.c#L736) 中的程式碼中可以看到：

*"spidev should never be referenced in DT without a specific compatible string, it is a Linux implementation thing rather than a description of the hardware."*

(但總之他在裡面了)

### Step 3：全部的結果

把上述兩個部分合起來，完整的 overlay 就會變成像下面這樣：

```python
/dts-v1/;
/plugin/;

/ {
    compatible = "brcm,bcm2835";
    fragment@0 {
        target = <&spidev0>;
        __overlay__ {
            status = "disabled";
        };
     };
    fragment@1 {
        target = <&spi0>;
        __overlay__ {
            #address-cells = <1>;
            #size-cells = <0>;
            arduino-spi@0 {
                compatible="arduino";
                reg = <0x00>;
                spi-max-frequency = <0x3d0900>;
            };
        };
    };
};
```

在 `fragment@0` 的部分，就是把 `spidev0` 的狀態改成 `disabled`。眼尖的人可能會發現：怎麼知道 `spidev@0` 就是 `spidev0`？這個可以去 *devie tree* 中的 `__symbols__` 部分找到：

```python
__symbols__ {
    ...
    spidev0 = "/soc/spi@7e204000/spidev@0";
    ...
}
```

### Step 4：編譯並觀察結果

把準備 overlay 的部分存起來 (假定叫做 `arduino-spi.dts`)，然後編譯：

```shell=
$ dtc -@ -I dts -O dtb -o arduino-spi.dtbo arduino-spi.dts
```

接著複製到 `/boot/overlay` 當中：

```shell
$ sudo cp arduino-spi.dtbo /boot/overlays/
```

最後再 `/boot/config.txt` 檔案中，把 `dtoverlay` 加上剛剛編譯出來的東西：

```python
# Uncomment this to use arduino spi transfer
dtoverlay=arduino-spi
```

重開機之後，就會發現 `spi0` 中多出了 `arduino-i2c` 的東西了：

```python
spi@7e204000 {
        compatible = "brcm,bcm2835-spi";
        clocks = < 0x07 0x14 >;
        status = "okay";
        #address-cells = < 0x01 >;
        interrupts = < 0x02 0x16 >;
        cs-gpios = < 0x10 0x08 0x01 0x10 0x07 0x01 >;
        #size-cells = < 0x00 >;
        dma-names = "tx\0rx";
        phandle = < 0x28 >;
        reg = < 0x7e204000 0x200 >;
        pinctrl-0 = < 0x0e 0x0f >;
        dmas = < 0x0b 0x06 0x0b 0x07 >;
        pinctrl-names = "default";

        spidev@1 {
                compatible = "spidev";
                #address-cells = < 0x01 >;
                #size-cells = < 0x00 >;
                phandle = < 0x68 >;
                reg = < 0x01 >;
                spi-max-frequency = < 0x7735940 >;
        };

        arduino-spi@0 {
                compatible = "arduino";
                reg = < 0x00 >;
                spi-max-frequency = < 0x3d0900 >;
        };

        spidev@0 {
                compatible = "spidev";
                status = "disabled";
                #address-cells = < 0x01 >;
                #size-cells = < 0x00 >;
                phandle = < 0x67 >;
                reg = < 0x00 >;
                spi-max-frequency = < 0x7735940 >;
        };
};
```

### Step 5：加上 OF Match Table

DT 做好了之後，接下來就把 *driver* 的部分加上 `of_match_table` ：

```c
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/printk.h>
+#include <linux/of.h>
+#include <linux/of_device.h>

static int dummy_probe(struct spi_device *spi)
{
    pr_info("Dummy SPI device being probed.\n");
    return 0;
}

static int dummy_remove(struct spi_device *spi)
{
    pr_info("Dummy SPI device being removed.\n");
    return 0;
}

+static const struct of_device_id arduino_dt_ids [] = {
+    {.compatible = "arduino", 0},
+    {}
+};
+MODULE_DEVICE_TABLE(of, arduino_dt_ids);

static struct spi_driver dummy_spi_driver = {
    .driver = {
        .name = "dummy_spi_driver",
	.owner = THIS_MODULE,
+   .of_match_table = of_match_ptr(arduino_dt_ids),
    },
    .probe = dummy_probe,
    .remove = dummy_remove,
};

module_spi_driver(dummy_spi_driver);
MODULE_LICENSE("GPL");
```

### Step 6：載入並驗證結果

重新編譯之後在載入模組，就會發現 `probe` 有執行到了：

```
[ 1596.657242] dummy_spi_module: loading out-of-tree module taints kernel.
[ 2785.720109] Dummy SPI device being probed.
```

除此之外，移除的時候也會發現 `remove` 有被執行到了：

```
[ 2971.886278] Dummy SPI device being removed.
```