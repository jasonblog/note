## Day 23：IIO (Part 1) - 構造改革！自動幫感測器做好介面的子系統！



在看完 GPIO、I2C、SPI 等等基本周邊之後，可能會覺得接下來會發生的事情是像 Arduino 那樣，用一些相關子系統的函式來讀取某些感測器的數值。不過，核心中的資料最終需要想辦法傳遞給 userspace，所以聽起來，每次有新的感測器時，除了要解決 `probe`、裝置樹的文題，還要自己實作各種 `sysfs` 的介面跟字元驅動程式等等工夫...嗎？

答案是不用。事實上，這類與電腦硬體自身無關的感測器，有專門的子系統來處理。也就是接下來要提到的 *Industrial IO* 子系統。只要適當地幫這些感測器註冊，那麼這個子系統就會根據註冊時提供的資訊，自動在 `sysfs` 加上適當的介面，並且提供 *buffer*、*event-triggered* 的機制給 userspace 使用。

## IIO 子系統：專門處理感測器

IIO 系統的主要開發者 *Jonathan Cameron* 在 2018 年的 ELCE 之中，以 [*10 Years of the Industrial I/O Kernel Subsystem*](https://youtu.be/644oH1FXdtE) 為題，講到了這個子系統的由來。當初他正在做一個監測運動員生理功能的計畫，但不確定這些感測器應該屬於哪個子系統。於是他去 *Linux Kernal Mailing List* [詢問](https://lore.kernel.org/lkml/4832A211.4040206@gmail.com/)。而在 IIO 出現之前，可能的候選有 `hwmon` (**h**ard**w**are **mon**ioring)，用來監測比如說 CPU 的溫度、風扇速度、電壓大小等等跟電腦硬體內部有關感測器的資訊; 以及 `input` 子系統，負責如滑鼠之類的外部輸入。最終，這兩者都不適合一般的感測器。所以就開始開發了這個新的子系統。

## 例子：DHT11

在這之前，先看看 DHT11 的例子。首先修改 `/boot/config.txt` 的內容，加上下面這一行 `dht11` 的裝置樹：

```diff
+dtoverlay=dht11
```

重開機之後，可以檢查裝置樹中，有沒有 `dht11` 的部分：

```shell
$ dtc -I fs /proc/device-tree | less
```

若裝置樹成功裝上去，則可以在裝置樹中查到以下的內容：

```python
        dht11@0 {
                gpios = < 0x10 0x04 0x00 >;
                compatible = "dht11";
                status = "okay";
                phandle = < 0x89 >;
                pinctrl-0 = < 0x88 >;
                pinctrl-names = "default";
        };
```

除此之外，若列出現在有的模組：

```shell
$ lsmod
```

則可以看到 DHT11 的模組：

```shell
spi_bcm2835            24576  0
dht11                  16384  0
industrialio           69632  1 dht11
uio_pdrv_genirq        16384  0
uio                    20480  1 uio_pdrv_genirq
```

### userspace 介面

從 IIO 子系統的[文件](https://www.kernel.org/doc/html/v4.14/driver-api/iio/core.html) 中提到：

*There are two ways for a user space application to interact with an IIO driver.*

1. `/sys/bus/iio/iio:deviceX/`*, this represents a hardware sensor and groups together the data channels of the same chip.*
2. `/dev/iio:deviceX`*, character device node interface used for buffered data transfer and for events information retrieval.*

依照文件的描述，去看底下的目錄：

```shell
$ ls /sys/bus/iio/devices/iio\:device0/
```

就會發現裡面有兩個檔案。分別是 `in_humidityrelative_input` 與 `in_temp_input`：

```shell
$ ls /sys/bus/iio/devices/iio\:device0/
dev                        of_node
in_humidityrelative_input  power
in_temp_input              subsystem
name                       uevent
```

## 追蹤程式

而這樣的東西是怎麼實作出來的呢？為了更進一步了解讀取 IIO 提供的節點時發生了什麼事，可以用 `ftrace` 去看看：

```shell
$ sudo trace-cmd record \
    -p function_graph \
    -a \
    -F \ 
    cat /sys/bus/iio/devices/iio\:device0/in_temp_input
```

會得到類似以下的結果：

```diff
 sys_read() {
        ...
        iio_read_channel_info() {
 +          dht11_read_raw() {
                mutex_lock() {
                    _cond_resched() {
                        rcu_all_qs();
                    }
                }
                ktime_get_with_offset() {
                    arch_counter_read();
                }
                ...
```

去查詢 `dht11_read_raw` 這個函數，可以發現在[*drivers/iio/humidity/dht11.c*](https://github.com/raspberrypi/linux/blob/rpi-5.4.y/drivers/iio/humidity/dht11.c) 當中。這麼方便的功能，在 `probe` 裡面做了什麼呢？去把他的程式碼找出來看看：

```c
static int dht11_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct dht11 *dht11;
	struct iio_dev *iio;

	iio = devm_iio_device_alloc(dev, sizeof(*dht11));

	dht11 = iio_priv(iio);
	dht11->dev = dev;
	dht11->gpiod = devm_gpiod_get(dev, NULL, GPIOD_IN);
	dht11->irq = gpiod_to_irq(dht11->gpiod);


	dht11->timestamp = ktime_get_boottime_ns() - DHT11_DATA_VALID_TIME - 1;
	dht11->num_edges = -1;

	platform_set_drvdata(pdev, iio);

	init_completion(&dht11->completion);
	mutex_init(&dht11->lock);
	iio->name = pdev->name;
	iio->dev.parent = &pdev->dev;
	iio->info = &dht11_iio_info;
	iio->modes = INDIO_DIRECT_MODE;
	iio->channels = dht11_chan_spec;
	iio->num_channels = ARRAY_SIZE(dht11_chan_spec);

	return devm_iio_device_register(dev, iio);
}
```

其中，`dht11` 的結構為：

```c
struct dht11 {
	struct device			*dev;
	struct gpio_desc		*gpiod;
	int				irq;
	struct completion		completion;
	struct mutex			lock;
	s64				timestamp;
	int				temperature;
	int				humidity;
	int				num_edges;
	struct {s64 ts; int value; }	edges[DHT11_EDGES_PER_READ];
};
```

可以發現：這當中並沒有實作複雜的字元驅動程式。其中只是註冊了適當的 IIO 子系統，而 IIO 子系統就會自動在 `sysfs` 中，依照感測器的性質配置適合的介面，比如上面的 `in_humidityrelative_input`、`in_temp_input`。

關於這個子系統，明天再更進一步實驗！