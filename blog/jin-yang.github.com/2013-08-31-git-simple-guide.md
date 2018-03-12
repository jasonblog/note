---
title: Git 简明教程
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: git
description: 如果你严肃对待编程，就必定会使用"版本管理系统"（Version Control System）。
---

如果你严肃对待编程，就必定会使用"版本管理系统"（Version Control System）。

<!-- more -->

![git logo]({{ site.url }}/images/misc/git-logo.png "git logo"){: .pull-center width="60%" }

## 简介

不像 SVN ，Git 采用的是分布式的管理，简单来说，如下所示。

![git decentralized model]({{ site.url }}/images/misc/git-decentralized-model.png "git decentralized model"){: .pull-center width="60%" }

所有人除了可以从 origin 获取提交最新的代码之外，每个人还可以从不同人的代码库中获取代码，这也就意味着，比如两或多个人可以共同开发一个特性，测试无误之后再提交到 origin 即可。

另外，需要注意的是，所有版本控制系统，只能跟踪文本文件的改动；对于像图片、视频这类的二进制文件，虽能管理，但没法跟踪文件内容的变化，只知道从 1K 变为了 2K 而改变的内容不知道。


### 常见概念

简单介绍下版本开发过程中，经常使用的一些概念。

##### alpha、beta、gamma

用来标识测试的阶段和范围。

* alpha 内测，指开发团队内部测试的版本或者有限用户体验测试版本。
* beta 公测，即针对所有用户公开的测试版本。
* gamma，基本是 beta 做过一些修改后，成为正式发布的候选版本，也称为 RC 。

##### Release Candidate, RC

发行候选版本，和 Beta 版最大的差别在于 Beta 阶段会一直加入新的功能，但是到了 RC 版本，几乎就不会加入新的功能了，而主要着重于除错。

##### General Availability, GA

开发团队认为该版本是稳定版，有的软件可能会标识为 stable 或者 production 版，其意思和 GA 基本相同，也就是官方开始推荐广泛使用了。

##### 包命名

推荐按照 RPM 包的命名方式，也就是 packagename-version-release.arch.rpm 。

* name: 表示包的名称，包括主包名和分包名；
* version: 表示包的版本信息；
* release: 用于标识 rpm 包本身的发行号，可还包含适应的操作系统；
* arch: 表示主机平台，noarch 表示此包能安装到所有平台上面。

如 gd-devel-2.0.35-11.el6.x86_64.rpm ，gd 是这个包的主包名；devel 是这个包的分包名； 2.0.35 是表示版本信息，2 为主版本号，0 表示次版本号，35 为源码包的发行号也叫修订号； 11.el6 中的 11 是 rpm 的发行号， el6 表示 RHEL6； x86_64 是表示适合的平台。


### 常见操作

接下来，先看下一些常见的操作。

{% highlight text %}
----- 1. 创建新仓库
$ git init                             ← 会在.git目录下保存元数据

----- 2. 创建文件，并添加到版本库中
$ echo 'Hello World!' > README         ← 新建文件
$ git add README                       ← 添加到版本库中
$ git ls-files                         ← 查看版本库中的文件
$ git commit -m 'Initial Commit'       ← 提交并添加注释
$ git status                           ← 是否有未提交的文件

----- 3. 修改文件
$ echo 'What A Hell.' >> README        ← 修改文件添加一行
$ git commit -a -m 'Add a new line'    ← 提交下
$ git log --pretty=oneline             ← 查看提交信息

----- 4. 回退
$ echo 'It is A real world' > README   ← 覆盖之前的内容
$ git reset --hard HEAD                ← 回退到之前提交版本，HEAD^(上个) HEAD~N(上N个)
$ git reflog                           ← 查看版本号，貌似是UUID
$ git reset --hard 6fcfc89             ← 回退到指定的版本

----- 4. clone一个副本
$ git clone /tmp/foobar .              ← 从本地复制一个副本
{% endhighlight %}

