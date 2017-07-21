# 目的檔

## 目的檔格式

可執行檔格式(Executable)

```sh
Windows : PE  (Portable Executable)
Linux   : ELF (Executable Linkable Format)
Intel   : OMF (Object Module Format)
Unix    : .out
DOS     : .COM
```

以上都是 COFF(Common object file format) 格式的變化

動態連結程式庫

```sh
Windows : .dll
Linux   : .so
```

靜態連結程式庫

```sh
Windows : .lib
Linux   : .a
```


動態/靜態連結程式庫也是按照可執行檔格式儲存
##ELF 格式

