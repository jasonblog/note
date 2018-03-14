---
title: Ansible 簡介
layout: post
comments: true
language: chinese
category: [linux, network, python]
keywords: ansible,簡介,python
description: Ansible 是一個配置管理工具，當然同類的產品還有 Puppet、Chef、SaltStack 等，不過上述的三者都是基於服務端+客戶端的模式，而 Ansible 相對來說要簡單的多，無需安裝服務端和客戶端，只需要有 ssh 即可，而且使用簡單。
---

Ansible 是一個配置管理工具，當然同類的產品還有 Puppet、Chef、SaltStack 等，不過上述的三者都是基於服務端+客戶端的模式，而 Ansible 相對來說要簡單的多，無需安裝服務端和客戶端，只需要有 ssh 即可，而且使用簡單。

<!-- more -->

## 簡介

Ansible 是基於 Python 的配置管理和應用部署工具，這個非常簡單，只需要 sshd 服務就可以，不需要客戶端，官方稱之為 "Ansible is Simple IT Automation"。

Ansible 採用模塊化設計，基於 Python 實現，其底層調用 Paramiko 庫實現併發連接 ssh 主機，另外還包括了兩個基本庫：PyYAML 和 Jinja2，分別用於配置文件以及模板化。

![ansible]({{ site.url }}/images/network/ansible-arch.png "ansible"){: .pull-center width="600"}

Ansible 的配置文件保存在 `/etc/ansible/` 目錄下，包括了主配置文件 `ansible.cfg`，以及管控主機 Host Inventory 的配置文件 hosts ；如果沒有可以直接新建。

### 安裝、測試

在 CentOS 中可以通過如下方式安裝。

{% highlight text %}
----- 直接通過yum安裝，不過有可能版本會比較老
# yum --enablerepo=epel install ansible

----- 通過pip安裝
# pip install ansible

----- 安裝完之後可以通過如下命令查看版本號
# ansible --version

----- 安裝啟動 sshd 服務
# systemctl start sshd

----- 首先設置SSH免密鑰登錄
$ ssh-keygen -t rsa -P ''                                    # 生成公鑰/私鑰
# cat /root/.ssh/id_rsa.pub >> /root/.ssh/authorized_keys    # 寫入信任文件，所有服務器都需要執行
# chmod 600 /root/.ssh/authorized_keys

----- 測試能否登陸
$ ssh foobar@127.1
{% endhighlight %}

測試 ansible 時，需要將 `/etc/ansible/hosts` 文件配置為 127.1 的 IP 地址。

{% highlight text %}
$ cat /etc/ansible/hosts
127.1
$ ansible all -m ping -u foobar
127.1 | SUCCESS => {
    "changed": false,
    "ping": "pong"
}

----- 也可以直接修改配值文件的內容
$ cat /etc/ansible/hosts
foobar ansible_connection=local ansible_ssh_host=127.1 ansible_ssh_user=foobar

$ ansible all -u foobar -m command -a "date"     # 執行下測試命令
$ ansible all -u foobar -m shell -a "date"       # 同上
$ ansible all -m setup -u foobar                 # 查看配置
{% endhighlight %}

其中 `-m` 指定模塊，`-u` 指定用戶名。

### 主機的系統變量 (facts)

ansible 會通過 module setup 來收集主機的系統信息，這些收集到的系統信息叫做 facts，這些 facts 信息可以直接以變量的形式使用。可以在命令行上調用 setup module 命令，查看那些變量可以使用。

{% highlight text %}
$ ansible all -m setup -u foobar -a "filter=ansible_os_family"
{% endhighlight %}

然後可以在 playbook 中直接使用 facts 變量。

{% highlight text %}
---
- hosts: all
  user: root
  tasks:
  - name: echo system
    shell: echo { { ansible_os_family } }
  - name install ntp on Debian linux
    apt: name=git state=installed
    when: ansible_os_family == "Debian"
  - name install ntp on redhat linux
    yum: name=git state=present
    when: ansible_os_family == "RedHat"
{% endhighlight %}

對於比較複雜的屬性，可以通過如下的兩種方式訪問。

{% highlight text %}
----- 使用中括號
{ { ansible_ens3["ipv4"]["address"] } }

----- 使用點號
{ { ansible_ens3.ipv4.address } }
{% endhighlight %}

