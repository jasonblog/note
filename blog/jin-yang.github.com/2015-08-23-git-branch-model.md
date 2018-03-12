---
title: Git 分支管理
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: git,版本管理,branch,分支
description: 相比其它的版本管理软件，git 有很多优点，其中很重要的一个特性就是版本的分支 (branch) 和合并 (merge) 十分方便。git 分支并非生成一个物理代码拷贝，而是只生成一个指向当前版本的指针，又被称为 "快照" (snapshot) ，因此，处理起来是分快速，而且节省空间。接下来，就看看 git 的版本分支管理策略。
---

相比其它的版本管理软件，git 有很多优点，其中很重要的一个特性就是版本的分支 (branch) 和合并 (merge) 十分方便。

git 分支并非生成一个物理代码拷贝，而是只生成一个指向当前版本的指针，又被称为 "快照" (snapshot) ，因此，处理起来是分快速，而且节省空间。

接下来，就看看 git 的版本分支管理策略。

<!-- more -->


## 分支

Git 版本的分支 (branch) 和合并 (merge) 十分方便，只生成一个指向当前版本的指针 (称为 "快照")，而非生成一份现有代码的物理拷贝，所以非常快捷易用。

更加详细的内容可以查看 [Git Branching](https://git-scm.com/book/en/v2/Git-Branching-Branches-in-a-Nutshell) 。

### 追踪分支

追踪分支是用与联系本地分支和远程分支，在追踪分支 (Tracking Branches) 上执行 push 或 pull 时，会自动关联到远程分支上；如果要经常从远程仓库里拉取分支到本地，而且想简单使用 `git pull` 命令，那么就应当使用 "追踪分支" 。

可以通过如下命令手动创建一个追踪分支。

{% highlight text %}
$ git branch --track experimental origin/experimental
{% endhighlight %}

当运行 `git pull experimental` 命令时，会自动从 `origin/experimental` fetch 内容，然后再将该分支 merge 进本地的 `experimental` 分支。

当要把修改 push 到 origin 时, 它会将你本地的 `experimental` 分支中的修改推送到 `origin/experimental` 分支里，而且无需指定 origin 。

### fast-forward

当前分支合并到另一分支时，当没分歧解决时就会直接移动文件指针，这就被叫做 fast-forward 。

举例来说，一直在 develop 分支开发，然后新建了一个 feature 分支，并在该分支上进行一系列提交，完成后，回到 develop 分支，此时，如果 develop 分支在创建 feature 分支后从未产生任何新的提交，此时的合并就叫 fast forward 。

**注意**：可以看出这次合并完之后的视图为扁平状，看不出 feature 分支开发的任何信息。

![git develop model no-ff means]({{ site.url }}/images/misc/git-develop-model-no-ff-means.png "git develop model no-ff means"){: .pull-center width="70%" }

另外，可以使用 `--no-ff` (no fast foward) 进行合并，使得每一次的合并都创建一个新的 commit 记录，并强制保留 feature 分支的开发记录，也就告诉后来者 **一系列的提交都是为了同一个目的** 。

{% highlight text %}
[branch "master"]
mergeoptions = --no-commit --no-ff
{% endhighlight %}

如果远程和本地分支的提交线图有分叉，也即不能 fast-forwarded，git 会执行一次 merge 操作，也就是产生一次没意义的提交记录，从而造成提交日志比较混乱。

#### rebase VS. no-ff

pull 时可以使用 `git pull --rebase` 选项，也即当提交线图有分叉的话，git 会用 rebase 策略来代替默认的 merge 策略，可以通过 `man git-merge` 查看详细内容，其好处介绍如下。

假设提交线图在执行 pull 前是这样的：

{% highlight text %}
                 A---B---C  remotes/origin/master
                /
           D---E---F---G  master
{% endhighlight %}

如果是执行 `git pull` 后，提交线图会变成这样：

{% highlight text %}
                 A---B---C remotes/origin/master
                /         \
           D---E---F---G---H master
{% endhighlight %}

结果多出了 H 这个没必要的提交记录，如果执行 `git pull --rebase` 的话，提交线图会变成如下：

{% highlight text %}
                       remotes/origin/master
                           |
           D---E---A---B---C---F'---G'  master
{% endhighlight %}

F G 两个提交通过 rebase 方式重新拼接在 C 之后，多余的分叉去掉了，目的达到。大多数时候，使用 `git pull --rebase` 是为了使提交线图更好看，从而方便 code review 。

<!--
不过，如果你对使用 git 还不是十分熟练的话，我的建议是 git pull --rebase 多练习几次之后再使用，因为 rebase 在 git 中，算得上是『危险行为』。
另外，还需注意的是，使用 git pull --rebase 比直接 pull 容易导致冲突的产生，如果预期冲突比较多的话，建议还是直接 pull。
-->

#### 最佳实践

在合并分支之前，例如要在本地将 feature 分支合并到 dev 分支，会先检查 feature 分支是否落后于远程 dev 分支：

{% highlight text %}
$ git checkout dev
$ git fetch                   ← 更新dev分支，pull<=>fetch+merge
$ git log feature..dev
{% endhighlight %}

如果没有输出任何提交信息的话，即表示 feature 对于 dev 分支是 up-to-date 的，如果有输出的而马上执行了 `git merge --no-ff` 的话，提交线图会变成上图的左侧。

<!--
所以这时在合并前，通常我会先执行：
git checkout feature
git rebase dev
这样就可以将 feature 重新拼接到更新了的 dev 之后，然后就可以合并了，最终得到一个干净舒服的提交线图。
再次提醒：像之前提到的，rebase 是『危险行为』，建议你足够熟悉 git 时才这么做，否则的话是得不偿失啊。
-->

### 常用场景

如上是从现有代码中 clone 并查看分支，进行开发。

{% highlight text %}
----- 1. 克隆代码，可以指定分支或者tag，默认是master
$ git clone -b <branch/tag> https://github.com/master-dev.git

----- 2. 查看所有分支，有master(本地主分支)、origin/master(远程主分支)、其它(如origin/dev)
-----    默认两个master关联，保持同步；由于dev本地没有关联，从而无法在那里开发
$ git branch --all                    ← 查看包括本地以及远程分支
$ git branch                          ← 只查看本地分支

----- 3. 创建本地关联origin/dev的分支
-----    默认本地dev分支的初始代码和远程的dev分支代码一样
$ git checkout dev origin/dev

----- 4. 切换到dev分支进行开发
$ git checkout dev
{% endhighlight %}

接下来看看如何从无到有新建分支，并提交到远端。

{% highlight text %}
----- 完整命令
$ git push <remote-host> <local-branch>:<remote-branch>

----- 1. 创建本地新的hello分支
$ git branch hello                    ← 创建本地分支
$ git branch                          ← 查看本地分支

----- 2. 将hello分支发布到远程仓库
$ git push origin dev:dev

----- 3. 删除远程分支
$ git branch -r -d origin/branch-name
$ git push origin :branch-name
{% endhighlight %}

实际上，第一步创建完本地分支后，该本地分支远程仓库并不知道，此时可以在本地分支开发，然后 merge 到 master ，使用 master 同步代码。

{% highlight text %}
----- 1. 创建本地新的hello分支
$ git branch hello                    ← 创建本地分支

----- 2. 切换到hello分支开发代码
$ git checkout hello

----- 3. 开发完成后，合并主分支
$ git checkout master                 ← 切换到主分支
$ git merge hello                     ← 把hello分支的更改合并到master
$ git push                            ← 提交主分支代码远程

----- 4. 删除本地分支
$ git branch -d hello
{% endhighlight %}

**注意**：在分支切换之前最好先commit全部的改变，除非你真的知道自己在做什么

### 合并冲突

作为分布式版本控制系统，所有修改操作都是基于本地的，在团队协作时，如果同时修改了相同的代码，而你同伴先于你 push 到远端，那么你必须先 pull 做本地合并，然后在 push 到远程。

当在合并的时候，可能会出现代码冲突。

{% highlight text %}
----- 0. 尝试合并
$ git checkout master
$ git merge hello

----- 1. 发生冲突时，将会暂停合并，可以通过如下命令查看冲突文件
$ git status
{% endhighlight %}

当查看冲突的文件时，显示的内容如下。

{% highlight text %}
<<<<<<< HEAD:index.html
<div id="footer">contact : email.support@domain.com</div>
=======
<div id="footer">
 please contact us at support@domain.com
</div>
>>>>>>> foobar:index.html
{% endhighlight %}

其中冲突的两个分支的内容通过 ```======``` 进行分割，解决冲突后，其内容如下。

{% highlight text %}
<div id="footer">
 please contact us at support@domain.com
</div>
{% endhighlight %}

然后，通过 ```git add index.html``` 命令标记下，表示现在已经解决了冲突；当然，也可以使用 `git mergetool` 工具。

#### 其它

{% highlight text %}
$ git branch -v
$ git branch --merged
$ git branch --no-merged

----- 获取某个分支
$ git clone http://github.com/project/foobar.git    # 只能获取默认的分支，一般是master，可以手动设置
$ git branch -a                                     # 查看所有分支
$ git checkout -b dev origin/dev                    # 获取其它分支
{% endhighlight %}


## 经典 branch 工作模式

如下是一个不错的代码管理模式，详细的可以参考 [A successful Git branching model](http://nvie.com/posts/a-successful-git-branching-model/)，也可以查看 [本地文档](/reference/misc/A successful Git branching model.mht) ，如下是一个简单介绍。

![git development model]({{ site.url }}/images/misc/git-develop-model.png "git development model"){: .pull-center width="90%" }

从上图可以看到主要包含下面几个分支，简单介绍如下：

* master: 主分支，用来版本发布，通常
* develop：日常开发分支，保存了开发的最新代码，用于每天的回归测试；
* feature：新的特性或者功能开发分支，只与 develop 分支交互；
* release：预发布分支，在特性开发基本测试完成后，准备发布前，用于发布前最后测试；
* hotfix：线上 bug 修复分支。

接下来详细介绍。

> NOTE: 如下的介绍中有个 bump-version.sh 脚本，该脚本的作用是，将源码中与版本号相关的内容替换为最新的值，然后用于发布。其中 Github 上有个相关的版本，可以参考 [Github bumpversion](https://github.com/peritus/bumpversion) 。

### Main Branches

含两个主分支 master 和 develop ，其中 origin/master 表示主分支， HEAD 始终是用于生产环境的代码。而 origin/develop 表示主开发分支，其中的 HEAD 表示最新提交的代码，这也就是每天用来编译测试的代码。

![git master develop]({{ site.url }}/images/misc/git-master-develop.png "git master develop"){: .pull-center width="50%" }

当开发分支的代码趋于稳定之后，且准备发布时，只需要打个发布的版本号标签 (tag) 即可。

因此，每次提交到主干时，也就意味着这是一个新的生产版本的发布，那么可以通过一个 hook 自动编译，生成生产环境的安装包。

{% highlight text %}
----- 新建一个仓库，默认会有一个master分支
$ git init
$ echo 'Hello World!' > README
$ git add README
$ git commit -m 'Initial Commit'

----- 新建一个develop分支
$ git branch develop
{% endhighlight %}

上述的 main 和 develop 都可以称之为主干分支。

除了主干的分支外，还包括了一些其它分支，区别是这些分支有固定的生命周期，包括了 Feature Branches、Release Branches、Hotfix Branches 三种针对不同的场景，也对应了如何获取分支以及如何合并分支。

如上介绍的三种分支也可以被称为 Support Branches，接下来详细介绍这三种分支。

### Feature Branches

特性分支用来开发一个新的特性，这一分支从 develop 创建，而且最终会合并到 develop 分支；当然，也有可能最终取消掉，这取决于最终产品的决策。

![git develop model feature branches]({{ site.url }}/images/misc/git-develop-model-feature-branches.png "git develop model feature branches"){: .pull-center width="20%" }

接下来看看如何使用。

{% highlight text %}
----- 0. 创建新的特性分支
$ git checkout -b feature-foobar develop

----- 1. 执行一些操作，多次提交
$ echo "FOOBAR Feature 1" >> README
$ git commit -a -m "foobar feature 1"
$ echo "FOOBAR Feature 2" >> README
$ git commit -a -m "foobar feature 2"

----- 2.1 开发完成，接下来准备合并，先切换到develop分支
$ git checkout develop

----- 2.2 合并到develop主分支
$ git merge --no-ff feature-foobar

----- 2.3 删除原来的分支
$ git branch -d feature-foobar

----- 2.4 提交到远程仓库
$ git push origin develop
{% endhighlight %}

另外，需要注意的是，上述合并到主分支的时候，采用的是 \-\-no-ff 模式。

![git develop model no-ff means]({{ site.url }}/images/misc/git-develop-model-no-ff-means.png "git develop model no-ff means"){: .pull-center width="70%" }

该参数的作用是强行关闭 fast-forward 方式，该方式就是当条件允许的时候，git 直接把 HEAD 指针指向合并分支的头，完成合并。不过如果删除分支，由于这个过程中没有创建 commit，则会丢失分支信息，使用该参数将保留分支 commit 历史。

实际上，通过 git log 查看时，使用 \-\-on-ff 会强制添加 commit ，否则看不到合并的信息。

<!--
git merge --squash 是用来把一些不必要commit进行压缩，比如说，你的feature在开发的时候写的commit很乱，那么我们合并的时候不希望把这些历史commit带过来，于是使用--squash进行合并，此时文件已经同合并后一样了，但不移动HEAD，不提交。需要进行一次额外的commit来“总结”一下，然后完成最终的合并。
-->

另外，还有个参数 \-\-squash，会把多次分支 commit 历史压缩为一次。


### Release Branches

用于正式发布前的最后测试，尽量减少 bug 数目，添加元信息 (版本号以及发布日期等)。该分支从 develop 创建，可以合并到 develop 或者 master 分支，其命名为 release-* 。

合并到 master 之后就可以用于发布了，而之所以合并到 develop ，是为了将最新修改合并到新的开发分支。

{% highlight text %}
----- 1. 创建分支，并更新文件一些头部信息等，然后提交
$ git checkout -b release-1.2 develop
$ ./bump-version.sh 1.2
$ git commit -a -m "Bumped version number to 1.2"

----- 2. 测试没有问题后，准备正式发布
$ git checkout master
$ git merge --no-ff release-1.2
$ git tag -a 1.2

----- 3. 删除release分支
$ git checkout develop
$ git merge --no-ff release-1.2
$ git branch -d release-1.2
{% endhighlight %}

### Hotfix Branches

该分支由于修复线上 bug，当线上代码出现 bug 时，从 master 开一个 hotfix 分支，修复 bug 之后再将 hotfix 分支合并到 master 分支并进行发布，同时也需要合并到 develop 分支上去。

![git develop model hotfix branch]({{ site.url }}/images/misc/git-develop-model-hotfix-branch.png "git develop model hotfix branch"){: .pull-center width="50%" }

可以发现，hotfix 和 release 分支功能类似，两者好处是不会打断 develop 分支正常功能开发。

{% highlight text %}
----- 1. 从master获取分支
$ git checkout -b hotfix-1.2.1 master
$ ./bump-version.sh 1.2.1
$ git commit -a -m "Bumped version number to 1.2.1"

----- 2. 然后修改代码修复bug，并提交
$ git commit -m "Fixed severe production problem"

----- 3. bug修复后，将该分支合并到master
$ git checkout master
$ git merge --no-ff hotfix-1.2.1
$ git tag -a 1.2.1

----- 4. 然后将hotfix合并到develop分支
$ git checkout develop
$ git merge --no-ff hotfix-1.2.1

----- 5. 删除hotfix分支
$ git branch -d hotfix-1.2.1
{% endhighlight %}


## git-flow

[git-flow](https://github.com/nvie/gitflow) 提供了很多不错的脚本，来支持上述的这种开发模式，不过它并没有提供重置 (rebase) 特性分支的能力，安装方式可以参考 [gitflow-installation](https://github.com/nvie/gitflow/wiki/Installation) 。

该工具集成了一些常见的命令，每次操作实际都会提示一些操作记录。

### 安装

可以直接从 [github gitflow/contrib/gitflow-installer.sh](https://raw.githubusercontent.com/nvie/gitflow/develop/contrib/gitflow-installer.sh) 下载文件，然后执行如下命令即可。

{% highlight text %}
----- 安装
$ wget --no-check-certificate -q https://raw.githubusercontent.com/.../gitflow-installer.sh
# bash gitflow-installer.sh install stable
$ rm gitflow-installer.sh

----- 删除
# bash gitflow-installer.sh uninstall
{% endhighlight %}

实际上很简单，只是将代码 clone 到本地，然后通过 install 命令安装；当然，也可以离线安装，该模块会依赖于 [github - nvie/shFlags](https://github.com/nvie/shFlags)，直接将该代码下载并放置到源码的 shFlags 目录下即可。

最后，通过 ```make install``` 安装，也可以执行如下步骤，不过需要修改安装脚本的代码，注释掉下载代码的步骤。

{% highlight text %}
$ tar -xf gitflow-x.x.x.tar.gz shFlags-x.x.x.tar.gz
$ mv gitflow-x.x.x gitflow && rm gitflow/shFlags -rf
$ mv shFlags-x.x.x gitflow/shFlags
# bash gitflow/contrib/gitflow-installer.sh
{% endhighlight %}



### 初始化

使用 git-flow 时，需要从初始化一个 git 库开始，其中命令如下。

{% highlight text %}
$ git flow init
No branches exist yet. Base branches must be created now.
Branch name for production releases: [master]
Branch name for "next release" development: [develop]

How to name your supporting branch prefixes?
Feature branches? [feature/]
Release branches? [release/]
Hotfix branches? [hotfix/]
Support branches? [support/]
Version tag prefix? []
{% endhighlight %}

主要是回答几个关于分支的命名约定，建议使用默认值。

对于最后的 tag 选项，可以设置为软件的名称，例如 ```foobar-```，然后 Release Branches 的名称使用类似 ```v1.1.0``` 名称，那么最终的 tag 名即为 ```foobar-v1.1.0``` 。

关于该 tag 的信息可以通过 ```git show foobar-v1.1.0``` 命令查看。

### 新特性

关于新特性的开发流程，为即将发布的版本开发新功能特性，这通常只存在开发者的库中。

#### 增加新特性

新特性的开发是基于 'develop' 分支的，可以通过下面的命令开始开发新特性：

{% highlight text %}
$ git flow feature start MYFEATURE
{% endhighlight %}

这个操作创建了一个基于 'develop' 的特性分支，并切换到这个分支之下。

#### 完成新特性

完成开发新特性之后，执行下面的操作，主要是合并 MYFEATURE 分支到 'develop'，并删除这个新特性分支，然后切换回 'develop' 分支。

{% highlight text %}
$ git flow feature finish MYFEATURE
{% endhighlight %}

#### 发布新特性

主要用于合作开发一新特性，将新特性分支发布到远程服务器，这样其他用户可以使用该分支。

{% highlight text %}
$ git flow feature publish MYFEATURE
{% endhighlight %}

#### 取得新特性分支

主要用于取得其他用户发布的新特性分支，并签出远程的变更，主要有如下的两种方式。

{% highlight text %}
----- 签出特性分支变更
$ git flow feature pull origin MYFEATURE

----- 跟踪特性分支变更
$ git flow feature track MYFEATURE
{% endhighlight %}

### 预发布版本

该分支准备开始发布，用于支持一个新的用于生产环境的发布版本，允许修正小问题 (bug)，但不新增特性，并为发布版本准备元数据。

#### 准备

开始准备预发布版本，它从 'develop' 分支开始创建一个 release 分支。

{% highlight text %}
$ git flow release start RELEASE [BASE]
{% endhighlight %}

可以选择一个 [BASE] 参数，这是 'develop' 分支提交记录的 hash 值。

创建 release 分支之后，可以立即发布到远端，从而允许其它用户向这个 release 分支提交代码，该命令十分类似发布新特性：

{% highlight text %}
----- 提交到远端
$ git flow release publish RELEASE

----- 签出release版本的远程变更
$ git flow release track RELEASE
{% endhighlight %}

#### 完成预发布版本

完成 release 版本分支操作，主要执行下面几个动作：A) 合并 release 分支到 master 分支；B) 用 release 分支名打 tag；C) 归并 release 分支到 develop；D) 移除 release 分支。

{% highlight text %}
$ git flow release finish RELEASE
{% endhighlight %}

### 紧急修复

当生产环境的版本突然发现一个严重 bug 时，需要立即修正；此时，有可能是需要修正 master 分支上某个 tag 标记的生产版本。

#### 开始

像其它 git flow 命令一样, 可以通过如下命令创建一个紧急修复分支。

{% highlight text %}
$ git flow hotfix start VERSION [BASENAME]
{% endhighlight %}

VERSION 参数标记着修正版本，可以从 [BASENAME] 开始，[BASENAME] 为 finish release 时填写的版本号。

#### 完成

当完成紧急修复分支后，代码归并回 develop 和 master 分支，相应地，master 分支打上修正版本的 tag 。

{% highlight text %}
$ git flow hotfix finish VERSION
{% endhighlight %}

### 其它

##### unable to start editor

可以通过 `git config --global core.editor "vim"` 命令设置。

## 参考

关于 Git 的分支介绍可以参考 [git-scm.com/doc](https://git-scm.com/doc) 中的介绍，另外，还有一本不错介绍 Git 的资料 [Pro Git Book](http://git-scm.com/book/)；另外一个不错的文档 [图解Git](http://marklodato.github.io/visual-git-guide/index-zh-cn.html)，或者 [本地文档](/reference/misc/git_graphs.mht) 。

对于一种不错的 Git 分支管理模式，也即如上的介绍，可以参考 [A successful Git branching model](http://nvie.com/posts/a-successful-git-branching-model/) 或者 [本地文档](/reference/misc/A successful Git branching model.mht) 。

<!--
http://www.worldhello.net/gotgithub/index.html

http://www.cnblogs.com/zhangjing230/archive/2012/05/09/2489745.html

http://www.cnblogs.com/ctrlzhang/p/5195079.html

https://www.oschina.net/news/70368/git-advanced-commands

http://blog.csdn.net/wirelessqa/article/details/20152353

git-filter-branch.sh

https://github.com/git/git
-->

{% highlight text %}
{% endhighlight %}
