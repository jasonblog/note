# gradle 教學

## build.gradle

```sh
buildscript {
    repositories {
        mavenCentral()
    }

    dependencies {
        classpath 'com.android.tools.build:gradle:2.1.3' 
        //classpath 'com.android.tools.build:gradle:1.5.0'
    }
}
```

```sh
https://repo1.maven.org/maven2/com/android/tools/build/gradle/
```

## could not resolve all dependencies for configuration ' classpath'

開始以為是下載依賴失敗了，因為打包機器之前出現過下載https的maven庫失敗的問題，但是這次看了下載的依賴jar地址是http的，在機器上執行wget也能下載。所以應該不是下載問題。

當然是不是下載的問題去看看就知道了，gradlew下載的依賴文件都在

```sh
~/.gradle/caches/modules-2/files-2.1
```
進入後發現依賴的文件是有的。這個問題最後沒找到原因，我把caches目錄整個刪了，重新下了一遍所有依賴就ok了。
之前打包只是把gradle deamon給關了反之編譯緩存造成問題。沒想到工程緩存也會有問題，對於打正式發佈的包來說，建議打包前除了執行git reset保證代碼同步，執行gradle clean清理環境外，也要

- 關閉deamon，
- 清理依賴緩存
營造一個全新的打正式包環境。


--- 

classpath 'com.android.tools.build:gradle:2.1.3'  // 版本要跟 gradle-wrapper.properties 配合

distributionUrl=http\://services.gradle.org/distributions/gradle-2.14.1-all.zip

classpath 'com.android.tools.build:gradle:2.1.3'


```sh
// Top-level build file where you can add configuration options common to all sub-projects/modules.

buildscript {
    repositories {
        mavenCentral()
    }

    dependencies {
        classpath 'com.android.tools.build:gradle:2.1.3'  // 版本要跟 gradle-wrapper.properties
    }
}

allprojects {
    repositories {
        mavenCentral()
    }
}

```
---

```sh
Error:Execution failed for task ':app:processDebugResources'.
> com.android.ide.common.process.ProcessException: org.gradle.process.internal.ExecException: Process 'command '/home/Android/Sdk/build-tools/23.0.2/aapt'' finished with non-zero exit value 127
```

——这个问题很难从提示信息看出原因，其实原因是在64位操作系统下面需要对32位库的支持才能运行aapt这个程序，因为aapt只有32位版本，即便是下载的64位的SDK build-tools,这个aapt依然是32位的。解决方法是安装32位的命令支持库：sudo apt-get install lib32stdc++6 lib32z1

