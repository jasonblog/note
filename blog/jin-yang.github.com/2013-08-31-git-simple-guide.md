---
title: Git 簡明教程
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: git
description: 如果你嚴肅對待編程，就必定會使用"版本管理系統"（Version Control System）。
---

如果你嚴肅對待編程，就必定會使用"版本管理系統"（Version Control System）。

<!-- more -->

![git logo]({{ site.url }}/images/misc/git-logo.png "git logo"){: .pull-center width="60%" }

## 簡介

不像 SVN ，Git 採用的是分佈式的管理，簡單來說，如下所示。

![git decentralized model]({{ site.url }}/images/misc/git-decentralized-model.png "git decentralized model"){: .pull-center width="60%" }

所有人除了可以從 origin 獲取提交最新的代碼之外，每個人還可以從不同人的代碼庫中獲取代碼，這也就意味著，比如兩或多個人可以共同開發一個特性，測試無誤之後再提交到 origin 即可。

另外，需要注意的是，所有版本控制系統，只能跟蹤文本文件的改動；對於像圖片、視頻這類的二進制文件，雖能管理，但沒法跟蹤文件內容的變化，只知道從 1K 變為了 2K 而改變的內容不知道。


### 常見概念

簡單介紹下版本開發過程中，經常使用的一些概念。

##### alpha、beta、gamma

用來標識測試的階段和範圍。

* alpha 內測，指開發團隊內部測試的版本或者有限用戶體驗測試版本。
* beta 公測，即針對所有用戶公開的測試版本。
* gamma，基本是 beta 做過一些修改後，成為正式發佈的候選版本，也稱為 RC 。

##### Release Candidate, RC

發行候選版本，和 Beta 版最大的差別在於 Beta 階段會一直加入新的功能，但是到了 RC 版本，幾乎就不會加入新的功能了，而主要著重於除錯。

##### General Availability, GA

開發團隊認為該版本是穩定版，有的軟件可能會標識為 stable 或者 production 版，其意思和 GA 基本相同，也就是官方開始推薦廣泛使用了。

##### 包命名

推薦按照 RPM 包的命名方式，也就是 packagename-version-release.arch.rpm 。

* name: 表示包的名稱，包括主包名和分包名；
* version: 表示包的版本信息；
* release: 用於標識 rpm 包本身的發行號，可還包含適應的操作系統；
* arch: 表示主機平臺，noarch 表示此包能安裝到所有平臺上面。

如 gd-devel-2.0.35-11.el6.x86_64.rpm ，gd 是這個包的主包名；devel 是這個包的分包名； 2.0.35 是表示版本信息，2 為主版本號，0 表示次版本號，35 為源碼包的發行號也叫修訂號； 11.el6 中的 11 是 rpm 的發行號， el6 表示 RHEL6； x86_64 是表示適合的平臺。


### 常見操作

接下來，先看下一些常見的操作。

{% highlight text %}
----- 1. 創建新倉庫
$ git init                             ← 會在.git目錄下保存元數據

----- 2. 創建文件，並添加到版本庫中
$ echo 'Hello World!' > README         ← 新建文件
$ git add README                       ← 添加到版本庫中
$ git ls-files                         ← 查看版本庫中的文件
$ git commit -m 'Initial Commit'       ← 提交併添加註釋
$ git status                           ← 是否有未提交的文件

----- 3. 修改文件
$ echo 'What A Hell.' >> README        ← 修改文件添加一行
$ git commit -a -m 'Add a new line'    ← 提交下
$ git log --pretty=oneline             ← 查看提交信息

----- 4. 回退
$ echo 'It is A real world' > README   ← 覆蓋之前的內容
$ git reset --hard HEAD                ← 回退到之前提交版本，HEAD^(上個) HEAD~N(上N個)
$ git reflog                           ← 查看版本號，貌似是UUID
$ git reset --hard 6fcfc89             ← 回退到指定的版本

----- 4. clone一個副本
$ git clone /tmp/foobar .              ← 從本地複製一個副本
{% endhighlight %}

<!--
如果是遠端服務器上的倉庫，你的命令會是這個樣子：
git clone username@host:/path/to/repository
-->


### 工作區和版本庫

在本地會分為三部分，分別是。

1. Working Directory(Tree)，工作目錄<br>
    也就是保存當前工作的文件所在的目錄，文件會在切換分支時被刪除或者替換。

2. GIT Directory (GIT庫目錄)<br>
    項目所有歷史提交都被保存在了GIT庫目錄中。

