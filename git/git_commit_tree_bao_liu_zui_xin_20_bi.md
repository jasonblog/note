# git commit tree 保留最新20筆


```sh
#!/bin/bash

cd /tmp
git clone https://github.com/chewing/libchewing.git
cd libchewing
git checkout -b new-tree
git reset --hard $(git commit-tree HEAD~21^{tree} -m "initial commit")
for i in $(seq 20 -1 0)
do
    if [[ $(git cat-file -p master~${i} | grep parent | wc -l) -gt 1 ]]; then
        git cherry-pick --allow-empty -m 1 master~${i}
    else
        git cherry-pick master~${i}
    fi
done
git diff master --exit-code
```


## 上傳到github

```sh
git remote add backup https://github.com/shihyu/runninglinuxkernel_4.git
git push -u backup new-tree
```