<!--
如果是远端服务器上的仓库，你的命令会是这个样子：
git clone username@host:/path/to/repository
-->


### 工作区和版本库

在本地会分为三部分，分别是。

1. Working Directory(Tree)，工作目录<br>
    也就是保存当前工作的文件所在的目录，文件会在切换分支时被删除或者替换。

2. GIT Directory (GIT库目录)<br>
    项目所有历史提交都被保存在了GIT库目录中。

3. GIT Index (GIT索引)<br>
    可以看作是工作目录和GIT库目录之间的暂存区，与 `Staging Area` 是相同意思，也就是已经被 ADD 但是尚未被 commit 的内容。

**工作目录** 就是在电脑上看到的目录以及文件 (.git隐藏目录版本库除外)，包括以后需要再新建的目录文件等等都属于工作区范畴。

**版本库** 工作目录下有一个隐藏目录 .git 就是版本库，其中比较重要的就是 stage(暂存区)，还有自动创建了第一个分支 master，以及指向 master 的一个指针 HEAD。

![git stage commit]({{ site.url }}/images/misc/git-stage-commit.png "git stage commit"){: .pull-center width="50%" }

如前所述，提交到版本库包含了两步：

1. 使用 git add 把文件添加到 stage；

2. 使用 git commit 把暂存区的所有内容提交到当前分支上。

关于其中的状态，可以在修改文件后、执行 add 后、执行 commit 后分别通过 status 命令查看文件的不同状态。

也就是说，添加到 stage 区域后，只要未提交，可以多次修改文件并 add 到 stage 区。

### upstream downstream

git 中的 upstream 和 downstream 的概念是相对的，如果本地库中的分支 foo 被 push 到远端中的分支 bar，那么 bar 就是 foo 的 upstream，而 foo 就是 bar 的 downstream 。

相关的配置会保存在本地库的 `.git/config` 文件中。

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

如果当前分支与多个主机存在追踪关系，则可以使用 `-u` 选项指定一个默认主机，这样后面就可以不加任何参数使用 `git push` ，命令如下，在 push 的同时也会指定当前分支的 upstream 。

{% highlight text %}
----- 初次提交本地分支，并不会设置当前本地分支的upstream分支
$ git push origin local-branch:remote-branch

----- 提交的同时，关联本地local-branch分支的upstream分支
$ git push --set-upstream origin local-branch:remote-branch
$ git push -u origin local-branch:remote-branch

----- 只设置分支，为新建的本地分支设置在远程库中的upstream分支
$ git branch --set-upstream-to=origin/remote-branch local-branch
$ git branch --set-upstream local-branch origin/remote-branch        ← 过期
{% endhighlight %}

通过 `git push -u origin master` 将本地的 master 分支推送到 origin 主机后，同时指定 origin 为默认主机，以及指定的远端分支，后面就可以不加任何参数使用 `git push` 了。

<!--
$ git remote add origin ssh://github/foobar/test
now configure master to know to track
# git config branch.master.remote origin
# git config branch.master.merge refs/heads/master
and push
# git push origin master
-->

### 子模块

子模块可用于多项目使用共同类库的工具，允许类库项目做一个库，子项目做为一个单独的 git 项目存在父项目中。

{% highlight text %}
----- 添加submodule仓库
$ git submodule add <remote_url> <local_path>
$ git submodule add -b branch <remote_url> <local_path>
$ git submodule add git@your.gitlab.com:Project/nodus.git Specific/Subdir

----- 查看相关配置信息
$ cat .gitmodules
$ cat .git/config

----- 对于clone一个有submodule的仓库，默认是不会把submodule也clone下来，需要执行如下步骤
----- 1. 注册submodule到.git/config里
$ git submodule init
----- 2. clone submodule
$ git submodule update --recursive
----- 上面两步等价于下面
$ git submodule update --init --recursive

----- 然后可以直接从submodule的库中拉取代码进行测试，也可以指定具体分支
$ git submodule foreach --recursive git fetch origin
$ git submodule add -b branch-name GIT-URL
{% endhighlight %}