3. GIT Index (GIT索引)<br>
    可以看作是工作目錄和GIT庫目錄之間的暫存區，與 `Staging Area` 是相同意思，也就是已經被 ADD 但是尚未被 commit 的內容。

**工作目錄** 就是在電腦上看到的目錄以及文件 (.git隱藏目錄版本庫除外)，包括以後需要再新建的目錄文件等等都屬於工作區範疇。

**版本庫** 工作目錄下有一個隱藏目錄 .git 就是版本庫，其中比較重要的就是 stage(暫存區)，還有自動創建了第一個分支 master，以及指向 master 的一個指針 HEAD。

![git stage commit]({{ site.url }}/images/misc/git-stage-commit.png "git stage commit"){: .pull-center width="50%" }

如前所述，提交到版本庫包含了兩步：

1. 使用 git add 把文件添加到 stage；

2. 使用 git commit 把暫存區的所有內容提交到當前分支上。

關於其中的狀態，可以在修改文件後、執行 add 後、執行 commit 後分別通過 status 命令查看文件的不同狀態。

也就是說，添加到 stage 區域後，只要未提交，可以多次修改文件並 add 到 stage 區。

### upstream downstream

git 中的 upstream 和 downstream 的概念是相對的，如果本地庫中的分支 foo 被 push 到遠端中的分支 bar，那麼 bar 就是 foo 的 upstream，而 foo 就是 bar 的 downstream 。

相關的配置會保存在本地庫的 `.git/config` 文件中。

{% highlight text %}
[core]
    filemode = true
