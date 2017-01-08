# 利用iptables給Docker綁定一個外網IP


##背景

由於Docker默認是不能夠與外部進行直接的通信，比較普遍的彷彿是利用啟動時-p來與主機進行端口映射與外界溝通。但是有時候在有其他需求時並不太方便，特別是在進行一些docker打包之前的內部開發時，希望其能夠像虛機一樣能夠與外部有很好的通信，便希望其能夠綁定外部的IP地址。 
docker默認的網絡是橋接在創建好後的網橋docker0上的。docker0默認的典型地址為172.17.42.1，子網掩碼為255.255.0.0。之後啟動容器會給容器分配一個同一網段（172.17.0.0/16）的地址。然後啟動docker容器時會創建一對veth pair。其中一端為容器內部的eth0，另外一端為掛載到docker0網橋並以veth開頭命名。如下所示：

```sh
#brctl show
bridge name bridge id STPenabled interfaces
docker0 8000.56847afe9799 no veth135f096
veth5f8fe2d
```

通過這種方式，容器可以跟主機以及容器之間進行通信，主機和容器共享虛擬網絡。 
在做開發等時候，可能希望容器能夠像虛機一樣遠程登錄與訪問，這時候就需要給容器再綁定一個外部IP地址，這時候即可考慮採用iptables進行端口轉發來實現對於容器的外部IP綁定。

##環境

一台ubuntu的服務器，所屬網段為10.50.10.0/26，該服務器的IP為10.50.10.26，由於還使用了ovs，因此是該IP是在br-ex上的； 

一個ubuntu的容器，IP為172.17.0.1/26 。

##目的

通過iptables為IP為172.17.0.1的容器綁定外部IP 這裡為10.50.10.56.

##過程

整個過程大致分為3部分：

###1. 為主機綁定多個IP地址

這一步可以通過如下命令來給網橋綁定另外一個IP：

```sh
#ifconfig br-ex:010.50.10.56/24
```

如果希望重啟機器後仍然能夠生效，需要將其寫入到/etc/network/interfaces中。

###2. iptables設置DNAT

通過DNAT來重寫包的目的地址，將指向10.50.10.56的數據包的目的地址都改為172.17.0.1，這樣即可以

```sh
#iptables -t nat -A PREROUTING -d 10.50.10.56 -p tcp -m tcp --dport 1:65535 -j DNAT--to-destination 172.17.0.1:1-65535
```

###3. iptables設置SNAT

重寫包的源IP地址，即在容器中收到數據包之後，將其源改為docker0的地址。

```sh
#iptables -t nat -A POSTROUTING -d 172.17.0.1 -p tcp -m tcp --dport 1:65535 -j SNAT --to-source172.17.42.1
```

##保存規則

如果希望保存下來，需要通過命令：

```sh
#/etc/init.d/iptables save
```

來進行保存。

##刪除規則

當然如果想刪除該規則，也可以通過

```sh
# iptables –t nat –D PREROUTING <number>
# iptables –t nat –D POSTROUTING <number>
```

來將創建的這兩條規則刪除。

##驗證

首先通過iptables來查看是否生效。

```sh
# iptables -n -t nat -L
Chain PREROUTING (policyACCEPT)
target prot opt source destination
DNAT tcp -- 0.0.0.0/0 10.50.10.56 tcp dpts:1:65535to:172.17.0.1:1-65535
Chain INPUT (policyACCEPT)
target prot opt source destination
Chain OUTPUT (policyACCEPT)
target prot opt source destination
DOCKER all -- 0.0.0.0/0 !127.0.0.0/8 ADDRTYPE match dst-type LOCAL
Chain POSTROUTING (policyACCEPT)
target prot opt source destination
MASQUERADE all -- 10.50.10.0/26 0.0.0.0/0
MASQUERADE all -- 172.17.0.0/16 0.0.0.0/0
SNAT tcp -- 0.0.0.0/0 172.17.0.1 tcp dpts:1:65535 to:172.17.42.1
```

然後可以通過安裝ssh或者apache2等需要使用端口的服務來進行驗證。

當然實現這個功能會有很多種方法，歡迎大家來拍磚～

##參考

- 1，《Docker——從入門到實踐》：高級網絡配置 
http://dockerpool.com/static/books/docker_practice/advanced_network/README.html 
- 2，The netfilter/iptables HOWTO's 
http://www.netfilter.org/documentation/index.html 
- 3，Iptables 指南
http://man.chinaunix.net/network/iptables-tutorial-cn-1.1.19.html