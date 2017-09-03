# 如何将GIT存储库从一个服务器迁移到新服务器


###问题描述

我有一台服务器，我正在服用。我唯一要迁移的是我的存储库。该服务器被列为我的一个项目的原始(主)。移动存储库以保持历史记录的正确方法是什么？
###最佳解决方案

要添加新的回购位置，

```sh
git remote add new_repo_name new_repo_url
```

然后将内容推送到新位置

```sh
git push new_repo_name master
```

最后删除旧的
```sh
git remote rm origin
```

之后，你可以做什么bdonlan说和编辑the.git /config文件来改变new_repo_name到原点。如果您不删除原始(原始远程存储库)，您只需将更改推送到新的备份库
```sh
git push new_repo_name master
```

###次佳解决方案

如果要迁移所有分支和标签，您应该使用以下命令：
```sh
git clone --mirror [oldUrl]
```

用所有分支克隆旧的回购
```sh
cd the_repo
git remote add remoteName newRepoUrl
```

设置一个新的遥控器

```sh
git push -f --tags remoteName refs/heads/*:refs/heads/*
```

推送所有参考文献(可能是你想要的)
###第三种解决方案

复制它。真的很简单:)
在客户端，只需在客户端的本地备份中编辑.git /config，就可以根据需要将遥控器指向新的URL。
###第四种方案

这对我来说无瑕疵。

```sh
git clone --mirror <URL to my OLD repo location>
cd <New directory where your OLD repo was cloned>
git remote set-url origin <URL to my NEW repo location>
git push -f origin
```

我必须提到，这将创建一个您当前的回购的镜子，然后将其推送到新的位置。因此，这可能需要一些时间才能进行大型回收或连接缓慢。
##第五种方案

我只是重新发表别人所说的话，简单地按照指示清单。
移动存储库：只需登录到新服务器cd到您要保存存储库的父目录，并使用rsync从旧服务器复制：
```sh
new.server> rsync -a -v -e ssh user@old.server.com:path/to/repository.git .
```

使客户端指向新的存储库：现在在使用存储库的每个客户端上，只需删除指向旧源的指针，并将其添加到新的存储库。

```sh
client> git remote rm origin
client> git remote add origin user@new.server.com:path/to/repository.git
```

###第六种方案

看看这个食谱GitHub：https://help.github.com/articles/importing-an-external-git-repository
在发现git push --mirror之前，我尝试了一些方法。
像一个魅力一样工作！
###第七种方案

这在某些其他答案中是部分完成的。

```sh
git clone --mirror git@oldserver:oldproject.git
cd oldproject.git
git remote add new git@newserver:newproject.git
git push --mirror new
```

###第八种方案

我按照BitBucket的指示，将所有分支机构的回购移到那里。这里有#字符后面的解释步骤：

```sh
cd path/to/local/repo
git remote remove origin # to get rid of the old setting, this was not in the BitBucket instructions
git remote add origin ssh://git@bitbucket.org/<username>/<newrepo> # modify URL as needed
git push -u origin --all # pushes _ALL_ branches in one go
git push -u origin --tags # pushes _ALL_ tags in one go
```

为我工作很好
###第九种方案

您可以使用以下命令：
```sh
git remote set-url --push origin new_repo_url
```

示例来自http://gitref.org/remotes/

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

###第十种方案

应该简单如下：
git remote set-url origin git://new.url.here
这样你就可以为新的repo保留名称origin – 然后按照其他答案中的详细说明推送新的repo。假设你单独工作，你有一个本地的回购，你想与你所有的货物一起镜子，你也可以(从您当地的回购)

```sh
git push origin --mirror # origin points to your new repo

```

但是看到Is “git push –mirror” sufficient for backing up my repository?(一切都不使用--mirror但是一次)。
参考文献

How to migrate GIT repository from one server to a new one