如果已经设置了 submodule ，后面做了修改，那么可以直接通过如下步骤操作。

{% highlight text %}
----- 添加子目录，并提交
git add PLUGIN/SUBMODULE/DIRECTORY
git commit -m "Add new sub-module"

----- 查看子模块状态，获取到对应的commit-id
git submodule status

----- 通过上述的commit-id到对应的子模块下查看日志
git log XXXXXXXXXX
{% endhighlight %}

<!--
#### No submodule mapping found in .gitmodules for path

在Stage中存在一个特殊类型的目录，可以通过如下方式进行查看。

git ls-files --stage | grep 160000
git rm --cached <path_to_submodule>

#### 指定分支/TAG

----- 查看当前所有分支信息
cd SUBMODULE_DIRECTORY
git branch -avv
----- 切换到某个远程分支上，添加并提交相关信息
git checkout release/v1.8.4
git add SUBMODULE_DIRECTORY
git commit -m "moved submodule to v1.0"

----- 然后通过如下命令更新git的代码，会通过.submodule中的配置拉取相应的分支
git submodule update
git submodule update --remote

----- 子模块更新后，可以通过如下方式拉去最新的配置，并查看当前状态
git submodule foreach git pull
git submodule status

Read-function of the

// 如果修改了.gitmodule的remote url，使用下面的命令更新submodule的remote url
git submodule sync
git submodule update --init --recursive

1.) Delete the relevant section from the .gitmodules file. You can use below command:

----- 1.1 删除.gitmodules中的配置，默认submodule_name是路径名；也可以手动删除
git config -f .gitmodules --remove-section "submodule.SUBMODULE_NAME"
----- 1.2 删除.git/config中的相关配置

----- 1. 删除.git/config中的配置，同上两步
git submodule deinit -f "SUBMODULE_NAME"

----- 2 保存.gitmodules的配置
git add .gitmodules

----- 3. 删除gitlink，注意没有/
git rm --cached PATH_TO_SUBMODULE

----- 4. 清理.git/modules模块
rm -rf .git/modules/PATH_TO_SUBMODULE

----- 5. 此时可以提交保存一份数据
git commit -m "Removed submodule <name>"

----- 6. 删除相关的文件
rm -rf PATH_TO_SUBMODULE

fatal: Not a git repository

有两个文件保存了 submodule 的绝对路径，当移动了路径之后，如下的内容就需要进行修改。

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

总共有三个配置文件：`/etc/gitconfig`、`~/.gitconfig`、`.git/config` ，其优先级依次递增，后者的配置会覆盖前面的配置项。

#### 全局配置

简单来说就是 `git config --system` 命令，可以添加了一个 system 参数，配置内容保存在 `/etc/gitconfig` 文件中，如下是设置 st 简化命令。

{% highlight text %}
$ git config --system alias.st status     # git st
{% endhighlight %}

#### 用户配置

执行 `git config` 会修改 `~/.gitconfig` 文件的内容。

{% highlight text %}
----- 设置用户的默认用户名和密码
$ git config --global user.name   "Your Name"
$ git config --global user.email  you@example.com
$ git config --global core.editor vim
$ git config --global merge.tool  vimdiff 
$ git config --list
$ git commit --amend --reset-author                 ← 未提交远端的可以进行修复

----- 开启颜色显示
$ git config --global color.ui true
{% endhighlight %}

#### 工作目录配置

进入工作根目录，运行 `git config -e`，这样就只会修改工作区的 `.git/config` 文件。


## 常见命令参考

在 clone 时会自动在本地分支和远程分支之间，建立一种追踪关系 (tracking)；例如，在通过 git clone 从远程库中复制代码库时，会自动将本地的 master 分支与 origin/master 分支对应。

![git operation]({{ site.url }}/images/misc/git-operatoins.jpg "git operations"){: .pull-center width="80%" }

当然，也可以手动建立追踪关系，如下命令指定本地 master 分支追踪远程的 origin/next 分支。

