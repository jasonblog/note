---
title: Git 分支管理
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: git,版本管理,branch,分支
description: 相比其它的版本管理軟件，git 有很多優點，其中很重要的一個特性就是版本的分支 (branch) 和合並 (merge) 十分方便。git 分支並非生成一個物理代碼拷貝，而是隻生成一個指向當前版本的指針，又被稱為 "快照" (snapshot) ，因此，處理起來是分快速，而且節省空間。接下來，就看看 git 的版本分支管理策略。
---

相比其它的版本管理軟件，git 有很多優點，其中很重要的一個特性就是版本的分支 (branch) 和合並 (merge) 十分方便。

git 分支並非生成一個物理代碼拷貝，而是隻生成一個指向當前版本的指針，又被稱為 "快照" (snapshot) ，因此，處理起來是分快速，而且節省空間。

接下來，就看看 git 的版本分支管理策略。

<!-- more -->


## 分支

Git 版本的分支 (branch) 和合並 (merge) 十分方便，只生成一個指向當前版本的指針 (稱為 "快照")，而非生成一份現有代碼的物理拷貝，所以非常快捷易用。

更加詳細的內容可以查看 [Git Branching](https://git-scm.com/book/en/v2/Git-Branching-Branches-in-a-Nutshell) 。

### 追蹤分支

追蹤分支是用與聯繫本地分支和遠程分支，在追蹤分支 (Tracking Branches) 上執行 push 或 pull 時，會自動關聯到遠程分支上；如果要經常從遠程倉庫里拉取分支到本地，而且想簡單使用 `git pull` 命令，那麼就應當使用 "追蹤分支" 。

可以通過如下命令手動創建一個追蹤分支。

{% highlight text %}
$ git branch --track experimental origin/experimental
{% endhighlight %}

當運行 `git pull experimental` 命令時，會自動從 `origin/experimental` fetch 內容，然後再將該分支 merge 進本地的 `experimental` 分支。

當要把修改 push 到 origin 時, 它會將你本地的 `experimental` 分支中的修改推送到 `origin/experimental` 分支裡，而且無需指定 origin 。

### fast-forward

當前分支合併到另一分支時，當沒分歧解決時就會直接移動文件指針，這就被叫做 fast-forward 。

舉例來說，一直在 develop 分支開發，然後新建了一個 feature 分支，並在該分支上進行一系列提交，完成後，回到 develop 分支，此時，如果 develop 分支在創建 feature 分支後從未產生任何新的提交，此時的合併就叫 fast forward 。

**注意**：可以看出這次合併完之後的視圖為扁平狀，看不出 feature 分支開發的任何信息。

![git develop model no-ff means]({{ site.url }}/images/misc/git-develop-model-no-ff-means.png "git develop model no-ff means"){: .pull-center width="70%" }

另外，可以使用 `--no-ff` (no fast foward) 進行合併，使得每一次的合併都創建一個新的 commit 記錄，並強制保留 feature 分支的開發記錄，也就告訴後來者 **一系列的提交都是為了同一個目的** 。

{% highlight text %}
[branch "master"]
mergeoptions = --no-commit --no-ff
{% endhighlight %}

如果遠程和本地分支的提交線圖有分叉，也即不能 fast-forwarded，git 會執行一次 merge 操作，也就是產生一次沒意義的提交記錄，從而造成提交日誌比較混亂。

#### rebase VS. no-ff

pull 時可以使用 `git pull --rebase` 選項，也即當提交線圖有分叉的話，git 會用 rebase 策略來代替默認的 merge 策略，可以通過 `man git-merge` 查看詳細內容，其好處介紹如下。

假設提交線圖在執行 pull 前是這樣的：

{% highlight text %}
                 A---B---C  remotes/origin/master
                /
           D---E---F---G  master
{% endhighlight %}

如果是執行 `git pull` 後，提交線圖會變成這樣：

{% highlight text %}
                 A---B---C remotes/origin/master
                /         \
           D---E---F---G---H master
{% endhighlight %}

結果多出了 H 這個沒必要的提交記錄，如果執行 `git pull --rebase` 的話，提交線圖會變成如下：

{% highlight text %}
                       remotes/origin/master
                           |
           D---E---A---B---C---F'---G'  master
{% endhighlight %}

F G 兩個提交通過 rebase 方式重新拼接在 C 之後，多餘的分叉去掉了，目的達到。大多數時候，使用 `git pull --rebase` 是為了使提交線圖更好看，從而方便 code review 。

<!--
不過，如果你對使用 git 還不是十分熟練的話，我的建議是 git pull --rebase 多練習幾次之後再使用，因為 rebase 在 git 中，算得上是『危險行為』。
另外，還需注意的是，使用 git pull --rebase 比直接 pull 容易導致衝突的產生，如果預期衝突比較多的話，建議還是直接 pull。
-->

#### 最佳實踐

在合併分支之前，例如要在本地將 feature 分支合併到 dev 分支，會先檢查 feature 分支是否落後於遠程 dev 分支：

{% highlight text %}
$ git checkout dev
$ git fetch                   ← 更新dev分支，pull<=>fetch+merge
$ git log feature..dev
{% endhighlight %}

如果沒有輸出任何提交信息的話，即表示 feature 對於 dev 分支是 up-to-date 的，如果有輸出的而馬上執行了 `git merge --no-ff` 的話，提交線圖會變成上圖的左側。

<!--
所以這時在合併前，通常我會先執行：
git checkout feature
git rebase dev
這樣就可以將 feature 重新拼接到更新了的 dev 之後，然後就可以合併了，最終得到一個乾淨舒服的提交線圖。
再次提醒：像之前提到的，rebase 是『危險行為』，建議你足夠熟悉 git 時才這麼做，否則的話是得不償失啊。
-->

### 常用場景

如上是從現有代碼中 clone 並查看分支，進行開發。

{% highlight text %}
----- 1. 克隆代碼，可以指定分支或者tag，默認是master
$ git clone -b <branch/tag> https://github.com/master-dev.git

----- 2. 查看所有分支，有master(本地主分支)、origin/master(遠程主分支)、其它(如origin/dev)
-----    默認兩個master關聯，保持同步；由於dev本地沒有關聯，從而無法在那裡開發
$ git branch --all                    ← 查看包括本地以及遠程分支
$ git branch                          ← 只查看本地分支

----- 3. 創建本地關聯origin/dev的分支
-----    默認本地dev分支的初始代碼和遠程的dev分支代碼一樣
$ git checkout dev origin/dev

----- 4. 切換到dev分支進行開發
$ git checkout dev
{% endhighlight %}

接下來看看如何從無到有新建分支，並提交到遠端。

{% highlight text %}
----- 完整命令
$ git push <remote-host> <local-branch>:<remote-branch>

----- 1. 創建本地新的hello分支
$ git branch hello                    ← 創建本地分支
$ git branch                          ← 查看本地分支

----- 2. 將hello分支發佈到遠程倉庫
$ git push origin dev:dev

----- 3. 刪除遠程分支
$ git branch -r -d origin/branch-name
$ git push origin :branch-name
{% endhighlight %}

實際上，第一步創建完本地分支後，該本地分支遠程倉庫並不知道，此時可以在本地分支開發，然後 merge 到 master ，使用 master 同步代碼。

{% highlight text %}
----- 1. 創建本地新的hello分支
$ git branch hello                    ← 創建本地分支

----- 2. 切換到hello分支開發代碼
$ git checkout hello

----- 3. 開發完成後，合併主分支
$ git checkout master                 ← 切換到主分支
$ git merge hello                     ← 把hello分支的更改合併到master
$ git push                            ← 提交主分支代碼遠程

----- 4. 刪除本地分支
$ git branch -d hello
{% endhighlight %}

**注意**：在分支切換之前最好先commit全部的改變，除非你真的知道自己在做什麼

### 合併衝突

作為分佈式版本控制系統，所有修改操作都是基於本地的，在團隊協作時，如果同時修改了相同的代碼，而你同伴先於你 push 到遠端，那麼你必須先 pull 做本地合併，然後在 push 到遠程。

當在合併的時候，可能會出現代碼衝突。

{% highlight text %}
----- 0. 嘗試合併
$ git checkout master
$ git merge hello

----- 1. 發生衝突時，將會暫停合併，可以通過如下命令查看衝突文件
$ git status
{% endhighlight %}

當查看衝突的文件時，顯示的內容如下。

{% highlight text %}
<<<<<<< HEAD:index.html
<div id="footer">contact : email.support@domain.com</div>
=======
<div id="footer">
 please contact us at support@domain.com
</div>
>>>>>>> foobar:index.html
{% endhighlight %}

其中衝突的兩個分支的內容通過 ```======``` 進行分割，解決衝突後，其內容如下。

{% highlight text %}
<div id="footer">
 please contact us at support@domain.com
</div>
{% endhighlight %}

然後，通過 ```git add index.html``` 命令標記下，表示現在已經解決了衝突；當然，也可以使用 `git mergetool` 工具。

#### 其它

{% highlight text %}
$ git branch -v
$ git branch --merged
$ git branch --no-merged

----- 獲取某個分支
$ git clone http://github.com/project/foobar.git    # 只能獲取默認的分支，一般是master，可以手動設置
$ git branch -a                                     # 查看所有分支
$ git checkout -b dev origin/dev                    # 獲取其它分支
{% endhighlight %}


## 經典 branch 工作模式

如下是一個不錯的代碼管理模式，詳細的可以參考 [A successful Git branching model](http://nvie.com/posts/a-successful-git-branching-model/)，也可以查看 [本地文檔](/reference/misc/A successful Git branching model.mht) ，如下是一個簡單介紹。

![git development model]({{ site.url }}/images/misc/git-develop-model.png "git development model"){: .pull-center width="90%" }

從上圖可以看到主要包含下面幾個分支，簡單介紹如下：

* master: 主分支，用來版本發佈，通常
* develop：日常開發分支，保存了開發的最新代碼，用於每天的迴歸測試；
* feature：新的特性或者功能開發分支，只與 develop 分支交互；
* release：預發佈分支，在特性開發基本測試完成後，準備發佈前，用於發佈前最後測試；
* hotfix：線上 bug 修復分支。

接下來詳細介紹。

> NOTE: 如下的介紹中有個 bump-version.sh 腳本，該腳本的作用是，將源碼中與版本號相關的內容替換為最新的值，然後用於發佈。其中 Github 上有個相關的版本，可以參考 [Github bumpversion](https://github.com/peritus/bumpversion) 。

### Main Branches

含兩個主分支 master 和 develop ，其中 origin/master 表示主分支， HEAD 始終是用於生產環境的代碼。而 origin/develop 表示主開發分支，其中的 HEAD 表示最新提交的代碼，這也就是每天用來編譯測試的代碼。

![git master develop]({{ site.url }}/images/misc/git-master-develop.png "git master develop"){: .pull-center width="50%" }

當開發分支的代碼趨於穩定之後，且準備發佈時，只需要打個發佈的版本號標籤 (tag) 即可。

因此，每次提交到主幹時，也就意味著這是一個新的生產版本的發佈，那麼可以通過一個 hook 自動編譯，生成生產環境的安裝包。

{% highlight text %}
----- 新建一個倉庫，默認會有一個master分支
$ git init
$ echo 'Hello World!' > README
$ git add README
$ git commit -m 'Initial Commit'

----- 新建一個develop分支
$ git branch develop
{% endhighlight %}

上述的 main 和 develop 都可以稱之為主幹分支。

除了主幹的分支外，還包括了一些其它分支，區別是這些分支有固定的生命週期，包括了 Feature Branches、Release Branches、Hotfix Branches 三種針對不同的場景，也對應瞭如何獲取分支以及如何合併分支。

如上介紹的三種分支也可以被稱為 Support Branches，接下來詳細介紹這三種分支。

### Feature Branches

特性分支用來開發一個新的特性，這一分支從 develop 創建，而且最終會合併到 develop 分支；當然，也有可能最終取消掉，這取決於最終產品的決策。

![git develop model feature branches]({{ site.url }}/images/misc/git-develop-model-feature-branches.png "git develop model feature branches"){: .pull-center width="20%" }

接下來看看如何使用。

{% highlight text %}
----- 0. 創建新的特性分支
$ git checkout -b feature-foobar develop

----- 1. 執行一些操作，多次提交
$ echo "FOOBAR Feature 1" >> README
$ git commit -a -m "foobar feature 1"
$ echo "FOOBAR Feature 2" >> README
$ git commit -a -m "foobar feature 2"

----- 2.1 開發完成，接下來準備合併，先切換到develop分支
$ git checkout develop

----- 2.2 合併到develop主分支
$ git merge --no-ff feature-foobar

----- 2.3 刪除原來的分支
$ git branch -d feature-foobar

----- 2.4 提交到遠程倉庫
$ git push origin develop
{% endhighlight %}

另外，需要注意的是，上述合併到主分支的時候，採用的是 \-\-no-ff 模式。

![git develop model no-ff means]({{ site.url }}/images/misc/git-develop-model-no-ff-means.png "git develop model no-ff means"){: .pull-center width="70%" }

該參數的作用是強行關閉 fast-forward 方式，該方式就是當條件允許的時候，git 直接把 HEAD 指針指向合併分支的頭，完成合並。不過如果刪除分支，由於這個過程中沒有創建 commit，則會丟失分支信息，使用該參數將保留分支 commit 歷史。

實際上，通過 git log 查看時，使用 \-\-on-ff 會強制添加 commit ，否則看不到合併的信息。

<!--
git merge --squash 是用來把一些不必要commit進行壓縮，比如說，你的feature在開發的時候寫的commit很亂，那麼我們合併的時候不希望把這些歷史commit帶過來，於是使用--squash進行合併，此時文件已經同合併後一樣了，但不移動HEAD，不提交。需要進行一次額外的commit來“總結”一下，然後完成最終的合併。
-->

另外，還有個參數 \-\-squash，會把多次分支 commit 歷史壓縮為一次。


### Release Branches

用於正式發佈前的最後測試，儘量減少 bug 數目，添加元信息 (版本號以及發佈日期等)。該分支從 develop 創建，可以合併到 develop 或者 master 分支，其命名為 release-* 。

合併到 master 之後就可以用於發佈了，而之所以合併到 develop ，是為了將最新修改合併到新的開發分支。

{% highlight text %}
----- 1. 創建分支，並更新文件一些頭部信息等，然後提交
$ git checkout -b release-1.2 develop
$ ./bump-version.sh 1.2
$ git commit -a -m "Bumped version number to 1.2"

----- 2. 測試沒有問題後，準備正式發佈
$ git checkout master
$ git merge --no-ff release-1.2
$ git tag -a 1.2

----- 3. 刪除release分支
$ git checkout develop
$ git merge --no-ff release-1.2
$ git branch -d release-1.2
{% endhighlight %}

### Hotfix Branches

該分支由於修復線上 bug，當線上代碼出現 bug 時，從 master 開一個 hotfix 分支，修復 bug 之後再將 hotfix 分支合併到 master 分支並進行發佈，同時也需要合併到 develop 分支上去。

![git develop model hotfix branch]({{ site.url }}/images/misc/git-develop-model-hotfix-branch.png "git develop model hotfix branch"){: .pull-center width="50%" }

可以發現，hotfix 和 release 分支功能類似，兩者好處是不會打斷 develop 分支正常功能開發。

{% highlight text %}
----- 1. 從master獲取分支
$ git checkout -b hotfix-1.2.1 master
$ ./bump-version.sh 1.2.1
$ git commit -a -m "Bumped version number to 1.2.1"

----- 2. 然後修改代碼修復bug，並提交
$ git commit -m "Fixed severe production problem"

----- 3. bug修復後，將該分支合併到master
$ git checkout master
$ git merge --no-ff hotfix-1.2.1
$ git tag -a 1.2.1

----- 4. 然後將hotfix合併到develop分支
$ git checkout develop
$ git merge --no-ff hotfix-1.2.1

----- 5. 刪除hotfix分支
$ git branch -d hotfix-1.2.1
{% endhighlight %}


## git-flow

[git-flow](https://github.com/nvie/gitflow) 提供了很多不錯的腳本，來支持上述的這種開發模式，不過它並沒有提供重置 (rebase) 特性分支的能力，安裝方式可以參考 [gitflow-installation](https://github.com/nvie/gitflow/wiki/Installation) 。

該工具集成了一些常見的命令，每次操作實際都會提示一些操作記錄。

### 安裝

可以直接從 [github gitflow/contrib/gitflow-installer.sh](https://raw.githubusercontent.com/nvie/gitflow/develop/contrib/gitflow-installer.sh) 下載文件，然後執行如下命令即可。

{% highlight text %}
----- 安裝
$ wget --no-check-certificate -q https://raw.githubusercontent.com/.../gitflow-installer.sh
# bash gitflow-installer.sh install stable
$ rm gitflow-installer.sh

----- 刪除
# bash gitflow-installer.sh uninstall
{% endhighlight %}

實際上很簡單，只是將代碼 clone 到本地，然後通過 install 命令安裝；當然，也可以離線安裝，該模塊會依賴於 [github - nvie/shFlags](https://github.com/nvie/shFlags)，直接將該代碼下載並放置到源碼的 shFlags 目錄下即可。

最後，通過 ```make install``` 安裝，也可以執行如下步驟，不過需要修改安裝腳本的代碼，註釋掉下載代碼的步驟。

{% highlight text %}
$ tar -xf gitflow-x.x.x.tar.gz shFlags-x.x.x.tar.gz
$ mv gitflow-x.x.x gitflow && rm gitflow/shFlags -rf
$ mv shFlags-x.x.x gitflow/shFlags
# bash gitflow/contrib/gitflow-installer.sh
{% endhighlight %}



### 初始化

使用 git-flow 時，需要從初始化一個 git 庫開始，其中命令如下。

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

主要是回答幾個關於分支的命名約定，建議使用默認值。

對於最後的 tag 選項，可以設置為軟件的名稱，例如 ```foobar-```，然後 Release Branches 的名稱使用類似 ```v1.1.0``` 名稱，那麼最終的 tag 名即為 ```foobar-v1.1.0``` 。

關於該 tag 的信息可以通過 ```git show foobar-v1.1.0``` 命令查看。

### 新特性

關於新特性的開發流程，為即將發佈的版本開發新功能特性，這通常只存在開發者的庫中。

#### 增加新特性

新特性的開發是基於 'develop' 分支的，可以通過下面的命令開始開發新特性：

{% highlight text %}
$ git flow feature start MYFEATURE
{% endhighlight %}

這個操作創建了一個基於 'develop' 的特性分支，並切換到這個分支之下。

#### 完成新特性

完成開發新特性之後，執行下面的操作，主要是合併 MYFEATURE 分支到 'develop'，並刪除這個新特性分支，然後切換回 'develop' 分支。

{% highlight text %}
$ git flow feature finish MYFEATURE
{% endhighlight %}

#### 發佈新特性

主要用於合作開發一新特性，將新特性分支發佈到遠程服務器，這樣其他用戶可以使用該分支。

{% highlight text %}
$ git flow feature publish MYFEATURE
{% endhighlight %}

#### 取得新特性分支

主要用於取得其他用戶發佈的新特性分支，並簽出遠程的變更，主要有如下的兩種方式。

{% highlight text %}
----- 簽出特性分支變更
$ git flow feature pull origin MYFEATURE

----- 跟蹤特性分支變更
$ git flow feature track MYFEATURE
{% endhighlight %}

### 預發佈版本

該分支準備開始發佈，用於支持一個新的用於生產環境的發佈版本，允許修正小問題 (bug)，但不新增特性，併為發佈版本準備元數據。

#### 準備

開始準備預發佈版本，它從 'develop' 分支開始創建一個 release 分支。

{% highlight text %}
$ git flow release start RELEASE [BASE]
{% endhighlight %}

可以選擇一個 [BASE] 參數，這是 'develop' 分支提交記錄的 hash 值。

創建 release 分支之後，可以立即發佈到遠端，從而允許其它用戶向這個 release 分支提交代碼，該命令十分類似發佈新特性：

{% highlight text %}
----- 提交到遠端
$ git flow release publish RELEASE

----- 簽出release版本的遠程變更
$ git flow release track RELEASE
{% endhighlight %}

#### 完成預發佈版本

完成 release 版本分支操作，主要執行下面幾個動作：A) 合併 release 分支到 master 分支；B) 用 release 分支名打 tag；C) 歸併 release 分支到 develop；D) 移除 release 分支。

{% highlight text %}
$ git flow release finish RELEASE
{% endhighlight %}

### 緊急修復

當生產環境的版本突然發現一個嚴重 bug 時，需要立即修正；此時，有可能是需要修正 master 分支上某個 tag 標記的生產版本。

#### 開始

像其它 git flow 命令一樣, 可以通過如下命令創建一個緊急修復分支。

{% highlight text %}
$ git flow hotfix start VERSION [BASENAME]
{% endhighlight %}

VERSION 參數標記著修正版本，可以從 [BASENAME] 開始，[BASENAME] 為 finish release 時填寫的版本號。

#### 完成

當完成緊急修復分支後，代碼歸併回 develop 和 master 分支，相應地，master 分支打上修正版本的 tag 。

{% highlight text %}
$ git flow hotfix finish VERSION
{% endhighlight %}

### 其它

##### unable to start editor

可以通過 `git config --global core.editor "vim"` 命令設置。

## 參考

關於 Git 的分支介紹可以參考 [git-scm.com/doc](https://git-scm.com/doc) 中的介紹，另外，還有一本不錯介紹 Git 的資料 [Pro Git Book](http://git-scm.com/book/)；另外一個不錯的文檔 [圖解Git](http://marklodato.github.io/visual-git-guide/index-zh-cn.html)，或者 [本地文檔](/reference/misc/git_graphs.mht) 。

對於一種不錯的 Git 分支管理模式，也即如上的介紹，可以參考 [A successful Git branching model](http://nvie.com/posts/a-successful-git-branching-model/) 或者 [本地文檔](/reference/misc/A successful Git branching model.mht) 。

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
