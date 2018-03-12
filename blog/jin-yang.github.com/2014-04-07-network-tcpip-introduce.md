---
title: TCP/IP 协议简介
layout: post
comments: true
language: chinese
category: [linux, network]
keywords: linux,tcp,ip,协议,面向链接
description: TCP 协议是 "面向连接" 的，整个过程被分为几个阶段：准备、协商、建立连接、管理以及最后的连接终止，同时可以管理多个链接。接下来，我们看看 TCP 协议是如何实现的。
---

TCP 协议是 "面向连接" 的，整个过程被分为几个阶段：准备、协商、建立连接、管理以及最后的连接终止，同时可以管理多个链接。

接下来，我们看看 TCP 协议是如何实现的。

<!-- more -->

## 简介

如上，TCP 可以同时管理多个连接，因此需要赋予每个连接一个唯一的标识进行识别，这个标识就是连接两端的 IP 以及端口号 (共4个key)，从而保证了连接后续的建立、管理和中断都是独立分开的。

对于各个状态迁移采用 FSM (Finite State Machine，有限状态机) 进行描述，其包含了如下的几种状态：

* state：在一个给定的时间，协议应用（protocol software）的环境以及状态的详细说明；
* transition：从一个状态到另一个状态的动作；
* event：引起状态间切换的事件；
* action：设备在切换到另一个状态之前，所做的对于 event 的回应。

下图是一个 TCP 有限状态机的简化版本。

![TCP/IP Finite State Machine FSM]({{ site.url }}/images/network/tcpip_The-TCP-Finite-State-MachineFSM.png "TCP/IP Finite State Machine FSM"){: .pull-center}

### TCB

由于每个连接都是独立的，所以我们使用了一个定制的数据结构来将与连接相关的信息分别存储，这个就是 TCB (Transmission Control Block，传输控制模块)。

在连接启动建立之前，内核会做一些准备工作，其中之一就是建立起维护连接信息的 TCB ，而且两端的 TCB 都是由两端自己的设备去维护。

TCB 保存了与连接相关的所有的重要信息，比如两端端口号，用于连接识别；用于管理连接数据流的 buffer 的内存指针；TCB 也用于窗口切换机制，用来保存所接收到的数据中有哪些已经被确认和尚未被确认的变量，当前窗口大小等等。

### 报文头部

按照 OSI 的七层模型，ARP 工作在第二层 (Data Link，Frame)，IP 在第三层 (Network, Packet)，第四层 (Transport, Segment)。数据依次打包，传到对端后，各个层解析自己的协议，然后把数据交给更高层的协议处理。

![TCP/IP Protocal Header]({{ site.url }}/images/network/tcpip_TCP-Header.png "TCP/IP Protocal Header"){: .pull-center}


<!--
1. 源端口号：数据发起者的端口号，16bit
2. 目的端口号：数据接收者的端口号，16bit
3. 序号：32bit的序列号，由发送方使用
4. 确认序号：32bit的确认号，是接收数据方期望收到发送方的下一个报文段的序号，因此确认序号应当是上次已成功收到数据字节序号加1。
5. 首部长度：首部中32bit字的数目，可表示15*32bit=60字节的首部。一般首部长度为20字节。
6. 保留：6bit, 均为0
7. 紧急URG：当URG=1时，表示报文段中有紧急数据，应尽快传送。
8. 确认比特ACK：ACK = 1时代表这是一个确认的TCP包，取值0则不是确认包。
9. 推送比特PSH：当发送端PSH=1时，接收端尽快的交付给应用进程。
10. 复位比特（RST）：当RST=1时，表明TCP连接中出现严重差错，必须释放连接，再重新建立连接。
11. 同步比特SYN：在建立连接是用来同步序号。SYN=1， ACK=0表示一个连接请求报文段。SYN=1，ACK=1表示同意建立连接。
12. 终止比特FIN：FIN=1时，表明此报文段的发送端的数据已经发送完毕，并要求释放传输连接。
13. 窗口：用来控制对方发送的数据量，通知发放已确定的发送窗口上限。
14. 检验和：该字段检验的范围包括首部和数据这两部分。由发端计算和存储，并由收端进行验证。
15. 紧急指针：紧急指针在URG=1时才有效，它指出本报文段中的紧急数据的字节数。
16. 选项：长度可变，最长可达40字节
-->