{% highlight text %}
----- 手动设置上游
$ git branch --set-upstream master origin/next

----- 查看origin远端的信息，包括哪些远程分支信息可能过期
$ git remote show origin
----- 删除origin远程过期的分支
$ git remote prune origin
{% endhighlight %}

### git pull

取回远程主机某个分支的更新，然后再与本地的指定分支合并，其完整格式以及常见命令如下。

{% highlight text %}
----- 完整命令
$ git pull <remote-host> <remote-branch>:<local-branch>

----- 取回origin主机的next分支，并与本地的master分支合并
$ git pull origin next:master        ← origin配置在.git/config文件中

----- 如果远程分支是与当前分支合并，则冒号后面的部分可以省略
$ git pull origin next               ← 等同于如下的两个命令
$ git fetch origin
$ git merge origin/next

----- 如果当前分支与远程分支存在追踪关系，git pull就可以省略远程分支名
$ git pull origin

----- 如果当前分支只有一个追踪分支，连远程主机名都可以省略。
$ git pull
{% endhighlight %}

<!--
如果合并需要采用rebase模式，可以使用--rebase选项。
$ git pull --rebase <远程主机名> <远程分支名>:<本地分支名>
-->

### git push

该命令用于将本地分支的更新推送到远程主机，其格式与 `git pull` 相仿，只是分支方向相反。

{% highlight text %}
----- 完整命令
$ git push <remote-host> <local-branch>:<remote-branch>

----- 将本地的master分支推送到origin主机的master分支；后者不存在，则会被新建
$ git push origin master

----- 省略本地分支名，则表示删除指定的远程分支
$ git push origin :master             ← 删除origin主机的master分支，等同于
$ git push origin --delete master

----- 如果当前分支与远程分支之间存在追踪关系，则本地分支和远程分支都可以省略
$ git push origin                     ← 将当前分支推送到origin主机的对应分支

----- 如果当前分支只有一个追踪分支，那么主机名都可以省略。
$ git push

{% endhighlight %}

#### simple & matching

<!--
Git push与pull的默认行为
http://blog.angular.in/git-pushmo-ren-fen-zhi/
-->

在 git 全局配置中，有个 `push.default` 属性决定了 `git push` 操作的默认行为，在 2.0 之前，默认为 `'matching'`，2.0 之后则被更改为了 `'simple'`。除此之外，还有如下的几个配置项：

* nothing<br>push操作无效，除非显式指定远程分支，例如 git push origin develop。
* current<br>push当前分支到远程同名分支，如果远程同名分支不存在则自动创建同名分支。
* upstream<br>把当前分支推送到远程跟踪分支(upstream)，远程跟踪分支必须存在，但是不必跟当前分支同名；常用于从本地分支push/pull到同一远程仓库的情景，这种模式叫做central workflow。
* simple<br>simple和upstream是相似的，只是必须保证本地分支和它的远程upstream分支同名且存在，否则会拒绝push操作。
* matching<br>push所有本地和远程两端都存在的同名分支。

简单来说，对于不带任何参数的 `git push` 命令；如果只推送当前分支，则称之为 simple方式；如果推送所有远程分支的对应本地分支，则为 matching，可以通过如下方式设置。

{% highlight text %}
$ git config --global push.default matching
$ git config --global push.default simple
{% endhighlight %}

另外，还有一种情况，就是不管是否存在对应的远程分支，都将本地的所有分支都推送到远程主机，这时就需要使用 \-\-all 选项。

{% highlight text %}
$ git push --all origin
{% endhighlight %}

上面命令表示，将所有本地分支都推送到 origin 主机。

### git log

通过 `git log` 中的两个高级用法 (A:自定义提交信息的输出格式；B:过滤提交信息)，基本上就可以找到项目中需要的任何信息 (分支、标签、HEAD、提交历史)。

{% highlight text %}
--oneline
  把每一个提交压缩到了一行中，不过包含分支的信息；
