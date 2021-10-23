## Day 24：IIO (Part 2) - 基本術語與架構



\#今天介紹一些 IIO 的基本術語。

## iio_chan_spec：給定 Channel 資訊

在 IIO 的術語中，感測器可以提供的數據稱作 *channel*。比如說 DHT11 提供的資料有溫度與濕度，那麼溫度與濕度就是 2 個不同的 *channel*。而 *channel* 的資料結構由 `struct iio_chan_spec` 表示。定義在[include/linux/iio/iio.h](https://elixir.bootlin.com/linux/latest/source/include/linux/iio/iio.h#L236)

```c
struct iio_chan_spec {
	enum iio_chan_type	type;
	int			channel;
	int			channel2;
	unsigned long		address;
	int			scan_index;
	struct {
		char	sign;
		u8	realbits;
		u8	storagebits;
		u8	shift;
		u8	repeat;
		enum iio_endian endianness;
	} scan_type;
	long			info_mask_separate;
	long			info_mask_separate_available;
	long			info_mask_shared_by_type;
	long			info_mask_shared_by_type_available;
	long			info_mask_shared_by_dir;
	long			info_mask_shared_by_dir_available;
	long			info_mask_shared_by_all;
	long			info_mask_shared_by_all_available;
    ...
	unsigned		modified:1;
	unsigned		indexed:1;
};
```

### type：哪種功能的感測器？

`type` 指定的是感測器的種類，而有哪些種類則可以在 [*include/uapi/linux/iio/types.h*](https://elixir.bootlin.com/linux/latest/source/include/uapi/linux/iio/types.h#L14) 中找到：

```c
enum iio_chan_type {
	IIO_VOLTAGE,
	IIO_CURRENT,
	IIO_POWER,
	IIO_ACCEL,
	IIO_ANGL_VEL,
	IIO_MAGN,
	IIO_LIGHT,
	IIO_INTENSITY,
	IIO_PROXIMITY,
	IIO_TEMP,
	IIO_INCLI,
    ...
}
```

比如說跟電壓 (`IIO_VOLTAGE`)、溫度 (`IIO_TEMP`)、加速度 (`IIO_ACCEL`) 等等。

### info_mask_*：屬性與適用範圍

`info_mask_*` 對應的代表 *channel* 屬性的位元遮罩。而這個屬性適用的範圍可能有所不同，比如說這個屬性是「是用於所有輸出的 *channel*」「所有用來量測距離的 *channel*」，或是「只有現在這個 *channel* 是用」。這時就可以在不同後綴的 `info_mask_*` 中去 *mask* 上不同的位元。

### scan_type：數值型態

用來指定數值型態。比如說：是不是有號數？資料寬度多少？這當中有效的位元是多少？*endianess* 是哪種類型？等等與數值表達法相關的屬性。

### indexed：同時有多個輸入/輸出時

如果 `indexed` 設為 `1` ，而且這個裝置有多個 *channel* 的話，就會依照結構中 `channel` 的號碼，在 `sysfs` 底下建立對應編號的檔案。

### modified：感測器的不同「模式」

舉例來說，有一個光感既可以讀取紅外線的強度，也可以同時讀取紅外線與可間光的強度。這時候就可以把 `modidied` 設成 `1`，然後在 `channel2` 中，指定不同的模式。

## iio_info：讀寫 sysfs 時對應的操作

文件在[這裡](https://www.kernel.org/doc/html/v4.14/driver-api/iio/core.html#c.iio_info)。這邊就是針對 `sysfs` 中的節點讀寫時，要進行對應的操作。比如說讀寫就需要去實作 `read_raw` 與 `write_raw` 這些函數：

```c
struct iio_info {
    ...
    int (* read_raw) (struct iio_dev *indio_dev,struct iio_chan_spec const *chan,int *val,int *val2, long mask);
    ...
    int (* write_raw) (struct iio_dev *indio_dev,struct iio_chan_spec const *chan,int val,int val2, long mask);
    ...
};
```

在這當中，`indio_dev` 就是對應的 `struct iio_dev`，而 `chan` 就是對應的 `iio_chan_spec`。