上图是 TCP 头部，来自 [nmap tcpip-ref](http://nmap.org/book/tcpip-ref.html)，需要注意的是 A) TCP 包是没有 IP 地址，那是 IP 层的事，但是有源端口和目标端口。

上图中比较重要的是，A) Sequence Number 是包的序号，用来解决网络包乱序问题；B) Acknowledgement Number 用于确认收到包，用来解决丢包问题；C) Window 又叫 Advertised-Window 或者 Sliding Window 滑动窗口，用于解决流控；D) TCP Flag ，也就是包的类型，主要是用于操控 TCP 的状态机的。

## 三次握手，建立连接

三次握手主要用于建立连接，主要过程如下所示。

![TCP/IP Three Way Handshaking]({{ site.url }}/images/network/tcpip_Three-Way-Handshaking.png "TCP/IP Three Way Handshaking"){: .pull-center}

简单看下连接的建立过程，在建立链接之前，server 已经处于 LISTEN 状态，而客户端处于 CLOSED 状态。

1. 第一次握手，客户端设置 TCB 并发送 SYN(syn=j) 包到服务器，尝试建立连接，而客户端进入 SYN-SENT 状态，等待服务器确认。
2. 第二次握手，服务器收到 SYN 包，必须确认客户的 SYN(ack=j+1)，同时自己也发送一个 SYN (syn=k) 包，也即 SYN+ACK 包，此时服务器进入 SYN_RECEIVED 状态，同时将该链接保存在半连接队列。
3. 第三次握手，客户端收到服务器的 SYN+ACK 包，向服务器发送确认包 ACK(ack=k+1)，客户端进入 ESTABLISHED 状态；当服务器接收到 ACK 后也进入 ESTABLISHED 状态，完成三次握手。

注意，对于服务端会将 ESTABLISHED 状态的连接移入 accept 队列，等待应用调用 accept() 。

可以看到，在服务端，建立连接涉及两个队列：

* 半连接队列，保存了处于 SYN_RECV 状态的连接，表示服务器已经收到 SYN 包，并向客户发出确认，在等待客户端的确认。队列长度由 net.ipv4.tcp_max_syn_backlog 设置，一般默认值为 512 或者 1024，超过这个数量，系统将不再接受新的 TCP 连接请求。
* 连接队列，保存 ESTABLISHED 状态的连接。队列长度为 min(net.core.somaxconn, backlog)，其中 backlog 是调用 listen(int sockfd, int backlog) 时设置。

### 序列号同步

通过 TCP 的三次握手，除了建立链接之外，设备上的 TCP 层也进行一些必须的信息交换，包括了 A) 第一次数据传输时使用的序列号，B) 有关如何操作该连接的一些参数。这一过程一般被称为 “序列号同步(Sequence Number Synchronization)”，这是建立连接过程中一个很重要的部分。

![TCP/IP Server Client Process]({{ site.url }}/images/network/tcpip_server-client-process.jpg "TCP/IP Server Client Process"){: .pull-center  width="50%"}

在建立链接时，双方需要相互通知自己的初始序列号 (Inital Sequence Number, ISN)，也就是 Synchronize Sequence Numbers。如上图中的 x 和 y，该值会作为以后的数据通信的序号，以保证应用层接收到的数据不会因为网络上的传输的问题而乱序，TCP 也就是用这个序号来拼接数据。

可以通过如下图的方式查看序列号是如何变化的，也就是使用 Wireshark 菜单中的 Statistics => Flow Graph… 。

![TCP/IP Sequence Number Process]({{ site.url }}/images/network/tcpip_sequence-number-process.jpg "TCP/IP Sequence Number Process"){: .pull-center }

你可以看到，序列号的增加是和传输的字节数相关的。上图中，三次握手后，来了两个长度为 1440 的包，而第二个包的 SeqNum 就成了1441，然后第一个 ACK 回的是 1441，表示第一个 1440 收到了。

