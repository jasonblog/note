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

开始以为是下载依赖失败了，因为打包机器之前出现过下载https的maven库失败的问题，但是这次看了下载的依赖jar地址是http的，在机器上执行wget也能下载。所以应该不是下载问题。

当然是不是下载的问题去看看就知道了，gradlew下载的依赖文件都在

```sh
~/.gradle/caches/modules-2/files-2.1
```
进入后发现依赖的文件是有的。这个问题最后没找到原因，我把caches目录整个删了，重新下了一遍所有依赖就ok了。
之前打包只是把gradle deamon给关了反之编译缓存造成问题。没想到工程缓存也会有问题，对于打正式发布的包来说，建议打包前除了执行git reset保证代码同步，执行gradle clean清理环境外，也要

- 关闭deamon，
- 清理依赖缓存
营造一个全新的打正式包环境。