[remote "origin"]
    url = https://github.com/foobar/test.git
    fetch = +refs/heads/*:refs/remotes/origin/*
[branch "master"]
    remote = origin
    merge = refs/heads/master
{% endhighlight %}

如果當前分支與多個主機存在追蹤關係，則可以使用 `-u` 選項指定一個默認主機，這樣後面就可以不加任何參數使用 `git push` ，命令如下，在 push 的同時也會指定當前分支的 upstream 。

{% highlight text %}
----- 初次提交本地分支，並不會設置當前本地分支的upstream分支
$ git push origin local-branch:remote-branch

----- 提交的同時，關聯本地local-branch分支的upstream分支
$ git push --set-upstream origin local-branch:remote-branch
$ git push -u origin local-branch:remote-branch

----- 只設置分支，為新建的本地分支設置在遠程庫中的upstream分支
$ git branch --set-upstream-to=origin/remote-branch local-branch
$ git branch --set-upstream local-branch origin/remote-branch        ← 過期
{% endhighlight %}

通過 `git push -u origin master` 將本地的 master 分支推送到 origin 主機後，同時指定 origin 為默認主機，以及指定的遠端分支，後面就可以不加任何參數使用 `git push` 了。

<!--
$ git remote add origin ssh://github/foobar/test
now configure master to know to track
# git config branch.master.remote origin
# git config branch.master.merge refs/heads/master
and push
# git push origin master
-->

### 子模塊

子模塊可用於多項目使用共同類庫的工具，允許類庫項目做一個庫，子項目做為一個單獨的 git 項目存在父項目中。

{% highlight text %}
----- 添加submodule倉庫
$ git submodule add <remote_url> <local_path>
$ git submodule add -b branch <remote_url> <local_path>
$ git submodule add git@your.gitlab.com:Project/nodus.git Specific/Subdir

----- 查看相關配置信息
$ cat .gitmodules
$ cat .git/config

----- 對於clone一個有submodule的倉庫，默認是不會把submodule也clone下來，需要執行如下步驟
----- 1. 註冊submodule到.git/config裡
$ git submodule init
----- 2. clone submodule
$ git submodule update --recursive
----- 上面兩步等價於下面
$ git submodule update --init --recursive

----- 然後可以直接從submodule的庫中拉取代碼進行測試，也可以指定具體分支
$ git submodule foreach --recursive git fetch origin
$ git submodule add -b branch-name GIT-URL
{% endhighlight %}

如果已經設置了 submodule ，後面做了修改，那麼可以直接通過如下步驟操作。

{% highlight text %}
----- 添加子目錄，並提交
git add PLUGIN/SUBMODULE/DIRECTORY
git commit -m "Add new sub-module"

----- 查看子模塊狀態，獲取到對應的commit-id
git submodule status

----- 通過上述的commit-id到對應的子模塊下查看日誌
git log XXXXXXXXXX
{% endhighlight %}

<!--
#### No submodule mapping found in .gitmodules for path

在Stage中存在一個特殊類型的目錄，可以通過如下方式進行查看。

git ls-files --stage | grep 160000
git rm --cached <path_to_submodule>

#### 指定分支/TAG

----- 查看當前所有分支信息
cd SUBMODULE_DIRECTORY
git branch -avv
----- 切換到某個遠程分支上，添加並提交相關信息
git checkout release/v1.8.4
git add SUBMODULE_DIRECTORY
git commit -m "moved submodule to v1.0"

----- 然後通過如下命令更新git的代碼，會通過.submodule中的配置拉取相應的分支
git submodule update
git submodule update --remote

----- 子模塊更新後，可以通過如下方式拉去最新的配置，並查看當前狀態
git submodule foreach git pull
git submodule status

Read-function of the

// 如果修改了.gitmodule的remote url，使用下面的命令更新submodule的remote url
git submodule sync
git submodule update --init --recursive

1.) Delete the relevant section from the .gitmodules file. You can use below command:

----- 1.1 刪除.gitmodules中的配置，默認submodule_name是路徑名；也可以手動刪除
git config -f .gitmodules --remove-section "submodule.SUBMODULE_NAME"
----- 1.2 刪除.git/config中的相關配置

----- 1. 刪除.git/config中的配置，同上兩步
git submodule deinit -f "SUBMODULE_NAME"

----- 2 保存.gitmodules的配置
git add .gitmodules

----- 3. 刪除gitlink，注意沒有/
git rm --cached PATH_TO_SUBMODULE

----- 4. 清理.git/modules模塊
rm -rf .git/modules/PATH_TO_SUBMODULE

----- 5. 此時可以提交保存一份數據
git commit -m "Removed submodule <name>"

----- 6. 刪除相關的文件
rm -rf PATH_TO_SUBMODULE

fatal: Not a git repository

有兩個文件保存了 submodule 的絕對路徑，當移動了路徑之後，如下的內容就需要進行修改。

{submodule}/.git
.git/modules/{submodule}/config

.git/config
.gitmodules

http://www.jianshu.com/p/ed0cb6c75e25

Branches 'master' and 'origin/master' have diverged.
And branch 'master' may be fast-forwarded.

git cherry origin/master
-->



### 配置文件

總共有三個配置文件：`/etc/gitconfig`、`~/.gitconfig`、`.git/config` ，其優先級依次遞增，後者的配置會覆蓋前面的配置項。

#### 全局配置

簡單來說就是 `git config --system` 命令，可以添加了一個 system 參數，配置內容保存在 `/etc/gitconfig` 文件中，如下是設置 st 簡化命令。

{% highlight text %}
$ git config --system alias.st status     # git st
{% endhighlight %}

#### 用戶配置

執行 `git config` 會修改 `~/.gitconfig` 文件的內容。

{% highlight text %}
----- 設置用戶的默認用戶名和密碼
$ git config --global user.name   "Your Name"
$ git config --global user.email  you@example.com
$ git config --global core.editor vim
$ git config --global merge.tool  vimdiff 
$ git config --list
$ git commit --amend --reset-author                 ← 未提交遠端的可以進行修復

----- 開啟顏色顯示
$ git config --global color.ui true
{% endhighlight %}

#### 工作目錄配置

進入工作根目錄，運行 `git config -e`，這樣就只會修改工作區的 `.git/config` 文件。


## 常見命令參考

在 clone 時會自動在本地分支和遠程分支之間，建立一種追蹤關係 (tracking)；例如，在通過 git clone 從遠程庫中複製代碼庫時，會自動將本地的 master 分支與 origin/master 分支對應。

![git operation]({{ site.url }}/images/misc/git-operatoins.jpg "git operations"){: .pull-center width="80%" }

當然，也可以手動建立追蹤關係，如下命令指定本地 master 分支追蹤遠程的 origin/next 分支。

{% highlight text %}
----- 手動設置上游
$ git branch --set-upstream master origin/next

----- 查看origin遠端的信息，包括哪些遠程分支信息可能過期
$ git remote show origin
----- 刪除origin遠程過期的分支
$ git remote prune origin
{% endhighlight %}

### git pull

取回遠程主機某個分支的更新，然後再與本地的指定分支合併，其完整格式以及常見命令如下。

{% highlight text %}
----- 完整命令
$ git pull <remote-host> <remote-branch>:<local-branch>

----- 取回origin主機的next分支，並與本地的master分支合併
$ git pull origin next:master        ← origin配置在.git/config文件中

----- 如果遠程分支是與當前分支合併，則冒號後面的部分可以省略
$ git pull origin next               ← 等同於如下的兩個命令
$ git fetch origin
$ git merge origin/next

----- 如果當前分支與遠程分支存在追蹤關係，git pull就可以省略遠程分支名
$ git pull origin

----- 如果當前分支只有一個追蹤分支，連遠程主機名都可以省略。
$ git pull
{% endhighlight %}

<!--
如果合併需要採用rebase模式，可以使用--rebase選項。
$ git pull --rebase <遠程主機名> <遠程分支名>:<本地分支名>
-->

### git push

該命令用於將本地分支的更新推送到遠程主機，其格式與 `git pull` 相仿，只是分支方向相反。

{% highlight text %}
----- 完整命令
$ git push <remote-host> <local-branch>:<remote-branch>

----- 將本地的master分支推送到origin主機的master分支；後者不存在，則會被新建
$ git push origin master

----- 省略本地分支名，則表示刪除指定的遠程分支
$ git push origin :master             ← 刪除origin主機的master分支，等同於
$ git push origin --delete master

----- 如果當前分支與遠程分支之間存在追蹤關係，則本地分支和遠程分支都可以省略
$ git push origin                     ← 將當前分支推送到origin主機的對應分支

----- 如果當前分支只有一個追蹤分支，那麼主機名都可以省略。
$ git push

{% endhighlight %}

#### simple & matching

<!--
Git push與pull的默認行為
http://blog.angular.in/git-pushmo-ren-fen-zhi/
-->

在 git 全局配置中，有個 `push.default` 屬性決定了 `git push` 操作的默認行為，在 2.0 之前，默認為 `'matching'`，2.0 之後則被更改為了 `'simple'`。除此之外，還有如下的幾個配置項：

* nothing<br>push操作無效，除非顯式指定遠程分支，例如 git push origin develop。
* current<br>push當前分支到遠程同名分支，如果遠程同名分支不存在則自動創建同名分支。
* upstream<br>把當前分支推送到遠程跟蹤分支(upstream)，遠程跟蹤分支必須存在，但是不必跟當前分支同名；常用於從本地分支push/pull到同一遠程倉庫的情景，這種模式叫做central workflow。
* simple<br>simple和upstream是相似的，只是必須保證本地分支和它的遠程upstream分支同名且存在，否則會拒絕push操作。
* matching<br>push所有本地和遠程兩端都存在的同名分支。

簡單來說，對於不帶任何參數的 `git push` 命令；如果只推送當前分支，則稱之為 simple方式；如果推送所有遠程分支的對應本地分支，則為 matching，可以通過如下方式設置。

{% highlight text %}
$ git config --global push.default matching
$ git config --global push.default simple
{% endhighlight %}

另外，還有一種情況，就是不管是否存在對應的遠程分支，都將本地的所有分支都推送到遠程主機，這時就需要使用 \-\-all 選項。

{% highlight text %}
$ git push --all origin
{% endhighlight %}

上面命令表示，將所有本地分支都推送到 origin 主機。

### git log

通過 `git log` 中的兩個高級用法 (A:自定義提交信息的輸出格式；B:過濾提交信息)，基本上就可以找到項目中需要的任何信息 (分支、標籤、HEAD、提交歷史)。

{% highlight text %}
--oneline
  把每一個提交壓縮到了一行中，不過包含分支的信息；
--decorate
  顯示時添加分支以及 tag 信息，可以看到有哪些分支或者設備指向了提交記錄；

<<<<<<<< 查看diff信息
--stat/-p
  查看每次提交時代碼的文件修改量，通常用於查看概覽信息比較有用，+ - 分別表示提交文件的增刪修改比例；
  注意，由於後者用於查看每個文件修改的詳細信息，如果修改代碼比較多那麼現實內容會比較大；

<<<<<<<< 按照用戶分類
shortlog
  按照提交用戶分類，很容易顯示哪些用戶提交了哪些內容，默認是按照用戶ID排序，可以通過-n按照提交量排序。
  git shortlog --format='%H|%cn|%s'

<<<<<<<< 查看分支歷史
--graph
  通過一個ASCII圖像來展示提交歷史的分支結構，可以和--oneline、--decorate選項一起使用，如下所示：
$ git log --graph --oneline --decorate
*   0e25143 (HEAD, master) Merge branch 'feature'
|\
| * 16b36c6 Fix a bug in the new feature
| * 23ad9ad Start a new feature
* | ad8621a Fix a critical security issue
|/
* 400e4b7 Fix typos in the documentation
* 160e224 Add the initial code base
  星號表明這個提交所在的分支，所以上圖的意思是23ad9ad和16b36c6這兩個提交在topic分支上，其餘的在master
  分支上。對於複雜項目可以通過gitk或SourceTree分析。

<<<<<<<< 過濾歷史
----- 顯示最近提交的3次commit記錄
$ git log -3
----- 指定時間範圍(也可以使用1 week ago、yesterday)，注意--since、--until和--after、--before
$ git log --after="2014-7-1" --before="2014-7-4"
----- 按照作者過濾，可以使用正則表達式，同時會匹配郵箱
$ git log --author="John\|Mary"
----- 按照提交信息過濾
$ git log --grep="JRA-224:"
{% endhighlight %}

#### 自定義格式

對於其它的 `git log` 格式需求，可以使用 `--pretty=format:"<string>"` 選項配置，通過不同的佔位符替換相關的信息，詳細可以查看 `man git-show` 。

{% highlight text %}
----- 簡單示例
$ git log --pretty=format:"%cn committed %h on %cd"
{% endhighlight %}

如果定製了一個輸出方案，可保存到 `git config`，或設置 `alias` 以便日後使用，在 `~/.gitconfig` 中加入:

{% highlight text %}
[alias]
    lg = log --graph
{% endhighlight %}

或者運行 `git config --global alias.lg "log --graph"` 。

<!--
git log --graph --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s %Cgreen(%cr)%Creset' --abbrev-commit --date=relative
git log --pretty=format:\"%h %ad | %s%d [%an]\" --graph --date=short
https://github.com/geeeeeeeeek/git-recipes/wiki/5.3-Git-log%E9%AB%98%E7%BA%A7%E7%94%A8%E6%B3%95

如果你的工作流區分提交者和作者，--committer也能以相同的方式使用。
-->

### git tag

標籤可對某一時間點的版本做標記，常用於版本發佈；分為兩種類型：輕量標籤和附註標籤，前者指向提交對象的引用，附註標籤則是倉庫中的一個獨立對象，建議使用附註標籤。

{% highlight text %}
----- 查看標籤，可以查看所有或者正則表達式過濾
$ git tag
$ git tag -l 'v0.1.*'

----- 打標籤，分別為輕量標籤以及創建附註標籤，其中-a表示annotated，也可以指定版本
$ git tag v0.1.2-light
$ git tag -a v0.1.2 -m "發佈0.1.2版本"
$ git tag -a v0.1.1 9fbc3d0

----- 切換標籤或者分支，兩者命令相同
$ git checkout [tagname|branch]

----- 查看標籤的版本信息
$ git show v0.1.2

----- 刪除標籤，誤操作需要刪除後重新添加
$ git tag -d v0.1.2

----- 標籤發佈，默認push不會將標籤提交到git服務器，需要顯示操作，可以提交單個或者所有的
$ git push origin v0.1.2
$ git push origin --tags
{% endhighlight %}

### git stash

<!--
Git 工具 - 儲藏與清理
https://git-scm.com/book/zh/v2/Git-%E5%B7%A5%E5%85%B7-%E5%82%A8%E8%97%8F%E4%B8%8E%E6%B8%85%E7%90%86#_git_stashing
-->

在做了一堆的修改之後，突然有另外的任務要做，切換分支時就會有問題，那麼此時可以通過該命令暫存，完成工作後恢復。

{% highlight text %}
$ git stash save "message ..."       ← 保存，不帶子命令的默認值  
$ git stash apply stash@{0}          ← 默認應用第一個，注意，此時不會刪除保存的stash
$ git stash drop stash@{0}           ← 手動刪除
$ git stash list                     ← 查看所有的stash
$ git stash pop [--index] [<stash>]  ← 恢復最新的保存，並從列表中刪除
$ git stash clear                    ← 刪除所有存儲的進度
{% endhighlight %}

<!--
git stash show [<stash>]
git stash ( pop | apply ) [--index] [-q|--quiet] [<stash>]
git stash branch <branchname> [<stash>]
git stash [push [-p|--patch] [-k|--[no-]keep-index] [-q|--quiet]
      [-u|--include-untracked] [-a|--all] [-m|--message <message>]]
      [--] [<pathspec>…]]
git stash clear
git stash create [<message>]
git stash store [-m|--message <message>] [-q|--quiet] <commit>
https://git-scm.com/book/zh/v1/Git-%E5%B7%A5%E5%85%B7-%E5%82%A8%E8%97%8F%EF%BC%88Stashing%EF%BC%89
-->

在引用時，可以使用類似如下的方式 ```stash@{0}```、```stash@{2.hours.ago}``` 。

### git revert reset

`git revert` 生成一個新的提交來撤銷某次提交，此次提交之前的 commit 都會被保留；`git reset` 回到某次提交，提交及之前的 commit 都會被保留，但是此次之後的修改都會被退回到暫存區。

如下，假設已經通過三次提交保存了三個文件。

{% highlight text %}
$ git log --pretty=oneline
commit3: add file3.txt
commit2: add file2.txt
commit1: add file1.txt

----- 通過revert撤銷一次修改，實際上是提交了一次反向的提交
$ git revert HEAD~1
$ git log --pretty=oneline
commit4: Revert "add file3.txt"
commit3: add file3.txt
commit2: add file2.txt
commit1: add file1.txt
{% endhighlight %}

通過 reset 撤銷上次提交時有三個參數，分別針對 `working tree` 和 `index` 和 `HEAD` 進行重置：

* soft：只回退 commit 信息，不恢復到 index file 一級，如果還要提交，直接 commit 即可；
* mixed：默認方式，回退到某個版本，只保留源碼，回退 commit 和 index 信息；
* hard：徹底回退到某個版本，本地的源碼也會變為上一個版本的內容。

{% highlight text %}
----- 通過soft回退，回退提交信息，文件回到stage區，不會刪除
$ git reset --soft HEAD~1
commit2: add file2.txt
commit1: add file1.txt
$ ls
file1.txt file2.txt file3.txt
$ git status                       ← file3.txt回退到stage區
# On branch master
# Changes to be committed:
#       new file:   file3.txt

----- 通過mixed回退，回退提交信息，文件回到工作區，不會刪除
$ git reset --mixed HEAD~1
commit2: add file2.txt
commit1: add file1.txt
$ ls
file1.txt file2.txt file3.txt
$ git status                       ← file3.txt回退到工作區
# On branch master
# Untracked files:
#       file3.txt

----- 通過hard回退，回退提交信息，文件被刪除
$ git reset --hard HEAD~1
HEAD is now at c518422 add file2.txt
commit2: add file2.txt
commit1: add file1.txt
$ ls
file1.txt file2.txt
$ git status                       ← file3.txt被刪除
# On branch master
nothing to commit, working directory clean
{% endhighlight %}

### git rm

主要介紹 `git rm --cached` 的操作，其原理是刪除 stage 和 repository 中對應文件，但是不會刪除 working directory 中的文件，由於已經從 repository 刪除，那麼文件會從 tracked 變成 untracked 。

### 撤銷操作

經常會有操作錯誤的情況，簡單介紹常見的撤銷操作。

#### 修改最後一次提交

有時候我們提交完了才發現漏掉了幾個文件沒有加，或者提交信息寫錯了。想要撤消剛才的提交操作，可以使用 `--amend` 選項重新提交：

{% highlight text %}
$ git commit --amend
{% endhighlight %}

此命令將使用當前的暫存區域快照提交，如果剛才提交完沒有作任何改動，相當於有機會重新編輯提交說明。

如果剛才提交時忘了暫存某些修改，可以先補上暫存操作，然後再運行 `--amend` 提交：

{% highlight text %}
$ git commit -m 'initial commit'
$ git add forgotten_file
$ git commit --amend
{% endhighlight %}

上面的三條命令最終只是產生一個提交，第二個提交命令修正了第一個的提交內容。

#### 取消已經暫存的文件

已經通過 add 添加到了 staged 中，但是想分開提交，那麼可以通過如下方式操作。

{% highlight text %}
----- 查看狀態，實際在提示中已經有相關的命令，
$ git status
On branch master
Changes to be committed:
  (use "git reset HEAD <file>..." to unstage)

        modified:   README.txt
        modified:   benchmarks.rb
----- 撤銷一個文件的緩存操作，如果不指定文件則會撤銷所有的
$ git reset HEAD benchmarks.rb
Unstaged changes after reset:
M       benchmarks.rb
$ git status
On branch master
Changes to be committed:
        modified:   README.txt

Changes not staged for commit:
        modified:   benchmarks.rb
{% endhighlight %}

#### 取消對文件的修改

在修改了一個文件之後，突然發現修改完全沒有必要，可以通過如下方式撤銷；實際上，在 `git status` 中已經有了提示。

{% highlight text %}
$ git checkout -- benchmarks.rb
$ git status
On branch master
Changes to be committed:
        modified:   README.txt
{% endhighlight %}

注意，這條命令是直接撤消了之前的修改，而且無法恢復，所以執行前一定要確認確實不需要了。

另外，任何已經提交到 git 的都可以被恢復，即便在已經刪除分支中的提交，或者用 `--amend` 重新改寫的提交；可能失去的數據，僅限於沒有提交過的，因為對 git 來說它們就像從未存在過一樣。

### 合併衝突

在對分支進行合併的時候，難免會發生衝突，可以通過如下方式設置。

{% highlight text %}
----- 獲取分支代碼
$ git fetch origin
$ git checkout -b branch origin/branch

----- 切換到開發分支，準備合併代碼
$ git checkout develop
$ git merge --no-ff branch

----- 查找衝突，一般類似與如下的代碼，修改衝突代碼
<<<<<<< HEAD
foobar
=======
conflict
>>>>>>> 6853e5ff961e684d3a6c02d4d06183b5ff330dcc

----- 提交修復後的代碼
$ git add src/conflict.c
$ git commit -m "fix the confict"
$ git push origin develop
{% endhighlight %}

### 版本號

通過 `git describe` 可以輸出類似如下的內容。

{% highlight text %}
$ git describe
v1.8.3.1-8-e1f4155b
{% endhighlight %}

這裡告訴你當前的版本號，也就是距離 `v1.8.3.1` 這個 tag 的第 8 次提交，其提交的 SHA 是 `e1f4155b` 。

也可以通過如下方式設置。

{% highlight text %}
$ git describe --match "v[0-9]*" --abbrev=12 HEAD
{% endhighlight %}



### 其它

{% highlight text %}
$ git rev-parse --abbrev-ref HEAD            ← 當前版本

$ git show <HASHID>                          ← 某次提交的文件具體修改內容
$ git show <HASHID> file                     ← 某次某個文件的修改內容
$ git show <HASHID> --stat                   ← 修改了那些文件的統計
$ git diff --name-status HEAD~2 HEAD~3       ← 標記修改了的文件
$ git log -p <FILENAME>                      ← 某個文件修改歷史

----- 查看當前目錄每個文件的最後提交者
git ls-tree -r --name-only HEAD | while read filename; do
  echo "$(git log -1 --format="%an %ae" -- $filename) $filename"
done

----- 同步tags
$ git fetch origin --prune --tags            ← 1.7之後版本
$ git tag -l | xargs git tag -d              ← 1.7之前版本需要刪除後重新獲取
$ git fetch

----- 同步branches，更新遠端已經刪除的分支，以及手動刪除本地分支
$ git fetch --prune
$ git branch -D local-branch

----- 設置跟蹤遠程分支
$ git checkout --track origin/serverfix

----- 設置代理
$ git config --global http.proxy 'http://user-name:user-password@proxy.foobar.com:8080'
{% endhighlight %}

#### 更新單個文件

可以通過如下命令更新單個文件。

{% highlight text %}
----- 獲取最新的版本，不過不會添加到工作區
$ git fetch

----- 通過下載的最新版本更新單個文件
$ git checkout origin/master -- path/to/file
{% endhighlight %}


<!--
如果遠程主機的版本比本地版本更新，推送時Git會報錯，要求先在本地做git pull合併差異，然後再推送到遠程主機。這時，如果你一定要推送，可以使用–force選項。

$ git push --force origin

上面命令使用–force選項，結果導致在遠程主機產生一個”非直進式”的合併(non-fast-forward merge)。除非你很確定要這樣做，否則應該儘量避免使用–force選項。

最後，git push不會推送標籤(tag)，除非使用–tags選項。

$ git push origin --tags
-->




<!--
### 徹底刪除文件

假設不小把一個大文件或者密碼等信息不小心添加到了版本庫，我們希望把它從版本庫中永久刪除不留痕跡，不僅要讓它在版本歷史裡看不出來，還要把它佔用的空間也釋放出來。

簡單來說，可以通過 git filter-branch 命令永久刪除一些不需要的信息，對應了 git-filter-branch.sh 腳本，可以參考相應的參數 。

通過如下的示例參考。

{% highlight text %}
$ mkdir /tmp/test && cd /tmp/test
$ git init
$ dd if=/dev/urandom of=test.txt bs=10240 count=1024
$ git add test.txt
$ git commit -m "Init commit"
$ git rm test.txt
$ git commit -m "After 'git rm' command"

$ du -hs

$ git filter-branch --tree-filter 'rm -f test.txt' HEAD

Rewrite bb383961a2d13e12d92be5f5e5d37491a90dee66 (2/2)
Ref 'refs/heads/master'
 was rewritten
$ git ls-remote .
230b8d53e2a6d5669165eed55579b64dccd78d11        HEAD
230b8d53e2a6d5669165eed55579b64dccd78d11        refs/heads/master
bb383961a2d13e12d92be5f5e5d37491a90dee66        refs/original/refs/heads/master
$ git update-ref -d refs/original/refs/heads/master [bb383961a2d13e12d92be5f5e5d37491a90dee66]
$ git ls-remote .
230b8d53e2a6d5669165eed55579b64dccd78d11        HEAD
230b8d53e2a6d5669165eed55579b64dccd78d11        refs/heads/master
$ rm -rf .git/logs
$ git reflog --all
$ git prune
$ git gc
$ du -hs
 84K    .
{% endhighlight %}


折騰了半天，還是無法上傳，於是，整個命令出來了：

    --tree-filter表示修改文件列表。
    --msg-filter表示修改提交信息，原提交信息從標準輸入讀入，新提交信息輸出到標準輸出。
    --prune-empty表示如果修改後的提交為空則扔掉不要。在一次試運行中我發現雖然文件被刪除了，但是還剩下個空的提交，就查了下 man 文檔，找到了這個選項。
    -f是忽略備份。不加這個選項第二次運行這個命令時會出錯，意思是 git 上次做了備份，現在再要運行的話得處理掉上次的備份。
    --all是針對所有的分支。

試運行了幾次，看到 40 多次提交逐一被重寫，然後檢查下，發現要刪除的文件確實被刪除了。於是高興地到 github 建立新倉庫，並上傳了。


OK，這個文件已經完完全全刪掉了，版本庫已經不再佔用空間了。
-->




## GitHub

關於使用 github 相關的內容。

### 1. 添加公鑰(可選)

首先，生成一對密鑰對，其中公鑰需要添加到 Github 中，而私鑰必須要保存好。

{% highlight text %}
----- 生成一對密鑰，建議輸入保護私鑰密碼
$ ssh-keygen -t rsa -f ~/.ssh/id_rsa_github -C "jinyang.sia@gmail.com"
{% endhighlight %}

按照如下步驟，將公鑰添加到 Github 中。

![git github add sshkey]({{ site.url }}/images/misc/git-github-add-sshkey.png "git github add sshkey"){: .pull-center width="95%" }

### 2. 新建遠程庫

這個很簡單，只需要添加個庫名稱，然後點擊新建按鈕即可。

![git github add repository]({{ site.url }}/images/misc/git-github-add-repository.png "git github add repository"){: .pull-center width="95%" }

實際上，新建完成後，會有提示如何導入一個庫。

{% highlight text %}
----- 添加遠端並提交
$ git remote add origin https://github.com/Jin-Yang/foobar.git
$ git push -u origin master             # 第一次導入
$ git push origin master                # 後面提交
{% endhighlight %}

<!--
### 3. clone 代碼庫

{% highlight text %}
$ git clone https://github.com/influxdata/influxdb.git
{% endhighlight %}

### 4. 刪除代碼庫


### git-submodule

{% highlight text %}
git submodule init
git submodule update
{% endhighlight %}
-->



## 參考

Windows 下的客戶端可以參考 [git for windows](https://git-for-windows.github.io/)，Linux 可以使用基於 ncurses 的客戶端 [tig](http://jonas.nitro.dk/tig/) ，也可以直接參考 [github](https://github.com/jonas/tig)；另外，一本不錯介紹 Git 的資料 [Pro Git Book](http://git-scm.com/book/) 。

對於一種不錯的 Git 分支管理模式，也即如上的介紹，可以參考 [A successful Git branching model](http://nvie.com/posts/a-successful-git-branching-model/) 或者 [本地文檔](/reference/misc/A successful Git branching model.mht) 。


<!--

git-filter-branch.sh


https://github.com/git/git

http://www.worldhello.net/gotgithub/index.html
http://www.cnblogs.com/zhangjing230/archive/2012/05/09/2489745.html

http://www.cnblogs.com/ctrlzhang/p/5195079.html

https://www.oschina.net/news/70368/git-advanced-commands

http://blog.csdn.net/wirelessqa/article/details/20152353

https://git-scm.com/book/zh/v1/Git-%E5%9F%BA%E7%A1%80-%E6%92%A4%E6%B6%88%E6%93%8D%E4%BD%9C
-->

{% highlight text %}
{% endhighlight %}