注意：Wireshark 抓包程序看3次握手，会发现SeqNum总是为0，这是为了显示更友好，使用了相对序号，只要在右键菜单中的 protocol preference 中取消掉就可以看到 “Absolute SeqNum” 了。

更加详细的可以参考 [查看 TCP/IP 三次握手](/blog/network-wireshark.html)，或者参考 [Understanding TCP Sequence and Acknowledgment Numbers](http://packetlife.net/blog/2010/jun/7/understanding-tcp-sequence-acknowledgment-numbers/)
，或者 [本地文档](/reference/linux/network/Understanding TCP Sequence and Acknowledgment Numbers.mht) 。

### 序列号相关

有一个问题，我们为什么不能每次都把数据的第一位的序号编为 1，或者一个固定值 ? 如果采用固定的序列号，可能会导致不同链接的数据流混乱。

假设我们在 A 和 B 之间建立起了一个连接， A 向 B 发送了一个 byte1 到 byte30 的消息，但是此时网络出现问题，这个数据流被延迟了，最后，这个连接中断。此时我们重启了 A 到 B 的连接， A 向 B 发送了一个以 1 为序列开头的数据流，然而此时可能之前的 byte1 到 byte30 发送到了 B ， B 也会认为这 30 个 byte 可能就是来自于后来新连接从而接受，那么此时就出现了错误。

为此，在 RFC793 中规定，ISN 会使用一个与时间绑定的计数器，在 TCP 层启动的时候初始化，每 4 微秒自增 1，直到溢出归 0，这样，一个 ISN 周期大约是 4.55 小时。

因为，我们假设我们的 TCP Segment 在网络上的存活时间不会超过 Maximum Segment Lifetime，只要 MSL 的值小于 4.55 小时，那么，我们就不会重用到 ISN。

这种解决方式会带来另外一个问题，就是 ISN 变得可以被预测：可以通过分析连接的之前的 ISN 然后预测后来的 ISN ，这就造成了一个安全隐患，这个漏洞在过去曾经被利用发起国著名的 Mitnick 攻击。为了解决这个问题，现在使用的随机数代理的自增。

<!--

<h3>TCP序列号的协商</h3>
<p>设备选好自己使用的 ISN 以后，就会把 ISN 的值填在 SYN 消息的序列号区中，并发送给对端，对端接收到以后，会回应一个 ACK （也可能包含自己的 SYN ），在 ACK 消息中，在“确认编号”区会设一个值，这个值等于接收到的 SYN 的 ISN 的值加一。这代表着设备接收到的下一个序列号来自于自己的对端。ISN 实际代表着最后收到的 byte 的序列号（假设连接是新建立并且上没有数据传输的情况下），具体这两个域是如何控制数据传输的，会在后面做介绍。<br><br>

关键点：作为连接建立就绪过程中的一部分， TCP 连接的两端都会告知对方自己在做第一次传输的时候将是用什么序列号，告知方式是将这个值放在 SYN 消息头部的序列号区域。对端收到以后，会对其进行确认，确认方式就是在 ACK 消息的头部的“确认编号”区域填入一个增加 1 后的数值。这个过程就叫做通讯序列号的协商。<br><br>

这里有一个简单的例子描述三次握手过程中的序列号是如何工作的，我选用了较小 ISN 方便描述，但是要记住 ISN 可以是一任意一个 32 位二进制的值。

<ol type="A"><li>
Client发出连接请求：Client 选在一个 ISN 给其传输使用，假设为 4567 。那么 Client 会发送一个序列号域值为 4567 的 SYN 消息。</li><br><li>

Server 进行确认并发出连接请求： Server 使用 12998 作为其连接请求的 ISN 。接收到 Client 的 SYN 以后，它会发送一个 SYN+ACK ，其中 ACK 的确认编号区的值为 4568（自增 1 ）， SYN 的序列号域值为 12998 。</li><br><li>

Client 进行确认：Client 返回个 Server 一个 ACK 消息， ACK 消息的确认编号区的值为 12999（Server 的 SYN 消息 ISN 自增 1 ）。</li></ol>
<center><img src="pictures/Three-Way-Handshaking-Sequence.png" height="410" width="450"></center><br>
现在连接就已经就绪了，准备好传输数据。 Client 发送数据的第一个 byte 的编号就会是 4568 ， Server 的第一个则会是 12999 。</p>
-->


### SYN 超时与 SYN Flood 攻击

在建立链接时，如果服务端接到了客户端发的 SYN 后回了 SYN-ACK 后客户端掉线了，服务端没有收到客户端回来的 ACK，那么这个连接处于一个中间状态，即没成功，也没失败。于是，服务端对于在一定时间内没收到 ACK 的请求会重发 SYN-ACK。

在 Linux 中，默认重试次数为 5 次，重试的间隔时间分别为 1s, 2s, 4s, 8s, 16s，在第 5 次发出后还要等 32s 都知道第 5 次也超时了，所以，总共需要 1s + 2s + 4s+ 8s+ 16s + 32s = 2^6 -1 = 63s，TCP 才会把断开这个连接。

SYN Flood 攻击就是给服务器发了一个 SYN 后，就下线了，于是服务器需要默认等 63s 才会断开连接，这样，攻击者就可以把服务器的 SYN 连接的队列耗尽，让正常的连接请求不能处理。

对于正常请求，应该调整三个 TCP 参数：A)  tcp_synack_retries 减少重试次数；B) tcp_max_syn_backlog 增大SYN连接数；C) tcp_abort_on_overflow 处理不过来干脆就直接拒绝连接了。