在 Playbook 中，可以通過 gather_facts 控制是否收集遠程系統的信息，如果不需要直接如下填寫即可。

{% highlight text %}
- hosts: whatever
  gather_facts: no
{% endhighlight %}

#### 自定義 facts

可以在 `/etc/ansible/facts.d` 目錄下添加文件自定義 facts 。

{% highlight text %}
$ cat /etc/ansible/facts.d/foobar.fact
[general]
foobar = "Just For Test"

$ ansible all -m setup -u foobar -a "filter=ansible_local"
{% endhighlight %}


## Inventory

Ansible 中的 Inventory 文件定義了服務器的列表，默認情況下是 `/etc/ansible/hosts` 。

{% highlight yaml %}
# 主機
192.168.9.10               # IP地址
mail.foobar.com            # 使用FQDN表示，包括主機名+域名
jumper ansible_ssh_port=5555 ansible_ssh_host=192.168.9.50        # 將某些靜態IP設置為別名
localhost ansible_connection=local                                # 指定選項

[webservers]               # 分組
web1.foobar.com
web2.foobar.com
web[10:50].foobar.com      # 表示從web10到web50，共計49臺主機
web[a:f].foobar.com        # 表示從weba到webf
web.foobar.com ansible_connection=ssh ansible_ssh_user=light      # 指定選項
{% endhighlight %}

其中常用的選項包括了：

{% highlight text %}
ansible_ssh_host                # 指定ssh連接的IP或者FQDN
ansible_ssh_port                # ssh連接端口
ansible_ssh_user                # 默認ssh連接用戶
ansible_ssh_pass                # ssh連接的密碼，這種方式不安全，可以用--ask-pass，或公鑰
ansible_sudo_pass               # sudo用戶的密碼
ansible_connection              # ssh連接的類型，包括local、ssh、paramiko
ansible_ssh_private_key_file    # ssh連接的公鑰文件
ansible_shell_type              # 主機使用的shell解釋器，默認sh，可以設置為csh、fish等
ansible_python_interpreter      # 用來指定python解釋器的路徑
{% endhighlight %}


## PlayBook

首先看個很簡單的示例。

{% highlight text %}
$ cat foobar.yml
---
- hosts: all
  remote_user: root
  tasks:
    - name: test connection
      ping:
      remote_user: foobar             # 可以用不同的用戶

$ ansible-playbook foobar.yml
{% endhighlight %}

playbook 是由一個或多個 "play" 組成的列表，其包括瞭如下的組件：

* hosts：指定要執行任務的主機，可以是一個或多個由冒號分割主機組。

* user：通過remote_user指定遠程主機上的執行任務的用戶。

* tasks：play主體部分是各個任務按次序逐個在hosts中指定的所有主機上執行。

* action：任務執行過程。

* handlers：用於當前關注的資源發生變化時採取一定指定的操作。

執行結果通過三種顏色表示：綠色代表執行成功，系統保持原樣；黃色代表系統代表系統狀態發生改變；紅色代表執行失敗，顯示錯誤輸出。

{% highlight text %}
# cat web.yml
- hosts: test                                # 選擇執行命令的主機組，在/etc/ansible/hosts定義
  vars:                                      # 設置的變量
    http_port: 80
    max_clients: 200
  remote_user: root                          # 遠端執行任務的用戶，也可以使用sudo
  tasks:                                     # 任務
  - name: install httpd                      # 任務描述
    command: yum -y install httpd            # 調用ansible的command模塊安裝httpd
    when: ansible_distribution == "CentOS"   # 設置執行條件
  - name: provide httpd.conf                 # 任務描述
    copy: src="/root/httpd.conf" dest="/etc/httpd/conf/httpd.conf"
    tags: conf                               # 打標記用於單獨執行標記任務，用ansible-playbook -C執行
    notify:                                  # 文件內容變更通知
    - server restart                         # 通知到指定的任務
  - name: server start                       # 任務描述
    service: name=httpd state=started enabled=true
  handlers:                                  # 定義接受關注的資源變化後執行的動作
  - name: server restart                     # 任務描述
    service: name=httpd state=restarted      # 當關注的資源發生變化後調用service模塊
{% endhighlight %}

在執行時，首先會收集遠端主機的元數據信息，也即 facts，包括了 IP 地址、CPU 核數、系統架構、主機名等信息，然後這些元數據可以作為變量供後面的 task 使用。