--decorate
  显示时添加分支以及 tag 信息，可以看到有哪些分支或者设备指向了提交记录；

<<<<<<<< 查看diff信息
--stat/-p
  查看每次提交时代码的文件修改量，通常用于查看概览信息比较有用，+ - 分别表示提交文件的增删修改比例；
  注意，由于后者用于查看每个文件修改的详细信息，如果修改代码比较多那么现实内容会比较大；

<<<<<<<< 按照用户分类
shortlog
  按照提交用户分类，很容易显示哪些用户提交了哪些内容，默认是按照用户ID排序，可以通过-n按照提交量排序。
  git shortlog --format='%H|%cn|%s'

<<<<<<<< 查看分支历史
--graph
  通过一个ASCII图像来展示提交历史的分支结构，可以和--oneline、--decorate选项一起使用，如下所示：
$ git log --graph --oneline --decorate
*   0e25143 (HEAD, master) Merge branch 'feature'
|\
| * 16b36c6 Fix a bug in the new feature
| * 23ad9ad Start a new feature
* | ad8621a Fix a critical security issue
|/
* 400e4b7 Fix typos in the documentation
* 160e224 Add the initial code base
  星号表明这个提交所在的分支，所以上图的意思是23ad9ad和16b36c6这两个提交在topic分支上，其余的在master
  分支上。对于复杂项目可以通过gitk或SourceTree分析。

<<<<<<<< 过滤历史
----- 显示最近提交的3次commit记录
$ git log -3
----- 指定时间范围(也可以使用1 week ago、yesterday)，注意--since、--until和--after、--before
$ git log --after="2014-7-1" --before="2014-7-4"
----- 按照作者过滤，可以使用正则表达式，同时会匹配邮箱
$ git log --author="John\|Mary"
----- 按照提交信息过滤
$ git log --grep="JRA-224:"
{% endhighlight %}

#### 自定义格式

对于其它的 `git log` 格式需求，可以使用 `--pretty=format:"<string>"` 选项配置，通过不同的占位符替换相关的信息，详细可以查看 `man git-show` 。

{% highlight text %}
----- 简单示例
$ git log --pretty=format:"%cn committed %h on %cd"
{% endhighlight %}

如果定制了一个输出方案，可保存到 `git config`，或设置 `alias` 以便日后使用，在 `~/.gitconfig` 中加入:

{% highlight text %}
[alias]
    lg = log --graph
{% endhighlight %}

或者运行 `git config --global alias.lg "log --graph"` 。

<!--
git log --graph --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s %Cgreen(%cr)%Creset' --abbrev-commit --date=relative
git log --pretty=format:\"%h %ad | %s%d [%an]\" --graph --date=short
https://github.com/geeeeeeeeek/git-recipes/wiki/5.3-Git-log%E9%AB%98%E7%BA%A7%E7%94%A8%E6%B3%95

如果你的工作流区分提交者和作者，--committer也能以相同的方式使用。
-->

### git tag

标签可对某一时间点的版本做标记，常用于版本发布；分为两种类型：轻量标签和附注标签，前者指向提交对象的引用，附注标签则是仓库中的一个独立对象，建议使用附注标签。

{% highlight text %}
----- 查看标签，可以查看所有或者正则表达式过滤
$ git tag
$ git tag -l 'v0.1.*'

----- 打标签，分别为轻量标签以及创建附注标签，其中-a表示annotated，也可以指定版本
$ git tag v0.1.2-light
$ git tag -a v0.1.2 -m "发布0.1.2版本"
$ git tag -a v0.1.1 9fbc3d0

----- 切换标签或者分支，两者命令相同
$ git checkout [tagname|branch]

----- 查看标签的版本信息
$ git show v0.1.2

----- 删除标签，误操作需要删除后重新添加
$ git tag -d v0.1.2

----- 标签发布，默认push不会将标签提交到git服务器，需要显示操作，可以提交单个或者所有的
$ git push origin v0.1.2
$ git push origin --tags
{% endhighlight %}

### git stash