### TCP SYNC cookies

SYN Cookie 是对 TCP 服务器端的三次握手协议作一些修改，专门用来防范 SYN Flood 攻击。

其原理是，在 TCP 服务器收到 TCP SYN 包并返回 TCP SYN+ACK 包时，不分配一个专门的数据区，而是根据这个 SYN 包计算出一个 cookie 值。在收到 TCP ACK 包时，TCP 服务器在根据那个 cookie 值检查这个 TCP ACK 包的合法性。如果合法，再分配专门的数据区进行处理未来的 TCP 连接。

于是，Linux下给了一个叫tcp_syncookies的参数来应对这个事——当SYN队列满了后，TCP会通过源地址端口、目标地址端口和时间戳打造出一个特别的Sequence Number发回去（又叫cookie），如果是攻击者则不会有响应，如果是正常连接，则会把这个 SYN Cookie发回来，然后服务端可以通过cookie建连接（即使你不在SYN队列中）。请注意，请先千万别用tcp_syncookies来处理正常的大负载的连接的情况。因为，synccookies是妥协版的TCP协议，并不严谨。

## 四次握手，关闭连接

关闭相对来说要复杂，这是由于两端设备都在相互的发送/接收数据，当一端告诉对端 “我打算关闭这个连接” 时，而对端以及相应的应用程序可能完全不知道自己的对端正在打算关闭这个连接，所以为了能够优雅的关闭当期的连接并且还要保证不会产生数据丢失，这个步骤就会相对的复杂一些。

通过信息同步，当两端的程序都意识到 “这个连接就要被关闭” 并且已经停止发送数据的时候才能真正的开始着手关闭这个连接。而对于连接的一端来说，连接的关闭意味着自己停止发送数据，但是还可以接受从对端传来的数据，直到对端也停止发送。

说是四次握手，而实际上是两次，这是因为 TCP 是全双工的，发送方和接收方都需要 FIN 和 ACK，只不过，有一方是被动的，所以看上去就成了所谓的四次挥手。如果两边同时断连接，那就会就进入到 CLOSING 状态，然后到达 TIME_WAIT 状态。

### 标准连接关闭过程

两端通过发送 FIN 消息来告知对端自己将关闭连接，当对端接收到 FIN 消息以后，会返回一个 ACK 消息告知 FIN 消息已经收到。只有当双方都发送了 FIN 并且接收到了 ACK 才会被认为已经完全的断开。

![TCP/IP Four Way Handshake]({{ site.url }}/images/network/tcpip_Four-Way-Handshake.png "TCP/IP Four Way Handshake"){: .pull-center}

### 同时关闭

如下图所示，在这个过程中有很多是相对称的地方，状态对称，动作和结果也对称。在 CLOSED 状态时，也就是没有连接时， TCP 层会保证所有重要的数据已经发送到应用层，有时候可以理解成是一个 “push” 。 TCB 也会在连接关闭完成后被销毁。

