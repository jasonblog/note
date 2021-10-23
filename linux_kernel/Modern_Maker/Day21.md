## Day 21：GPIO (Part 3) - 所以我說那個裝置樹呢？



講到 GPIO，最直接想到的功能就是使用各種感測器。而現在的問題是這樣：如果想要在核心裡面使用 GPIO，並且接收各類感測器的資料 (就像平常在 Arduino，或是用 Raspberry Pi 的各種 Python 函式庫那樣)，那麼要怎麼做到呢？回顧一下前幾天的做的事情：

1. 在裝置樹上增加節點，並且提供 `compatible` 字串
2. 在 Driver 中提供 `of_device_id`

接著才能夠開始 `probe`，並且進行一連串的初始化。

這邊就有一個問題：I2C 連接的裝置很明顯就是放在 `i2cN` 節點下; SPI 的裝置很明顯也是放在 `spiN` 下。這兩種狀況處理裝置樹的狀況很明顯，就是直接在這兩個節點底下新增節點。

但如果像一些只有用到 GPIO 輸入輸出的感測器，會出現在哪個地方呢？其實我也不知道。既然不知道，就去找例子來看看。

## 找例子：raspberrypi/firmware

講到裝置樹，前幾天在 SPI 的文章中，有提到 [raspberrypi/firmware](https://github.com/raspberrypi/firmware) 這個 repo 中，裡面有各種 DT Overlay 與文件。所以要找裝置樹怎麼做，首先就拿他開刀。

## 例子：DHT11 的 DT Overlay

那麼要找誰當參考資料呢？這邊先找簡單的，最好只有一個 GPIO。比如說：[文件](https://github.com/raspberrypi/firmware/blob/master/boot/overlays/README) 中有關於啟動 [DHT11](https://github.com/raspberrypi/firmware/blob/master/boot/overlays/README#L598) 的 DT Overlay 的資訊：

```shell
Name:   dht11
Info:   Overlay for the DHT11/DHT21/DHT22 humidity/temperature sensors
        Also sometimes found with the part number(s) AM230x.
Load:   dtoverlay=dht11,<param>=<val>
Params: gpiopin                 GPIO connected to the sensor's DATA output.
                                (default 4)
```

這邊注意一個細節：預設 GPIO 是 4，而這個 4 號根據文件中的說法：

*"GPIO numbering uses the hardware pin numbering scheme (aka BCM scheme) and not the physical pin numbers."*

所以這個 4 指得是 GPIO4。這個等一下會再出現。

事實上，不只是 Raspberry Pi 官方有針對他們的支援。有些隨手可得的感測器，驅動程式其實已經合併到 Linux Kernel 中。比如說 [DHT11](https://www.electronicoscaldas.com/datasheet/DHT11_Aosong.pdf) 溫濕度感測器的驅動程式在 [*drivers/iio/humidity/dht11.c*](https://github.com/torvalds/linux/blob/master/drivers/iio/humidity/dht11.c); 而 [SRF04](http://inside.mines.edu/~whoff/courses/EENG383/lab/SRF04 Technical Documentation.pdf) 超聲波測距則是在 [*drivers/iio/proximity/srf04.c*](https://github.com/torvalds/linux/blob/master/drivers/iio/proximity/srf04.c) 中。

回到 Raspberry Pi 的例子。從 DHT11 的 DT overlay 資訊來看，他剛好只需要一個 GPIO。所以現在 **只要比較 overlay 前後裝置樹的變化，就可以知道該怎麼啟動 GPIO 了**。

## 觀察：比較裝置樹的差異

首先，把 overlay 之前的 Device Tree 存起來：

```shell
$ dtc -I fs /proc/device-tree > old_dt.dts
```

然後修改 `/boot/config.txt` 中的資訊，加上 DHT11 的部分：

```diff
...
+# Enable DHT11 device tree overlay
+dtoverlay=dht11
+dtdebug=1
...
```

接著重開機。開機之後再看一次裝置樹：

```shell
$ dtc -I fs /proc/device-tree | less
```

這時候應該會期待出現 DHT11 相關的裝置節點。所以搜尋一下，`dht11`，發現在兩個地方找到他們。第一個是在裝置樹的根部，有一個 `dht11@0` 的節點：

```python
/ {
    ...
    dht11@0 {
            gpios = < 0x10 0x04 0x00 >;
            compatible = "dht11";
            status = "okay";
            phandle = < 0x87 >;
            pinctrl-0 = < 0x86 >;
            pinctrl-names = "default";
    };
    ...
};
```

第二個則是在 `/soc/gpio` 底下，有一個 `dht11_pins@0` 節點：

```python
soc {
    ...
    gpio@7e200000 {
            ...
            dht11_pins@0 {
                    brcm,pins = < 0x04 >;
                    phandle = < 0x86 >;
                    brcm,pull = < 0x00 >;
                    brcm,function = < 0x00 >;
            };
            ...
    };
    ...
};
```

明明只是要啟動一個 GPIO，卻出現了兩個節點？看起來很有趣。為了更徹底的比對兩者的差異，記錄下來新的裝置樹：

```shell=
$ dtc -I fs /proc/device-tree > new_dt.dts
```

並且使用 `diff` 看看哪邊不一樣：

```shell=
$ diff -u old_dt.dts new_dt.dts | less
```

會出現類似以下的結果：

```diff
--- old_dt.dts	2020-10-06 14:32:01.158445459 +0100
+++ new_dt.dts	2020-10-06 14:37:12.360016876 +0100
@@ -97,6 +97,15 @@
 		interrupts = < 0x09 0x04 >;
 	};
 
+	dht11@0 {
+		gpios = < 0x10 0x04 0x00 >;
+		compatible = "dht11";
+		status = "okay";
+		phandle = < 0x87 >;
+		pinctrl-0 = < 0x86 >;
+		pinctrl-names = "default";
+	};
+
 	system {
 		linux,serial = < 0x00 0x57a3535d >;
 		linux,revision = < 0xa02082 >;
@@ -391,6 +400,13 @@
 				brcm,function = < 0x02 >;
 			};
 
+			dht11_pins@0 {
+				brcm,pins = < 0x04 >;
+				phandle = < 0x86 >;
+				brcm,pull = < 0x00 >;
+				brcm,function = < 0x00 >;
+			};
+
 			gpclk0_gpio4 {
 				brcm,pins = < 0x04 >;
 				phandle = < 0x3e >;
@@ -1278,7 +1294,7 @@
 	chosen {
 		bootargs = "coherent_pool=1M 8250.nr_uarts=0 snd_bcm2835.enable_compat_alsa=0 snd_bcm2835.enable_hdmi=1 snd_bcm2835.enable_headphones=1 bcm2708_fb.fbwidth=656 bcm2708_fb.fbheight=416 bcm2708_fb.fbswap=1 vc_mem.mem_base=0x3ec00000 vc_mem.mem_size=0x40000000  console=ttyS0,115200 console=tty1 root=PARTUUID=d29e949e-02 rootfstype=ext4 elevator=deadline fsck.repair=yes rootwait";
 		rpi-boardrev-ext = < 0x00 >;
-		kaslr-seed = < 0xee847e9d 0x1e9edeef >;
+		kaslr-seed = < 0xcba4bf28 0x875888ec >;
 	};
 
 	thermal-zones {
```

可以發現：除了 [KASLR](https://lwn.net/Articles/569635/) 的種子不同 (看起來很合理) 之外，不一樣的地方恰好只有上面所述的兩個地方。

> 事實上，有更簡單的作法。因為這些 DT overlay 的檔案都在 `/boot/overlays` 資料夾底下，所以只要把這些編譯好的 `.dtbo` 檔案反向編譯回去，就可以有一堆現成的例子可以參考。

### 根節點：裝置描述

首先看第一個節點，也就是直接在根節點下方的節點：

```python
/ {
    ...
    dht11@0 {
            gpios = < 0x10 0x04 0x00 >;
            compatible = "dht11";
            status = "okay";
            phandle = < 0x87 >;
            pinctrl-0 = < 0x86 >;
            pinctrl-names = "default";
    };
    ...
};
```

在這個節點當中，出現了熟悉的 `compatible`，以及 `status`。而這當中， `phandle` 是用來辨識節點的數值。每一個節點的 `phandle` 都是唯一的：

*"`<phandle>`：A `<u32>` value. A phandle value is a way to reference another node in the devicetree. Any node that can be referenced defines a phandle property with a unique `<u32>` value. That number is used for the value of properties with a phandle value type."*

除了這個之外，還有下面兩類不知道功能的屬性：

1. **GPIO**：`gpios = < 0x10 0x04 0x00 >`
2. **pinctrl**：`pinctrl-0 = < 0x86 >` 與 `pinctrl-names = "default"` 這兩個部分。

關於 `gpios` 的部分，參考核心文件中的 [Documentation/devicetree/bindings/gpio/gpio.txt](https://www.kernel.org/doc/Documentation/devicetree/bindings/gpio/gpio.txt) 說明：

```diff
The following example could be used to describe GPIO pins used as device enable
and bit-banged data signals:

	gpio1: gpio1 {
		gpio-controller;
		#gpio-cells = <2>;
	};
	[...]

	data-gpios = <&gpio1 12 0>,
		     <&gpio1 13 0>,
		     <&gpio1 14 0>,
		     <&gpio1 15 0>;

+In the above example, &gpio1 uses 2 cells to specify a gpio. The first cell is
+a local offset to the GPIO line and the second cell represent consumer flags,
+such as if the consumer desire the line to be active low (inverted) or open
+drain. This is the recommended practice.
```

可以知道使用的方法是 `<controller 編號 設定>`。比如說，`gpios = < 0x10 0x04 0x00 >`，就是「`phandle` 為 `0x10` 的 GPIO Controller 中，編號 4 號的 GPIO，功能設定成 `0`」，而這恰好就是剛剛裝置樹的文件中，提到的預設腳位。

而如果去查 `0x10` 對應的 `phandle` 是誰，就會發現正好是 gpio 對應的節點：

```diff=
gpio@7e200000 {
        compatible = "brcm,bcm2835-gpio";
        gpio-controller;
        #interrupt-cells = < 0x02 >;
        interrupts = < 0x02 0x11 0x02 0x12 >;
+       phandle = < 0x10 >;
        reg = < 0x7e200000 0xb4 >;
        #gpio-cells = < 0x02 >;
        pinctrl-names = "default";
        interrupt-controller;
        ...
};
```

這樣一來，就可以知道 `gpios` 屬性的意義是什麼了：`phandle` 編號為 `0x10` 的 *GPIO controller* 中，編號 4 號的 GPIO 的設定是 0。

所以現在就剩下一個問題：那個 `pinctrl-0` 不知道是做什麼用的。不過可以發現：另外一個出現 `dht11` 字樣的節點，他的 `phandle` 恰好就是 `0x86`：

```python
soc {
    ...
    gpio@7e200000 {
            ...
            dht11_pins@0 {
                    brcm,pins = < 0x04 >;
                    phandle = < 0x86 >;
                    brcm,pull = < 0x00 >;
                    brcm,function = < 0x00 >;
            };
            ...
    };
    ...
};
```

### GPIO 下的節點：pinctrl

如果去查文件中 `pinctrl-0` 這個屬性的 [文件](https://www.kernel.org/doc/Documentation/devicetree/bindings/pinctrl/pinctrl-bindings.txt)，會發查到以下的內容：

```shell
pinctrl-0:	List of phandles, each pointing at a pin configuration
		node. These referenced pin configuration nodes must be child
		nodes of the pin controller that they configure. Multiple
		entries may exist in this list so that multiple pin
		controllers may be configured, or so that a state may be built
		from multiple nodes for a single pin controller, each
		contributing part of the overall configuration. See the next
		section of this document for details of the format of these
		pin configuration nodes.
```

言下之意，`pinctrl-0` 屬性會指向一個代表該 GPIO 腳位設定的節點。而設定的方式以 Raspberry Pi 為例，在核心的 [Documentation/devicetree/bindings/pinctrl/brcm,bcm2835-gpio.txt](https://www.kernel.org/doc/Documentation/devicetree/bindings/pinctrl/brcm,bcm2835-gpio.txt) 文件當中。而為什麼要這麼麻煩有兩個東西呢？這個就留到明天！