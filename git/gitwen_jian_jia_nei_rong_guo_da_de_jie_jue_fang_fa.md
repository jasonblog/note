# git文件夾內容過大的解決方法


.git文件夾大的原因是commit內容多

###方法一
clone的時候，可以指定深度

```sh
git clone .... --depth 1
```

表示只克隆最近一次commit

###方法二
新建一個沒有commit的分支，再刪除原來的分支

```sh
git checkout --orphan new_start
```

這條命令會創建一個叫做 new_start 的分支，該分支沒有任何歷史記錄
然後提交：

```sh
git commit -m  xxx
```

push到遠程倉庫github

```sh
git push origin new_start
```

直接git push origin new_start:master應該也可以，作者沒驗證

刪除原來的遠程的master分支

```sh
git branch -D master
D表示強制刪除
刪除過程會報錯，需要在github做修改
對responsible的設置進行修改
setting中的Default branch 改為new_start
然後在執行上面的命令
```

將剛才提交的new_start分支作為遠程倉庫的master分支

```sh
git push origin new_start:master
（此處，作者為了作為博客內容使用，所以進行修改，可以不運行此代碼）
```

最後刪除newstart分支 git branch －D newstart

或者clone時創建新的分支替代默認Origin HEAD（master）

```sh
git clone -b [new_branch_name]  xxx.git
```