关键点：就像连接可以通过两端同时主动 OPEN 建立，连接也可以通过两端同时中断来完成关闭。在这种情况下，转换过程会有一些不同，两端的设备都是先用 ACK 回复对端发来的 FIN ，然后再等待接受返回的 ACK ，最后再经过 2 倍 MSL 来确保消息和数据已经传输完成，终于关闭了连接。

![TCP/IP Four Way Handshake Sync]({{ site.url }}/images/network/tcpip_Four-Way-Handshake-Sync.png "TCP/IP Four Way Handshake Sync"){: .pull-center}

### CLOSE-WAIT 和 TIME-WAIT

收到 FIN 消息的服务器可能会在 CLOSE-WAIT 状态等待相当长的一段时间，直到对应的应用程序告诉它 “我已经准备好关闭连接了”。TCP 不能假设自己会需要等待多久，在这一段时间里， Server 可能仍在发送数据，而 Client 则一直在接受，但是 Client 已经不会再发给 Server 数据了。

到最后，Server 终于发出了 FIN 消息要关闭自己这一端的连接了，那么起初发送 FIN 消息的机器即 Client 会返回一个 ACK 进行确认。但是 Client 不能在发送完 ACK 立刻就进入 CLOSED 状态，因为它必须要等待 ACK 确确实实的已经发送到了对端，正常情况下这个过程很快，但是还是要考虑不正常情况可能造成的延迟。

从 TIME_WAIT 到 CLOSED 有一个 2*MSL 的超时设置 (RFC793 定义为 2min，而 Linux 设置为 30s)，这么作主要有两个原因：

* 要确保足够长的时间能让 ACK 到达连接对端，并且还有可能会要求重传，如果被动关闭的那方没有收到 ACK，就会触发被动端重发 FIN，一来一去正好 2 个 MSL。

<!--
这是因为：虽然双方都同意关闭连接了，而且握手的 4 个报文也都协调和发送完毕，按理可以直接回到 CLOSED 状态；但是因为我们必须要假想网络是不可靠的，你无法保证你最后发送的 ACK 报文会一定被对方收到，因此对方处于 LAST_ACK 状态下的 SOCKET 可能会因为超时未收到 ACK 报文，而重发 FIN 报文，所以这个 TIME_WAIT 状态的作用就是用来重发可能丢失的 ACK 报文，并保证于此。<br><br>

如果这时被动方会重新发 FIN ，如果这时主动方处于 CLOSED 状态，就会响应 RST 而不是 ACK，所以主动方要处于 TIME_WAIT 状态，而不能是 CLOSED 。
-->

* 第二个原因就是需要为即将关闭的当前连接和后继的新连接之间提供一个 "缓冲间隔"，避免不同链接数据造成混乱的可能。主要防止上一次连接中的包，迷路后重新出现，影响新连接 (经过2MSL，上一次连接中所有的重复包都会消失)。

TCP 标准规定 TIME-WAIT 时间是两倍的 MSL，在现在的网络中，这是一个常量，所以 TCP 层允许程序使用一个较小的值去代替，只要能保证工作正常即可。

MSL (最大分段生存期) 用以指明 TCP 报文在 Internet 上最长生存时间，每个具体的 TCP 实现都必须选择一个确定的 MSL 值。RFC1122 建议是 2 分钟，但 BSD 传统实现采用了 30 秒。

可以参考 [TIME_WAIT and its design implications for protocols and scalable client server systems](http://www.serverframework.com/asynchronousevents/2011/01/time-wait-and-its-design-implications-for-protocols-and-scalable-servers.html)，或者 [本地文档](/reference/linux/network/TIME_WAIT and its design implications for protocols and scalable client server systems - AsynchronousEvents.mht) 。

## 滑动窗口

TCP 需要保证数据传输的质量，会对发送的数据进行跟踪，解决可靠传输以及包乱续的问题。这种数据管理主要有以下两大关键功能：A) 可靠性，保证数据确实到达目的地，未到达，则能够发现并重传；B) 数据流控，管理数据的发送速率，以使接收设备不致于过载。