<!--
Git 工具 - 储藏与清理
https://git-scm.com/book/zh/v2/Git-%E5%B7%A5%E5%85%B7-%E5%82%A8%E8%97%8F%E4%B8%8E%E6%B8%85%E7%90%86#_git_stashing
-->

在做了一堆的修改之后，突然有另外的任务要做，切换分支时就会有问题，那么此时可以通过该命令暂存，完成工作后恢复。

{% highlight text %}
$ git stash save "message ..."       ← 保存，不带子命令的默认值  
$ git stash apply stash@{0}          ← 默认应用第一个，注意，此时不会删除保存的stash
$ git stash drop stash@{0}           ← 手动删除
$ git stash list                     ← 查看所有的stash
$ git stash pop [--index] [<stash>]  ← 恢复最新的保存，并从列表中删除
$ git stash clear                    ← 删除所有存储的进度
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

在引用时，可以使用类似如下的方式 ```stash@{0}```、```stash@{2.hours.ago}``` 。

### git revert reset

`git revert` 生成一个新的提交来撤销某次提交，此次提交之前的 commit 都会被保留；`git reset` 回到某次提交，提交及之前的 commit 都会被保留，但是此次之后的修改都会被退回到暂存区。

如下，假设已经通过三次提交保存了三个文件。

{% highlight text %}
$ git log --pretty=oneline
commit3: add file3.txt
commit2: add file2.txt
commit1: add file1.txt

----- 通过revert撤销一次修改，实际上是提交了一次反向的提交
$ git revert HEAD~1
$ git log --pretty=oneline
commit4: Revert "add file3.txt"
commit3: add file3.txt
commit2: add file2.txt
commit1: add file1.txt
{% endhighlight %}

通过 reset 撤销上次提交时有三个参数，分别针对 `working tree` 和 `index` 和 `HEAD` 进行重置：

* soft：只回退 commit 信息，不恢复到 index file 一级，如果还要提交，直接 commit 即可；
* mixed：默认方式，回退到某个版本，只保留源码，回退 commit 和 index 信息；
* hard：彻底回退到某个版本，本地的源码也会变为上一个版本的内容。

{% highlight text %}
----- 通过soft回退，回退提交信息，文件回到stage区，不会删除
$ git reset --soft HEAD~1
commit2: add file2.txt
commit1: add file1.txt
$ ls
file1.txt file2.txt file3.txt
$ git status                       ← file3.txt回退到stage区
# On branch master
# Changes to be committed:
#       new file:   file3.txt

----- 通过mixed回退，回退提交信息，文件回到工作区，不会删除
$ git reset --mixed HEAD~1
commit2: add file2.txt
commit1: add file1.txt
$ ls
file1.txt file2.txt file3.txt
$ git status                       ← file3.txt回退到工作区
# On branch master
# Untracked files:
#       file3.txt

----- 通过hard回退，回退提交信息，文件被删除
$ git reset --hard HEAD~1
HEAD is now at c518422 add file2.txt
commit2: add file2.txt
commit1: add file1.txt
$ ls
file1.txt file2.txt
$ git status                       ← file3.txt被删除
# On branch master
nothing to commit, working directory clean
{% endhighlight %}

### git rm

主要介绍 `git rm --cached` 的操作，其原理是删除 stage 和 repository 中对应文件，但是不会删除 working directory 中的文件，由于已经从 repository 刪除，那么文件会从 tracked 变成 untracked 。

### 撤销操作

经常会有操作错误的情况，简单介绍常见的撤销操作。

#### 修改最后一次提交

有时候我们提交完了才发现漏掉了几个文件没有加，或者提交信息写错了。想要撤消刚才的提交操作，可以使用 `--amend` 选项重新提交：

{% highlight text %}
$ git commit --amend
{% endhighlight %}

此命令将使用当前的暂存区域快照提交，如果刚才提交完没有作任何改动，相当于有机会重新编辑提交说明。

如果刚才提交时忘了暂存某些修改，可以先补上暂存操作，然后再运行 `--amend` 提交：

