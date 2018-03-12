---
title: Ansible 简介
layout: post
comments: true
language: chinese
category: [linux, network, python]
keywords: ansible,简介,python
description: Ansible 是一个配置管理工具，当然同类的产品还有 Puppet、Chef、SaltStack 等，不过上述的三者都是基于服务端+客户端的模式，而 Ansible 相对来说要简单的多，无需安装服务端和客户端，只需要有 ssh 即可，而且使用简单。
---

Ansible 是一个配置管理工具，当然同类的产品还有 Puppet、Chef、SaltStack 等，不过上述的三者都是基于服务端+客户端的模式，而 Ansible 相对来说要简单的多，无需安装服务端和客户端，只需要有 ssh 即可，而且使用简单。

<!-- more -->

## 简介

Ansible 是基于 Python 的配置管理和应用部署工具，这个非常简单，只需要 sshd 服务就可以，不需要客户端，官方称之为 "Ansible is Simple IT Automation"。

Ansible 采用模块化设计，基于 Python 实现，其底层调用 Paramiko 库实现并发连接 ssh 主机，另外还包括了两个基本库：PyYAML 和 Jinja2，分别用于配置文件以及模板化。

![ansible]({{ site.url }}/images/network/ansible-arch.png "ansible"){: .pull-center width="600"}

Ansible 的配置文件保存在 `/etc/ansible/` 目录下，包括了主配置文件 `ansible.cfg`，以及管控主机 Host Inventory 的配置文件 hosts ；如果没有可以直接新建。

### 安装、测试

在 CentOS 中可以通过如下方式安装。

{% highlight text %}
----- 直接通过yum安装，不过有可能版本会比较老
# yum --enablerepo=epel install ansible

----- 通过pip安装
# pip install ansible

----- 安装完之后可以通过如下命令查看版本号
# ansible --version

----- 安装启动 sshd 服务
# systemctl start sshd

----- 首先设置SSH免密钥登录
$ ssh-keygen -t rsa -P ''                                    # 生成公钥/私钥
# cat /root/.ssh/id_rsa.pub >> /root/.ssh/authorized_keys    # 写入信任文件，所有服务器都需要执行
# chmod 600 /root/.ssh/authorized_keys

----- 测试能否登陆
$ ssh foobar@127.1
{% endhighlight %}

测试 ansible 时，需要将 `/etc/ansible/hosts` 文件配置为 127.1 的 IP 地址。

{% highlight text %}
$ cat /etc/ansible/hosts
127.1
$ ansible all -m ping -u foobar
127.1 | SUCCESS => {
    "changed": false,
    "ping": "pong"
}

----- 也可以直接修改配值文件的内容
$ cat /etc/ansible/hosts
foobar ansible_connection=local ansible_ssh_host=127.1 ansible_ssh_user=foobar

$ ansible all -u foobar -m command -a "date"     # 执行下测试命令
$ ansible all -u foobar -m shell -a "date"       # 同上
$ ansible all -m setup -u foobar                 # 查看配置
{% endhighlight %}

其中 `-m` 指定模块，`-u` 指定用户名。

### 主机的系统变量 (facts)

ansible 会通过 module setup 来收集主机的系统信息，这些收集到的系统信息叫做 facts，这些 facts 信息可以直接以变量的形式使用。可以在命令行上调用 setup module 命令，查看那些变量可以使用。

{% highlight text %}
$ ansible all -m setup -u foobar -a "filter=ansible_os_family"
{% endhighlight %}

然后可以在 playbook 中直接使用 facts 变量。

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

对于比较复杂的属性，可以通过如下的两种方式访问。

{% highlight text %}
----- 使用中括号
{ { ansible_ens3["ipv4"]["address"] } }

----- 使用点号
{ { ansible_ens3.ipv4.address } }
{% endhighlight %}

在 Playbook 中，可以通过 gather_facts 控制是否收集远程系统的信息，如果不需要直接如下填写即可。

{% highlight text %}
- hosts: whatever
  gather_facts: no
{% endhighlight %}

#### 自定义 facts

可以在 `/etc/ansible/facts.d` 目录下添加文件自定义 facts 。

{% highlight text %}
$ cat /etc/ansible/facts.d/foobar.fact
[general]
foobar = "Just For Test"

$ ansible all -m setup -u foobar -a "filter=ansible_local"
{% endhighlight %}


## Inventory

Ansible 中的 Inventory 文件定义了服务器的列表，默认情况下是 `/etc/ansible/hosts` 。

{% highlight yaml %}
# 主机
192.168.9.10               # IP地址
mail.foobar.com            # 使用FQDN表示，包括主机名+域名
jumper ansible_ssh_port=5555 ansible_ssh_host=192.168.9.50        # 将某些静态IP设置为别名
localhost ansible_connection=local                                # 指定选项

