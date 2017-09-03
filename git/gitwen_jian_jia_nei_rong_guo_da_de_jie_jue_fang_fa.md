# git文件夹内容过大的解决方法


.git文件夹大的原因是commit内容多

###方法一
clone的时候，可以指定深度

```sh
git clone .... --depth 1
```

表示只克隆最近一次commit

###方法二
新建一个没有commit的分支，再删除原来的分支

```sh
git checkout --orphan new_start
```

这条命令会创建一个叫做 new_start 的分支，该分支没有任何历史记录
然后提交：

```sh
git commit -m  xxx
```

push到远程仓库github

```sh
git push origin new_start
```

直接git push origin new_start:master应该也可以，作者没验证

删除原来的远程的master分支

```sh
git branch -D master
D表示强制删除
删除过程会报错，需要在github做修改
对responsible的设置进行修改
setting中的Default branch 改为new_start
然后在执行上面的命令
```

将刚才提交的new_start分支作为远程仓库的master分支

```sh
git push origin new_start:master
（此处，作者为了作为博客内容使用，所以进行修改，可以不运行此代码）
```

最后删除newstart分支 git branch －D newstart

或者clone时创建新的分支替代默认Origin HEAD（master）

```sh
git clone -b [new_branch_name]  xxx.git
```