{% highlight text %}
$ git commit -m 'initial commit'
$ git add forgotten_file
$ git commit --amend
{% endhighlight %}

上面的三条命令最终只是产生一个提交，第二个提交命令修正了第一个的提交内容。

#### 取消已经暂存的文件

已经通过 add 添加到了 staged 中，但是想分开提交，那么可以通过如下方式操作。

{% highlight text %}
----- 查看状态，实际在提示中已经有相关的命令，
$ git status
On branch master
Changes to be committed:
  (use "git reset HEAD <file>..." to unstage)

        modified:   README.txt
        modified:   benchmarks.rb
----- 撤销一个文件的缓存操作，如果不指定文件则会撤销所有的
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

#### 取消对文件的修改

在修改了一个文件之后，突然发现修改完全没有必要，可以通过如下方式撤销；实际上，在 `git status` 中已经有了提示。

{% highlight text %}
$ git checkout -- benchmarks.rb
$ git status
On branch master
Changes to be committed:
        modified:   README.txt
{% endhighlight %}

注意，这条命令是直接撤消了之前的修改，而且无法恢复，所以执行前一定要确认确实不需要了。

另外，任何已经提交到 git 的都可以被恢复，即便在已经删除分支中的提交，或者用 `--amend` 重新改写的提交；可能失去的数据，仅限于没有提交过的，因为对 git 来说它们就像从未存在过一样。

### 合并冲突

在对分支进行合并的时候，难免会发生冲突，可以通过如下方式设置。

{% highlight text %}
----- 获取分支代码
$ git fetch origin
$ git checkout -b branch origin/branch

----- 切换到开发分支，准备合并代码
$ git checkout develop
$ git merge --no-ff branch

----- 查找冲突，一般类似与如下的代码，修改冲突代码
<<<<<<< HEAD
foobar
=======
conflict
>>>>>>> 6853e5ff961e684d3a6c02d4d06183b5ff330dcc

----- 提交修复后的代码
$ git add src/conflict.c
$ git commit -m "fix the confict"
$ git push origin develop
{% endhighlight %}

### 版本号

通过 `git describe` 可以输出类似如下的内容。

{% highlight text %}
$ git describe
v1.8.3.1-8-e1f4155b
{% endhighlight %}

这里告诉你当前的版本号，也就是距离 `v1.8.3.1` 这个 tag 的第 8 次提交，其提交的 SHA 是 `e1f4155b` 。

也可以通过如下方式设置。

{% highlight text %}
$ git describe --match "v[0-9]*" --abbrev=12 HEAD
{% endhighlight %}



### 其它

{% highlight text %}
$ git rev-parse --abbrev-ref HEAD            ← 当前版本

$ git show <HASHID>                          ← 某次提交的文件具体修改内容
$ git show <HASHID> file                     ← 某次某个文件的修改内容
$ git show <HASHID> --stat                   ← 修改了那些文件的统计
$ git diff --name-status HEAD~2 HEAD~3       ← 标记修改了的文件
$ git log -p <FILENAME>                      ← 某个文件修改历史

----- 查看当前目录每个文件的最后提交者
git ls-tree -r --name-only HEAD | while read filename; do
  echo "$(git log -1 --format="%an %ae" -- $filename) $filename"
done

----- 同步tags
$ git fetch origin --prune --tags            ← 1.7之后版本
$ git tag -l | xargs git tag -d              ← 1.7之前版本需要删除后重新获取
$ git fetch

----- 同步branches，更新远端已经删除的分支，以及手动删除本地分支
$ git fetch --prune
$ git branch -D local-branch

----- 设置跟踪远程分支
$ git checkout --track origin/serverfix

----- 设置代理
$ git config --global http.proxy 'http://user-name:user-password@proxy.foobar.com:8080'
{% endhighlight %}

#### 更新单个文件

可以通过如下命令更新单个文件。

{% highlight text %}
----- 获取最新的版本，不过不会添加到工作区
$ git fetch