[webservers]               # 分组
web1.foobar.com
web2.foobar.com
web[10:50].foobar.com      # 表示从web10到web50，共计49台主机
web[a:f].foobar.com        # 表示从weba到webf
web.foobar.com ansible_connection=ssh ansible_ssh_user=light      # 指定选项
{% endhighlight %}

其中常用的选项包括了：

{% highlight text %}
ansible_ssh_host                # 指定ssh连接的IP或者FQDN
ansible_ssh_port                # ssh连接端口
ansible_ssh_user                # 默认ssh连接用户
ansible_ssh_pass                # ssh连接的密码，这种方式不安全，可以用--ask-pass，或公钥
ansible_sudo_pass               # sudo用户的密码
ansible_connection              # ssh连接的类型，包括local、ssh、paramiko
ansible_ssh_private_key_file    # ssh连接的公钥文件
ansible_shell_type              # 主机使用的shell解释器，默认sh，可以设置为csh、fish等
ansible_python_interpreter      # 用来指定python解释器的路径
{% endhighlight %}


## PlayBook

首先看个很简单的示例。

{% highlight text %}
$ cat foobar.yml
---
- hosts: all
  remote_user: root
  tasks:
    - name: test connection
      ping:
      remote_user: foobar             # 可以用不同的用户

$ ansible-playbook foobar.yml
{% endhighlight %}

playbook 是由一个或多个 "play" 组成的列表，其包括了如下的组件：

* hosts：指定要执行任务的主机，可以是一个或多个由冒号分割主机组。

* user：通过remote_user指定远程主机上的执行任务的用户。

* tasks：play主体部分是各个任务按次序逐个在hosts中指定的所有主机上执行。

* action：任务执行过程。

* handlers：用于当前关注的资源发生变化时采取一定指定的操作。

执行结果通过三种颜色表示：绿色代表执行成功，系统保持原样；黄色代表系统代表系统状态发生改变；红色代表执行失败，显示错误输出。

{% highlight text %}
# cat web.yml
- hosts: test                                # 选择执行命令的主机组，在/etc/ansible/hosts定义
  vars:                                      # 设置的变量
    http_port: 80
    max_clients: 200
  remote_user: root                          # 远端执行任务的用户，也可以使用sudo
  tasks:                                     # 任务
  - name: install httpd                      # 任务描述
    command: yum -y install httpd            # 调用ansible的command模块安装httpd
    when: ansible_distribution == "CentOS"   # 设置执行条件
  - name: provide httpd.conf                 # 任务描述
    copy: src="/root/httpd.conf" dest="/etc/httpd/conf/httpd.conf"
    tags: conf                               # 打标记用于单独执行标记任务，用ansible-playbook -C执行
    notify:                                  # 文件内容变更通知
    - server restart                         # 通知到指定的任务
  - name: server start                       # 任务描述
    service: name=httpd state=started enabled=true
  handlers:                                  # 定义接受关注的资源变化后执行的动作
  - name: server restart                     # 任务描述
    service: name=httpd state=restarted      # 当关注的资源发生变化后调用service模块
{% endhighlight %}

在执行时，首先会收集远端主机的元数据信息，也即 facts，包括了 IP 地址、CPU 核数、系统架构、主机名等信息，然后这些元数据可以作为变量供后面的 task 使用。

{% highlight text %}
$ ansible host -m setup -i inventory_file
{% endhighlight %}

当然可以通过 `gather_facts: no` 选项关闭，经常被用在条件语句和模板当中，可以使用条件判断语句关闭指定发行版的主机。

### Notify + Handler

ansible 还支持设置 handlers，也就是是在执行 tasks 之后可供调用的处理过程，使用起来如下，同样是本地执行:

{% highlight text %}
$ cat /etc/ansible/hosts
localhost ansible_connection=local ansible_ssh_host=127.1 ansible_ssh_user=foobar

$ echo "Hello World, Ansible" > /tmp/foobar

$ cat playbook.yml
---
- hosts: localhost       # hosts中指定
  tasks:
      - name: whoami
        copy: src=/tmp/foobar dest=/tmp/foobar.copy  #  本地拷贝到远端
        notify:  # 如果复制前发现foobar.copy文件改变了，则执行如下的handler
            - clear copy
  handlers:
      - name: clear copy
        shell: 'mv /tmp/foobar.copy /tmp/foobar.del'  # 假装删除
{% endhighlight %}

上述判断 foobar.copy 改变暂时还不太清楚如何做到的，无论是缺少，还是内容被修改，都会执行。

### 调试

对于 Playbook 可以通过如下方式调试。

{% highlight text %}
$ ANSIBLE_KEEP_REMOTE_FILES=1 ansible-playbook -i production site.yml \
 --tags git --step --start-at-task='git | setup' -vvvv
{% endhighlight %}

