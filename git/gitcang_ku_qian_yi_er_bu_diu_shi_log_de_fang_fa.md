# Git倉庫遷移而不丟失log的方法


要求能保留原先的commit記錄，應該如何遷移呢？
同時，本地已經clone了原倉庫，要配置成新的倉庫地址，該如何修改呢？
 
##1 建立新倉庫
1). 從原地址克隆一份裸版本庫，比如原本託管於 GitHub。

git clone --bare git://github.com/username/project.git

2). 然後到新的 Git 服務器上創建一個新項目，比如 GitCafe。

3). 以鏡像推送的方式上傳代碼到 GitCafe 服務器上。

cd project.git

git push --mirror git@gitcafe.com/username/newproject.git

4). 刪除本地代碼

cd ..

rm -rf project.git

5). 到新服務器 GitCafe 上找到 Clone 地址，直接 Clone 到本地就可以了。

git clone git@gitcafe.com/username/newproject.git

這種方式可以保留原版本庫中的所有內容。

 

##切換remote_url

先查看remote的名字

git branch -r
假設你的remote是origin，用git remote set_url 更換地址

git remote set-url origin remote_git_address
remote_git_address更換成你的新的倉庫地址。

第二種切換remote_url的方法更直接，直接更改.git/conf配置文件裡的ip地址就行。