要完成这些任务，整个协议围绕滑动窗口确认机制来进行。所以，TCP 必需要知道网络实际的数据处理带宽或是数据处理速度，这样才不会引起网络拥塞，导致丢包。所以，TCP 引入了一些技术来做网络流控，滑动窗口就是其中之一。

TCP 头有一个字段 Window，用于接收端告诉发送端自己还有多少缓冲区可以接收数据，于是发送端就可以根据这个接收端的处理能力来发送数据，而不会导致接收端处理不过来。

![TCP/IP Slide Window]({{ site.url }}/images/network/tcpip-slide-window-struct.jpg "TCP/IP Slide Window"){: .pull-center width="70%" }

上图是发送和接收端的相关数据结构，可以看到：

* 接收端 LastByteRead 指向了 TCP 缓冲区中读到的位置，NextByteExpected 指向的是收到的连续包的最后一个位置，LastByteRcved 指向的是收到的包的最后一个位置，我们可以看到中间有些数据还没有到达，所以有数据空白区。
* 发送端的 LastByteAcked 指向了被接收端 Ack 过的位置（表示成功发送确认），LastByteSent 表示发出去了，但还没有收到成功确认的 Ack，LastByteWritten 指向的是上层应用正在写的地方。

于是，接收端在给发送端回 ACK 中会汇报自己的 `AdvertisedWindow = MaxRcvBuffer - LastByteRcvd - 1`；而发送方会根据这个窗口来控制发送数据的大小，以保证接收方可以处理。

![TCP/IP Slide Window]({{ site.url }}/images/network/tcpip-slide-window-send.png "TCP/IP Slide Window"){: .pull-center width="90%" }

上图中的黑框就是滑动窗口，整个缓冲区被分成了四个部分：#1 已收到 ACK 确认的数据；#2 已经发送但还没收到 ACK；#3 在窗口但还没有发出，接收方还有空间；#4 窗口以外的数据，接收方没空间。

### Zero Window

![TCP/IP Zero Window]({{ site.url }}/images/network/tcpip-tcpswflow.png "TCP/IP Zero Window"){: .pull-center width="70%" }

上图是一个处理缓慢的 Server 把 Client 的 TCP Sliding Window 给降成 0 ，此时 Client 将不再发送数据，如果接收方一会儿 Window Size 可用了，怎么通知发送端呢？

TCP 使用 Zero Window Probe, ZWP 技术，也就是说，发送端在窗口变成 0 后，会发 ZWP 包让接收方响应它的 Window 大小，一般会重试 3 次，第次间隔约 30-60s，如果 3 次后仍为 0 ，有的协议实现就会发 RST 把链接断了。

<!--
注意：只要有等待的地方都可能出现DDoS攻击，Zero Window也不例外，一些攻击者会在和HTTP建好链发完GET请求后，就把Window设置为0，然后服务端就只能等待进行ZWP，于是攻击者会并发大量的这样的请求，把服务器端的资源耗尽。（关于这方面的攻击，大家可以移步看一下Wikipedia的SockStress词条）
另外，Wireshark中，你可以使用tcp.analysis.zero_window来过滤包，然后使用右键菜单里的follow TCP stream，你可以看到ZeroWindowProbe及ZeroWindowProbeAck的包。
-->

### Silly Window Syndrome

如果接收方太忙了，来不及取走 Receive Windows 里的数据，那么，就会导致发送方越来越小；到最后，如果接收方只能腾出几个字节并告诉发送方现在有几个字节的 window，而发送方会义无反顾地发送这几个字节。

<!--
要知道，我们的TCP+IP头有40个字节，为了几个字节，要达上这么大的开销，这太不经济了。

另外，你需要知道网络上有个MTU，对于以太网来说，MTU是1500字节，除去TCP+IP头的40个字节，真正的数据传输可以有1460，这就是所谓的MSS（Max Segment Size）注意，TCP的RFC定义这个MSS的默认值是536，这是因为 RFC 791里说了任何一个IP设备都得最少接收576尺寸的大小（实际上来说576是拨号的网络的MTU，而576减去IP头的20个字节就是536）。