其中各个参数如下：

* ANSIBLE_KEEP_REMOTE_FILES=1 阻止删除远端的临时文件，可以用来查看。

* 通过 `--tags` 和 `--start-at-task` 设置执行那些任务，或者从那些任务开始。

* `--step` 表示单步执行，每执行完一个任务都会暂停。

* 使用多个 `-v` 参数，输出详细的调试信息。

另外，可以使用 debug 模块，方式如下：

{% highlight text %}
- name: debug this
  debug: var=result
{% endhighlight %}

还可以在配置文件中通过 log_path 指定日志文件路径。

### 输入密码

通过 ansible 连接远程主机时，默认是需要输入远程主机的密码的，这样会很不方便，可以有以下三种方式处理：

#### 1、通过 -k 提示输入密码

实际上该参数是 `--ask-pass` 的简写形式，不过这种方式依赖于 sshpass 命令。

{% highlight text %}
# yum --enablerepo=epel install sshpass
$ ansible all -m setup -k -u foobar -a "filter=ansible_os_family"
{% endhighlight %}

也可以在 playbook 文件中指定提示输入密码。

{% highlight text %}
hosts: all
remote_user: root
vars_prompt:
 - name: 'https_passphrase'          # 存储数据的变量名
   prompt: 'Key Passphrase'          # 手工输入数据
   private: yes                      # 当该值为yes，则用户的输入不会被打印
{% endhighlight %}

#### 2、 将用户名密码保存在 inventory 文件中

一般 inventory 文件保存在 `/etc/ansible/hosts` 文件里，书写格式如下。

{% highlight text %}
foobar ansible_ssh_host=127.1 ansible_ssh_user=root ansible_ssh_pass=yourpassword
{% endhighlight %}

#### 3. 使用SSH免秘钥登录

实际上就是通过 SSH 的公私钥验证方式。

### 其它技巧

为了灵活的控制 playbook，ansible 提供了 ansible-playbook 命令行工具，如下是一些常用的示例，更过用法可以参考 ansible-playbook -h 。

#### 指定开始 tasks

查看当前任务，然后指定从哪里开始执行任务。

{% highlight text %}
$ ansible-playbook --list-tasks foobar.yml
$ ansible-playbook --start-at-task="foo bar" foobar.yml
{% endhighlight %}

#### 只在某主机执行任务

可以通过如下方式执行，或者跳过某些任务。

{% highlight text %}
$ ansible-playbook --limit=host1 --tags=foobar foobar.yml
$ ansible-playbook --limit=host1 --skip-tags=foobar foobar.yml
{% endhighlight %}

## Vault

当使用 Ansible 自动化地维护服务器时，不可避免需要接触一些密码或其它敏感数据，我们可以使用三方的密码管理工具，如 HashiCorp 的 Vault、Square 的 Keywhiz、亚马逊的 Key Management Service 和微软 Azure 的 Key Vault。

当然也可以使用 Ansible 自带的 Vault 加密功能，可以将经过加密的密码和敏感数据同 Playbook 存储在一起，详细的内容可以查看


<!--
http://docs.ansible.com/ansible/playbooks_vault.html
http://www.ansible.com.cn/docs/playbooks_vault.html

Ansible Vault 的工作方式跟现实生活中的保险柜的工作方法很像：

    1.    我们可以把Ansible任务中用到的任意文件放入Vault保险柜中
    2.    Ansible Vault会使用密码来加密这些文件，就钥匙把保险柜的门关起来一样
    3.    我们把密码（钥匙）保存在一个只有我们自己知道或都有权访问的地址，与Playbook独立分开存储
    4.    在我们需要运行Playbook的时候，我们拿出密码（钥匙），解密敏感数据（打开保险柜门，拿出数据），就能正常的执行Playbook任务了

接下来，我们通过一个实例来了解一下这事个过程。下面的一段Playbook代码是要使用API key的方式来访问一个服务的API：

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

这种将key存储在纯文件文件中的做法非常之便捷，但是并不安全。如果我们使用Ansible Tower和Jenkins等工具来运行Playbook，或者Playbook在一个共享的环境中时，这种存储key的方法就更不可取。或许我们有非常严格的主机操作和系统安全规范，但是我们并不能保证每一位开发者或管理员都能严格的遵守，人通常是整个环节中最不稳定的因素。Ansible Vault可以为我们提供非常高的安全加密级别，这将很好帮我们解决后顾之忧。

    使用如下命令，可以利用Vault给文件加密：

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

    除了encrypt选项之外，关于ansible-vault命令有几个比较常用的选项，列举如下：

        edit: 用于编辑经过ansible-vault加密过的文件

        rekey: 重新修已被加密文件的密码

        create: 创建一个新文件，并直接对其进行加密

        view: 查看经过加密的文件

        decrypt: 解密文件

            以上所有选项都可以在后面是时跟多个文件进行操作，比如：ansible-vault create x.yml y.yml z.yml