{% highlight text %}
$ ansible host -m setup -i inventory_file
{% endhighlight %}

當然可以通過 `gather_facts: no` 選項關閉，經常被用在條件語句和模板當中，可以使用條件判斷語句關閉指定發行版的主機。

### Notify + Handler

ansible 還支持設置 handlers，也就是是在執行 tasks 之後可供調用的處理過程，使用起來如下，同樣是本地執行:

{% highlight text %}
$ cat /etc/ansible/hosts
localhost ansible_connection=local ansible_ssh_host=127.1 ansible_ssh_user=foobar

$ echo "Hello World, Ansible" > /tmp/foobar

$ cat playbook.yml
---
- hosts: localhost       # hosts中指定
  tasks:
      - name: whoami
        copy: src=/tmp/foobar dest=/tmp/foobar.copy  #  本地拷貝到遠端
        notify:  # 如果複製前發現foobar.copy文件改變了，則執行如下的handler
            - clear copy
  handlers:
      - name: clear copy
        shell: 'mv /tmp/foobar.copy /tmp/foobar.del'  # 假裝刪除
{% endhighlight %}

上述判斷 foobar.copy 改變暫時還不太清楚如何做到的，無論是缺少，還是內容被修改，都會執行。

### 調試

對於 Playbook 可以通過如下方式調試。

{% highlight text %}
$ ANSIBLE_KEEP_REMOTE_FILES=1 ansible-playbook -i production site.yml \
 --tags git --step --start-at-task='git | setup' -vvvv
{% endhighlight %}

其中各個參數如下：

* ANSIBLE_KEEP_REMOTE_FILES=1 阻止刪除遠端的臨時文件，可以用來查看。

* 通過 `--tags` 和 `--start-at-task` 設置執行那些任務，或者從那些任務開始。

* `--step` 表示單步執行，每執行完一個任務都會暫停。

* 使用多個 `-v` 參數，輸出詳細的調試信息。

另外，可以使用 debug 模塊，方式如下：

{% highlight text %}
- name: debug this
  debug: var=result
{% endhighlight %}

還可以在配置文件中通過 log_path 指定日誌文件路徑。

### 輸入密碼

通過 ansible 連接遠程主機時，默認是需要輸入遠程主機的密碼的，這樣會很不方便，可以有以下三種方式處理：

#### 1、通過 -k 提示輸入密碼

實際上該參數是 `--ask-pass` 的簡寫形式，不過這種方式依賴於 sshpass 命令。

{% highlight text %}
# yum --enablerepo=epel install sshpass
$ ansible all -m setup -k -u foobar -a "filter=ansible_os_family"
{% endhighlight %}

也可以在 playbook 文件中指定提示輸入密碼。

{% highlight text %}
hosts: all
remote_user: root
vars_prompt:
 - name: 'https_passphrase'          # 存儲數據的變量名
   prompt: 'Key Passphrase'          # 手工輸入數據
   private: yes                      # 當該值為yes，則用戶的輸入不會被打印
{% endhighlight %}

#### 2、 將用戶名密碼保存在 inventory 文件中

一般 inventory 文件保存在 `/etc/ansible/hosts` 文件裡，書寫格式如下。

{% highlight text %}
foobar ansible_ssh_host=127.1 ansible_ssh_user=root ansible_ssh_pass=yourpassword
{% endhighlight %}

#### 3. 使用SSH免祕鑰登錄

實際上就是通過 SSH 的公私鑰驗證方式。

### 其它技巧

為了靈活的控制 playbook，ansible 提供了 ansible-playbook 命令行工具，如下是一些常用的示例，更過用法可以參考 ansible-playbook -h 。

#### 指定開始 tasks

查看當前任務，然後指定從哪裡開始執行任務。

{% highlight text %}
$ ansible-playbook --list-tasks foobar.yml
$ ansible-playbook --start-at-task="foo bar" foobar.yml
{% endhighlight %}

#### 只在某主機執行任務

可以通過如下方式執行，或者跳過某些任務。

{% highlight text %}
$ ansible-playbook --limit=host1 --tags=foobar foobar.yml
$ ansible-playbook --limit=host1 --skip-tags=foobar foobar.yml
{% endhighlight %}

## Vault