----- 通过下载的最新版本更新单个文件
$ git checkout origin/master -- path/to/file
{% endhighlight %}


<!--
如果远程主机的版本比本地版本更新，推送时Git会报错，要求先在本地做git pull合并差异，然后再推送到远程主机。这时，如果你一定要推送，可以使用–force选项。

$ git push --force origin

上面命令使用–force选项，结果导致在远程主机产生一个”非直进式”的合并(non-fast-forward merge)。除非你很确定要这样做，否则应该尽量避免使用–force选项。

最后，git push不会推送标签(tag)，除非使用–tags选项。

$ git push origin --tags
-->




<!--
### 彻底删除文件

假设不小把一个大文件或者密码等信息不小心添加到了版本库，我们希望把它从版本库中永久删除不留痕迹，不仅要让它在版本历史里看不出来，还要把它占用的空间也释放出来。

简单来说，可以通过 git filter-branch 命令永久删除一些不需要的信息，对应了 git-filter-branch.sh 脚本，可以参考相应的参数 。

通过如下的示例参考。

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


折腾了半天，还是无法上传，于是，整个命令出来了：

    --tree-filter表示修改文件列表。
    --msg-filter表示修改提交信息，原提交信息从标准输入读入，新提交信息输出到标准输出。
    --prune-empty表示如果修改后的提交为空则扔掉不要。在一次试运行中我发现虽然文件被删除了，但是还剩下个空的提交，就查了下 man 文档，找到了这个选项。
    -f是忽略备份。不加这个选项第二次运行这个命令时会出错，意思是 git 上次做了备份，现在再要运行的话得处理掉上次的备份。
    --all是针对所有的分支。

试运行了几次，看到 40 多次提交逐一被重写，然后检查下，发现要删除的文件确实被删除了。于是高兴地到 github 建立新仓库，并上传了。


OK，这个文件已经完完全全删掉了，版本库已经不再占用空间了。
-->




## GitHub

关于使用 github 相关的内容。

### 1. 添加公钥(可选)

首先，生成一对密钥对，其中公钥需要添加到 Github 中，而私钥必须要保存好。

{% highlight text %}
----- 生成一对密钥，建议输入保护私钥密码
$ ssh-keygen -t rsa -f ~/.ssh/id_rsa_github -C "jinyang.sia@gmail.com"
{% endhighlight %}

按照如下步骤，将公钥添加到 Github 中。

![git github add sshkey]({{ site.url }}/images/misc/git-github-add-sshkey.png "git github add sshkey"){: .pull-center width="95%" }

### 2. 新建远程库

这个很简单，只需要添加个库名称，然后点击新建按钮即可。

![git github add repository]({{ site.url }}/images/misc/git-github-add-repository.png "git github add repository"){: .pull-center width="95%" }

实际上，新建完成后，会有提示如何导入一个库。

{% highlight text %}
----- 添加远端并提交
$ git remote add origin https://github.com/Jin-Yang/foobar.git
$ git push -u origin master             # 第一次导入
$ git push origin master                # 后面提交
{% endhighlight %}

<!--
### 3. clone 代码库

{% highlight text %}
$ git clone https://github.com/influxdata/influxdb.git
{% endhighlight %}

### 4. 删除代码库


### git-submodule

{% highlight text %}
git submodule init
git submodule update
{% endhighlight %}
-->



## 参考

Windows 下的客户端可以参考 [git for windows](https://git-for-windows.github.io/)，Linux 可以使用基于 ncurses 的客户端 [tig](http://jonas.nitro.dk/tig/) ，也可以直接参考 [github](https://github.com/jonas/tig)；另外，一本不错介绍 Git 的资料 [Pro Git Book](http://git-scm.com/book/) 。

对于一种不错的 Git 分支管理模式，也即如上的介绍，可以参考 [A successful Git branching model](http://nvie.com/posts/a-successful-git-branching-model/) 或者 [本地文档](/reference/misc/A successful Git branching model.mht) 。


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
