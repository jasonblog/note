# 反組譯 apk

Java 反組譯dex2jar + jd-gui 解讀apk，觀看java原碼


首先，如果我的Android程式有test.apk檔

請將test.apk改為test.zip解壓縮後

取得classes.dex


- https://sourceforge.net/projects/dex2jar/


```sh
chmod a+x d2j_invoke.sh
chmod a+x d2j-dex2jar.sh
```


```
d2j-dex2jar.sh classes.dex
```

然後產生了classes_dex2jar.jar


## jd-gui 反組譯 jar 成 java

再用jd-gui去讀取classes_dex2jar.jar，就可以觀看java 程式碼了。