如果你的网络包可以塞满MTU，那么你可以用满整个带宽，如果不能，那么你就会浪费带宽。（大于MTU的包有两种结局，一种是直接被丢了，另一种是会被重新分块打包发送） 你可以想像成一个MTU就相当于一个飞机的最多可以装的人，如果这飞机里满载的话，带宽最高，如果一个飞机只运一个人的话，无疑成本增加了，也而相当二。

所以，Silly Windows Syndrome这个现像就像是你本来可以坐200人的飞机里只做了一两个人。 要解决这个问题也不难，就是避免对小的window size做出响应，直到有足够大的window size再响应，这个思路可以同时实现在sender和receiver两端。

如果这个问题是由Receiver端引起的，那么就会使用 David D Clark’s 方案。在receiver端，如果收到的数据导致window size小于某个值，可以直接ack(0)回sender，这样就把window给关闭了，也阻止了sender再发数据过来，等到receiver端处理了一些数据后windows size 大于等于了MSS，或者，receiver buffer有一半为空，就可以把window打开让send 发送数据过来。

如果这个问题是由Sender端引起的，那么就会使用著名的 Nagle’s algorithm。这个算法的思路也是延时处理，他有两个主要的条件：1）要等到 Window Size>=MSS 或是 Data Size >=MSS，2）收到之前发送数据的ack回包，他才会发数据，否则就是在攒数据。

另外，Nagle算法默认是打开的，所以，对于一些需要小包场景的程序——比如像telnet或ssh这样的交互性比较强的程序，你需要关闭这个算法。你可以在Socket设置TCP_NODELAY选项来关闭这个算法（关闭Nagle算法没有全局参数，需要根据每个应用自己的特点来关闭）
1 setsockopt(sock_fd, IPPROTO_TCP, TCP_NODELAY, (char *)&value,sizeof(int));

另外，网上有些文章说TCP_CORK的socket option是也关闭Nagle算法，这不对。TCP_CORK其实是更新激进的Nagle算汉，完全禁止小包发送，而Nagle算法没有禁止小包发送，只是禁止了大量的小包发送。最好不要两个选项都设置。
-->

## 拥塞控制

TCP 通过 Sliding Window 来做流控 (Flow Control)，但是这种方式依赖于连接的发送端和接收端，并不知道网络中间发生了什么。

<!--
TCP的设计者觉得，一个伟大而牛逼的协议仅仅做到流控并不够，因为流控只是网络模型4层以上的事，TCP的还应该更聪明地知道整个网络上的事。
-->

## 参考

关于 TCP/IP 协议可以参考 《TCP/IP 详解 卷1：协议》，以及 RFC793 等一系列的标准，如下是一些比较经典的参考。

* [IP_TCP_UDP_Header 本地文档](/reference/linux/network/IP_TCP_UDP_Header.pdf)，包括了 IP、TCP、UDP 头部信息，以及 TCP 状态转换，也就是比较经典的格式。

* [网络基本功](http://blog.csdn.net/xiongyingzhuantu/article/details/38665549) 一系列网路基本知识的介绍，包括了整个链路；[TCP 的那些事儿](http://coolshell.cn/articles/11564.html) 在 CoolShell 中的关于 TCP 协议的介绍，包括上述的图片。

* [The TCP/IP Guide](htp://www.tcpipguide.com/index.htm) 很好的介绍 TCP 概念的资料，可以参考 [本地文档](/reference/linux/The TCP-IP Guide - A Comprehensive Illustrated Internet Protocols Reference.pdf)，可以参考 TCP Fundamentals and General Operation 相关章节；[Nmap Network Scanning](http://nmap.org/book/tcpip-ref.html) 实际是 NMAP 的介绍，不过有很多相关协议的介绍。

* [高性能网络编程](http://blog.csdn.net/column/details/high-perf-network.html) 包括了对网络协议的详细介绍。

<!--
http://blog.csdn.net/hc260164797/article/details/7563228
http://blog.csdn.net/hc260164797/article/details/7558624
http://blog.chinaunix.net/uid-29867135-id-4910871.html
http://blog.csdn.net/yusiguyuan/article/details/28625129
-->



{% highlight c %}
{% endhighlight %}