當使用 Ansible 自動化地維護服務器時，不可避免需要接觸一些密碼或其它敏感數據，我們可以使用三方的密碼管理工具，如 HashiCorp 的 Vault、Square 的 Keywhiz、亞馬遜的 Key Management Service 和微軟 Azure 的 Key Vault。

當然也可以使用 Ansible 自帶的 Vault 加密功能，可以將經過加密的密碼和敏感數據同 Playbook 存儲在一起，詳細的內容可以查看


<!--
http://docs.ansible.com/ansible/playbooks_vault.html
http://www.ansible.com.cn/docs/playbooks_vault.html

Ansible Vault 的工作方式跟現實生活中的保險櫃的工作方法很像：

    1.    我們可以把Ansible任務中用到的任意文件放入Vault保險櫃中
    2.    Ansible Vault會使用密碼來加密這些文件，就鑰匙把保險櫃的門關起來一樣
    3.    我們把密碼（鑰匙）保存在一個只有我們自己知道或都有權訪問的地址，與Playbook獨立分開存儲
    4.    在我們需要運行Playbook的時候，我們拿出密碼（鑰匙），解密敏感數據（打開保險櫃門，拿出數據），就能正常的執行Playbook任務了

接下來，我們通過一個實例來了解一下這事個過程。下面的一段Playbook代碼是要使用API key的方式來訪問一個服務的API：

---
- hosts: appserver
  vars_files:
    - vars/api_key.yml
  tasks:
    - name: Connect to service with our API key.
      command: connect_to_service
      environment:
        SERVICE_API_KEY: "{{ myapp_service_api_key }}"
---
myapp_service_api_key: “yJJvPqhqgxyPZMispRycaVMBmBWPqYDf3DFanPxAMAm4UZcw"

這種將key存儲在純文件文件中的做法非常之便捷，但是並不安全。如果我們使用Ansible Tower和Jenkins等工具來運行Playbook，或者Playbook在一個共享的環境中時，這種存儲key的方法就更不可取。或許我們有非常嚴格的主機操作和系統安全規範，但是我們並不能保證每一位開發者或管理員都能嚴格的遵守，人通常是整個環節中最不穩定的因素。Ansible Vault可以為我們提供非常高的安全加密級別，這將很好幫我們解決後顧之憂。

    使用如下命令，可以利用Vault給文件加密：

    1 $ANSIBLE_VAULT;1.1;AES256
    2 653635363963663439383865313262396665353063663839616266613737616539303
    3 530313663316264336133626266336537616463366465653862366231310a30633064
    4 633234306335333739623661633132376235666563653161353239383664613433663
    5 1303132303566316232373865356237383539613437653563300a3263386336393866
    6 376535646562336664303137346432313563373534373264363835303739366362393
    7 639646137656633656630313933323464333563376662643336616534353234663332
    8 656138326530366434313161363562333639383864333635333766316161383832383
    9 831626166623762643230313436386339373437333830306438653833666364653164
    10 6633613132323738633266363437

    除了encrypt選項之外，關於ansible-vault命令有幾個比較常用的選項，列舉如下：

        edit: 用於編輯經過ansible-vault加密過的文件

        rekey: 重新修已被加密文件的密碼

        create: 創建一個新文件，並直接對其進行加密

        view: 查看經過加密的文件

        decrypt: 解密文件

            以上所有選項都可以在後面是時跟多個文件進行操作，比如：ansible-vault create x.yml y.yml z.yml

除了手動輸入密碼進行解密以外，Ansible還提供了以密碼文件形式來解密的認證方式，這類似於SSH的密鑰認證。SSH將密鑰放於~/.ssh目錄下面，Ansible Vault將密碼文件放置於~/. ansible/，同時對於這個文件也必須有嚴格的權限控制，需設置其權限為600。現在我們可以~/.ansible/目錄下創建一個權限為600的純文件文件vault_pass.txt，並寫入我們的Vault密碼，使用如下命令就可非交互式的使用被加密過的Playbook運行任務了：

$ ansible-playbook test.yml --vault-password-file ~/.ansible/vault_pass.txt

