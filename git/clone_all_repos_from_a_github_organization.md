# Clone all repos from a GitHub organization


- sysprog21

```sh
curl -s https://api.github.com/users/sysprog21/repos?per_page=200 | python -c $'import json, sys, os\nfor repo in json.load(sys.stdin): os.system("git clone " + repo["ssh_url"])'
```


```py
python -c "import json, urllib, os; [os.system('git clone ' + r['ssh_url']) for r in json.load(urllib.urlopen('https://api.github.com/users/yenWu/repos?per_page=200'))]"
```