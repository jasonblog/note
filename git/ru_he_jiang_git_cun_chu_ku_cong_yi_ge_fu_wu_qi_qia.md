# 如何將GIT存儲庫從一個服務器遷移到新服務器


###問題描述

我有一臺服務器，我正在服用。我唯一要遷移的是我的存儲庫。該服務器被列為我的一個項目的原始(主)。移動存儲庫以保持歷史記錄的正確方法是什麼？
###最佳解決方案

要添加新的回購位置，

```sh
git remote add new_repo_name new_repo_url
```

然後將內容推送到新位置

```sh
git push new_repo_name master
```

最後刪除舊的
```sh
git remote rm origin
```

之後，你可以做什麼bdonlan說和編輯the.git /config文件來改變new_repo_name到原點。如果您不刪除原始(原始遠程存儲庫)，您只需將更改推送到新的備份庫
```sh
git push new_repo_name master
```

###次佳解決方案

如果要遷移所有分支和標籤，您應該使用以下命令：
```sh
git clone --mirror [oldUrl]
```

用所有分支克隆舊的回購
```sh
cd the_repo
git remote add remoteName newRepoUrl
```

設置一個新的遙控器

```sh
git push -f --tags remoteName refs/heads/*:refs/heads/*
```

推送所有參考文獻(可能是你想要的)
###第三種解決方案

複製它。真的很簡單:)
在客戶端，只需在客戶端的本地備份中編輯.git /config，就可以根據需要將遙控器指向新的URL。
###第四種方案

這對我來說無瑕疵。

```sh
git clone --mirror <URL to my OLD repo location>
cd <New directory where your OLD repo was cloned>
git remote set-url origin <URL to my NEW repo location>
git push -f origin
```

我必須提到，這將創建一個您當前的回購的鏡子，然後將其推送到新的位置。因此，這可能需要一些時間才能進行大型回收或連接緩慢。
##第五種方案

我只是重新發表別人所說的話，簡單地按照指示清單。
移動存儲庫：只需登錄到新服務器cd到您要保存存儲庫的父目錄，並使用rsync從舊服務器複製：
```sh
new.server> rsync -a -v -e ssh user@old.server.com:path/to/repository.git .
```

使客戶端指向新的存儲庫：現在在使用存儲庫的每個客戶端上，只需刪除指向舊源的指針，並將其添加到新的存儲庫。

```sh
client> git remote rm origin
client> git remote add origin user@new.server.com:path/to/repository.git
```

###第六種方案

看看這個食譜GitHub：https://help.github.com/articles/importing-an-external-git-repository
在發現git push --mirror之前，我嘗試了一些方法。
像一個魅力一樣工作！
###第七種方案

這在某些其他答案中是部分完成的。

```sh
git clone --mirror git@oldserver:oldproject.git
cd oldproject.git
git remote add new git@newserver:newproject.git
git push --mirror new
```

###第八種方案

我按照BitBucket的指示，將所有分支機構的回購移到那裡。這裡有#字符後面的解釋步驟：

```sh
cd path/to/local/repo
git remote remove origin # to get rid of the old setting, this was not in the BitBucket instructions
git remote add origin ssh://git@bitbucket.org/<username>/<newrepo> # modify URL as needed
git push -u origin --all # pushes _ALL_ branches in one go
git push -u origin --tags # pushes _ALL_ tags in one go
```

為我工作很好
###第九種方案

您可以使用以下命令：
```sh
git remote set-url --push origin new_repo_url
```

示例來自http://gitref.org/remotes/

```sh
$ git remote -v
github  git@github.com:schacon/hw.git (fetch)
github  git@github.com:schacon/hw.git (push)
origin  git://github.com/github/git-reference.git (fetch)
origin  git://github.com/github/git-reference.git (push)
$ git remote set-url --push origin git://github.com/pjhyett/hw.git
$ git remote -v
github  git@github.com:schacon/hw.git (fetch)
github  git@github.com:schacon/hw.git (push)
origin  git://github.com/github/git-reference.git (fetch)
origin  git://github.com/pjhyett/hw.git (push)
```

###第十種方案

應該簡單如下：
git remote set-url origin git://new.url.here
這樣你就可以為新的repo保留名稱origin – 然後按照其他答案中的詳細說明推送新的repo。假設你單獨工作，你有一個本地的回購，你想與你所有的貨物一起鏡子，你也可以(從您當地的回購)

```sh
git push origin --mirror # origin points to your new repo

```

但是看到Is “git push –mirror” sufficient for backing up my repository?(一切都不使用--mirror但是一次)。
參考文獻

How to migrate GIT repository from one server to a new one