我們也可以使用可執行腳本來生成單行的Vault密碼，比如∼/.ansible/vault_pass.py，前提是該腳本只能生成一行密碼數據。
如果系統上通過pip install cryptography命令安裝了Python的cryptography模塊，那麼這將會加快Vault的運行速度。

    你是否在為Ansbile Vault的加密方式感到不放心？Ansible Vault採用AES-256加密算法對文件進行加密，這種自滿是極其安全的。即使是使用目前世界上最運算速度最快的集群來7x24小時的去解密一個通過該算法加密的文件，也需要數十億年的時間才能完成破解。所以，安全算法是沒有問題的，我們要做的就是保管好自己的Vault加密密碼。
    可以官方文檔中參考更的選項用法及詳細案例：http://docs.ansible.com/ansible/playbooks_vault.html

第二章、變量優先級

通過本章節一系列對變量的學習，可能會有人問了，定義變量的方式有那麼多，我究竟該如何確定哪一個定義會最終生效呢？下面我們就來理一下變量優先級的問題。如果同樣名稱的變量在多個地方被定義,那麼究竟以哪一次定義的值為準呢？Ansible官方給出瞭如下優先級排序:

    1、在命令行中定義的變量（即用-e定義的變量）優先級最高

    2、在inventory中定義的連接變量(比如ansible_ssh_user)

    3、大多數的其它變量(命令行轉換,play中的變量,included的變量,role中的變量等)

    4、在Inventory定義的其它變量

    5、由系統通過gather_facts方法發現的Facts

    6、“Role默認變量”, 這個是最默認的值,很容易喪失優先權

通過一段時間的使用之後呢，大部分人都會有自己的一套定義變量的方法或習慣。下面我們總結一些變量定義方面的小技巧，希望可以給初學者提供一些有益的參考：

    Role(下節即將講到)中的默認變量應設置的儘可能合理，因為他優先級最低，以防這些亦是在其他地方都沒被定義，而Role的默認亦是又定義的不合理而產生問題。

    Playbook中應儘量少的定義變量，Playbook中用的變量應儘量定義在專門的變量文件中通過vars_files引用，或定義在Inventory文件中。

    只有真正與主機或主機組強相關的變量才定義在Inventory文件中。

    應儘量少的在動態或靜態的Inventory源文件中定義變量，尤其是不要定義那些很少在Playbook中被用的變量。

    應儘量避免在命行中使用-e選項來定義變量。只有在我們不用去關心項目的可維護性和任務冪等性的時候，才建議全用這種變量定義方式。比如只是做本地測試或者運行一個一次性的Playbook任務

關注我們
-->


## Python API