除了手动输入密码进行解密以外，Ansible还提供了以密码文件形式来解密的认证方式，这类似于SSH的密钥认证。SSH将密钥放于~/.ssh目录下面，Ansible Vault将密码文件放置于~/. ansible/，同时对于这个文件也必须有严格的权限控制，需设置其权限为600。现在我们可以~/.ansible/目录下创建一个权限为600的纯文件文件vault_pass.txt，并写入我们的Vault密码，使用如下命令就可非交互式的使用被加密过的Playbook运行任务了：

$ ansible-playbook test.yml --vault-password-file ~/.ansible/vault_pass.txt

我们也可以使用可执行脚本来生成单行的Vault密码，比如∼/.ansible/vault_pass.py，前提是该脚本只能生成一行密码数据。
如果系统上通过pip install cryptography命令安装了Python的cryptography模块，那么这将会加快Vault的运行速度。

    你是否在为Ansbile Vault的加密方式感到不放心？Ansible Vault采用AES-256加密算法对文件进行加密，这种自满是极其安全的。即使是使用目前世界上最运算速度最快的集群来7x24小时的去解密一个通过该算法加密的文件，也需要数十亿年的时间才能完成破解。所以，安全算法是没有问题的，我们要做的就是保管好自己的Vault加密密码。
    可以官方文档中参考更的选项用法及详细案例：http://docs.ansible.com/ansible/playbooks_vault.html

第二章、变量优先级

通过本章节一系列对变量的学习，可能会有人问了，定义变量的方式有那么多，我究竟该如何确定哪一个定义会最终生效呢？下面我们就来理一下变量优先级的问题。如果同样名称的变量在多个地方被定义,那么究竟以哪一次定义的值为准呢？Ansible官方给出了如下优先级排序:

    1、在命令行中定义的变量（即用-e定义的变量）优先级最高

    2、在inventory中定义的连接变量(比如ansible_ssh_user)

    3、大多数的其它变量(命令行转换,play中的变量,included的变量,role中的变量等)

    4、在Inventory定义的其它变量

    5、由系统通过gather_facts方法发现的Facts

    6、“Role默认变量”, 这个是最默认的值,很容易丧失优先权

通过一段时间的使用之后呢，大部分人都会有自己的一套定义变量的方法或习惯。下面我们总结一些变量定义方面的小技巧，希望可以给初学者提供一些有益的参考：

    Role(下节即将讲到)中的默认变量应设置的尽可能合理，因为他优先级最低，以防这些亦是在其他地方都没被定义，而Role的默认亦是又定义的不合理而产生问题。

    Playbook中应尽量少的定义变量，Playbook中用的变量应尽量定义在专门的变量文件中通过vars_files引用，或定义在Inventory文件中。

    只有真正与主机或主机组强相关的变量才定义在Inventory文件中。

    应尽量少的在动态或静态的Inventory源文件中定义变量，尤其是不要定义那些很少在Playbook中被用的变量。

    应尽量避免在命行中使用-e选项来定义变量。只有在我们不用去关心项目的可维护性和任务幂等性的时候，才建议全用这种变量定义方式。比如只是做本地测试或者运行一个一次性的Playbook任务

关注我们
-->


## Python API

详细的可以参考官方文档 [Python API](http://docs.ansible.com/ansible/developing_api.html)，在 2.0 之前的版本，代码非常简单，如下：


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

# 指定选项，可以在启动时指定，或者在/etc/ansible/ansible.cfg中指定
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

## 源码解析

实际上 ansible 安装后，ansible-XXX 都是指向 ansible 的可执行文件，不过调用的时候会在 ansible 中进行判断，根据不同的命令调用 cli 目录下的文件。


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


## 参考

可以查看中文帮助 [www.ansible.com.cn](http://www.ansible.com.cn)，源码参考 [github - ansible](https://github.com/ansible/ansible) 。可以查看 [ansible 中文文档](/reference/linux/Ansible-中文手册.pdf)，其中有介绍一个不错的运维监控工具的总结。

详细的文档可以参考 [Ansible Documentation](http://docs.ansible.com/ansible/) 。


<!--
http://docs.ansible.com/ansible/playbooks_best_practices.html

ansible常见用法为 ansible host-pattern -m 模块 -a 命令 ，host-pattern类似于简化的正则表达式，而模块可以通过 ansible-doc -l 命令来查询。下面是一些常用模块的使用方法：

http://hakunin.com/six-ansible-practices
-->


{% highlight text %}
{% endhighlight %}