詳細的可以參考官方文檔 [Python API](http://docs.ansible.com/ansible/developing_api.html)，在 2.0 之前的版本，代碼非常簡單，如下：


{% highlight text %}
import ansible.runner

runner = ansible.runner.Runner(
   module_name='ping',
   module_args='',
   pattern='web*',
   forks=10
)
datastructure = runner.run()
{% endhighlight %}

如下是 2.0 的接口使用示例。

{% highlight python %}
#!/usr/bin/env python
#-*- coding: UTF-8 -*-

import json
from collections import namedtuple
from ansible.parsing.dataloader import DataLoader
from ansible.vars import VariableManager
from ansible.inventory import Inventory
from ansible.playbook.play import Play
from ansible.executor.task_queue_manager import TaskQueueManager
from ansible.plugins.callback import CallbackBase

class ResultCallback(CallbackBase):
    """A sample callback plugin used for performing an action as results come in

    If you want to collect all results into a single object for processing at
    the end of the execution, look into utilizing the ``json`` callback plugin
    or writing your own custom callback plugin
    """
    def v2_runner_on_ok(self, result, **kwargs):
        """Print a json representation of the result

        This method could store the result in an instance attribute for retrieval later
        """
        host = result._host
        print json.dumps({host.name: result._result}, indent=4)

# 指定選項，可以在啟動時指定，或者在/etc/ansible/ansible.cfg中指定
Options = namedtuple('Options', ['connection', 'module_path', 'forks', 'become',
    'become_method', 'become_user', 'check'])
options = Options(connection='local', module_path='/path/to/mymodules', forks=100,
    become=None, become_method=None, become_user=None, check=False)

# initialize needed objects
variable_manager = VariableManager()
loader = DataLoader()
passwords = dict(vault_pass='secret')

# Instantiate our ResultCallback for handling results as they come in
results_callback = ResultCallback()

# create inventory and pass to var manager
inventory = Inventory(loader=loader, variable_manager=variable_manager, host_list='/etc/ansible/hosts')
variable_manager.set_inventory(inventory)

# create play with tasks
play_source =  dict(
        name = "Ansible Play",
        hosts = 'localhost',
        gather_facts = 'no',
        tasks = [
            dict(action=dict(module='shell', args='ls'), register='shell_out'),
            dict(action=dict(module='debug', args=dict(msg='{{shell_out.stdout}}')))
         ]
    )
play = Play().load(play_source, variable_manager=variable_manager, loader=loader)

# actually run it
tqm = None
try:
    tqm = TaskQueueManager(
              inventory=inventory,
              variable_manager=variable_manager,
              loader=loader,
              options=options,
              passwords=passwords,
              stdout_callback=results_callback,  # Use custom callback instead of the 'default'
          )
    result = tqm.run(play)
finally:
    if tqm is not None:
        tqm.cleanup()
{% endhighlight %}

## 源碼解析

實際上 ansible 安裝後，ansible-XXX 都是指向 ansible 的可執行文件，不過調用的時候會在 ansible 中進行判斷，根據不同的命令調用 cli 目錄下的文件。


<!---
import ansible.runner
import ansible.inventory
import loggingMod.init_logger
import json
import sys

class rootUser():

    def __init__(self, ipAddress, userName, remoteUser):
        self.ipAddress = [ ipAddress ]
        self.userName = userName
        self.remoteUser = remoteUser

        '''init base info'''
        self.webInventory = ansible.inventory.Inventory(self.ipAddress)
        self.remotePort = 22
        self.timeOut = 10
        self.priKeyFile = '/home/%s/.ssh/id_rsa'

    def printLog(self, output):
        for (hostname, result) in output["contacted"].items():
            if 'failed' in result:
                logging.error('there is a error[%s]' % result['msg'])
                sys.exit(1)
            elif 'stderr' in result:
                if result['stderr']:
                  logging.error('there is a error [%s]' % result['stderr'])
                    sys.exit(1)

    def checkUserExist(self):
        checkrunner = ansible.runner.Runner(
            module_name='shell',
            module_args='id -u %s' % self.userName,
            timeout=self.timeOut,
            remote_port=self.remotePort,
            remote_user=self.remoteUser,
#           private_key_file=self.priKeyFile,
            become=True,
            become_user='root',
            inventory = self.webInventory
        )
        self.output = checkrunner.run()
        self.printLog(self.output)

    def addRoot(self):
        self.checkUserExist()
        runner = ansible.runner.Runner(
            module_name='user',
            module_args='name=%s groups=root append=no' % self.userName,
            timeout=self.timeOut,
            remote_port=self.remotePort,
            remote_user=self.remoteUser,
            private_key_file=self.priKeyFile,
            become=True,
            become_user='root',
            inventory = self.webInventory
        )

        self.result = runner.run()
        self.printLog(self.result)
        self.info('success')

    def removeRoot(self):
        rmRunner = ansible.runner.Runner(
            module_name='shell',
            module_args='gpasswd -d %s root' % self.userName,
            timeout=self.timeOut,
            remote_port=self.remotePort,
            remote_user=self.remoteUser,
            private_key_file=self.priKeyFile,
            become=True,
            become_user='root',
            inventory = self.webInventory
        )
        self.rmOutput = rmRunner.run()
        self.printLog(self.rmOutput)
        self.info('success remove root privilege')


theUser = rootUser('192.168.35.125', 'mr.right', 'raingolee')
theUser.removeRoot()
-->


## 參考

可以查看中文幫助 [www.ansible.com.cn](http://www.ansible.com.cn)，源碼參考 [github - ansible](https://github.com/ansible/ansible) 。可以查看 [ansible 中文文檔](/reference/linux/Ansible-中文手冊.pdf)，其中有介紹一個不錯的運維監控工具的總結。

詳細的文檔可以參考 [Ansible Documentation](http://docs.ansible.com/ansible/) 。


<!--
http://docs.ansible.com/ansible/playbooks_best_practices.html

ansible常見用法為 ansible host-pattern -m 模塊 -a 命令 ，host-pattern類似於簡化的正則表達式，而模塊可以通過 ansible-doc -l 命令來查詢。下面是一些常用模塊的使用方法：

http://hakunin.com/six-ansible-practices
-->


{% highlight text %}
{% endhighlight %}
