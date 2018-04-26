# 50 行 Python 代码构建一个区块链


尽管有人认为区块链 (blockchain) 是一个答案先于问题的技术，但毫无疑问, 这项新的技术已然是一个计算的奇迹。不过，到底区块链是什么呢

##区块链

```
一个随时间增长，记录比特币或其他加密货币的交易的公开数字账本。
```

简单来说，区块链是一个公开的数据库，新的数据会被存储到一个叫做区块 (block) 的数据结构中，而区块会被添加到一个不可更改的链 (chain) 上 (也就是区块链)，该链上存储着过去所添加的所有数据。在比特币和其他一些加密货币中，这些数据就是交易。不过，实际上，这些数据可以是任何类型。

区块链技术催生了像比特币，莱特币这样全新的完全数字资产，它们不由任何一个中心权威机构发行或管理。这给那些不信任现有银行系统的个人带来了新的选项。区块链也重新定义了分布式计算，出现了像以太坊这样的技术，它引入了像智能合约这样新的概念。

在本文，我将会通过不到 50 行的 Python 代码构建一个简单的区块链原型（原文代码为 Python 2，分为多个部分托管于 gist。译者已将其改为 Python 3，并将源码放到了 GitHub 上，点击 这里 查看。)，就叫 SnakeCoin 吧。


```py
#!/usr/bin/env python
# -*- coding: utf-8 -*-

import hashlib as hasher
import datetime as date


class Block:
    def __init__(self, index, timestamp, data, previous_hash):
        self.index = index
        self.timestamp = timestamp
        self.data = data
        self.previous_hash = previous_hash
        self.hash = self.hash_block()

    def hash_block(self):
        sha = hasher.sha256()
        sha.update(
            bytes(
                str(self.index) + str(self.timestamp) + str(self.data) + str(
                    self.previous_hash), 'utf-8'))
        return sha.hexdigest()


def create_genesis_block():
    #  Manually construct a block with index 0 and arbitrary previous hash
    return Block(0, date.datetime.now(), "Genesis Block", "0")


def next_block(last_block):
    this_index = last_block.index + 1
    this_timestamp = date.datetime.now()
    this_data = "Hey! I'm block " + str(this_index)
    this_hash = last_block.hash
    return Block(this_index, this_timestamp, this_data, this_hash)


def main():
    #  Create the blockchain and add the genesis block
    blockchain = [create_genesis_block()]
    previous_block = blockchain[0]

    #  How many blocks should we add to the chain after the genesis block
    num_of_blocks_to_add = 20

    for i in range(0, num_of_blocks_to_add):
        block_to_add = next_block(previous_block)
        blockchain.append(block_to_add)
        previous_block = block_to_add
        #  Tell everyone about it!
        print("Block #{} has been added to the"
              "blockchain!".format(block_to_add.index))
        print("Hash: {}\n".format(block_to_add.hash))


if __name__ == "__main__":
    main()
```


首先，来定义我们的区块大概是什么样。在区块链中，每个区块都需要一个时间戳 (timestamp) 和一个可选的索引 (index)。在 SnakeCoin 中，我们会同时存储这两项。为了确保区块链的完整性，每个区块都需要有一个能够识别自身身份的哈希 (hash)。在比特币中，每个区块的哈希对区块索引、时间戳、数据和前一区块哈希所有内容的一个加密哈希。此外，数据可以是任何你想要存储的任何内容。

```py
import hashlib as hasher

class Block:
    def __init__(self, index, timestamp, data, previous_hash):
        self.index = index
        self.timestamp = timestamp
        self.data = data
        self.previous_hash = previous_hash
        self.hash = self.hash_block()

    def hash_block(self):
        sha = hasher.sha256()
        sha.update(
            bytes(
                str(self.index) + str(self.timestamp) + str(self.data) + str(
                    self.previous_hash), 'utf-8'))
        return sha.hexdigest()
```

很好，已经有了区块结构，但是我们构建的是一个区块链。所以，我们需要将区块添加到真正的链上。正如前文所说，每个区块都需要前一个区块的信息。如此一来，就出现了一个问题：区块链中的第一个区块是如何而来？ 第一个区块，或者一般叫做创始块(genesis block), 这是一个十分特殊的块。在很多情况下，它是通过手动或是一些特殊的逻辑添加到区块链中。

为简便起见，我们创建一个简单返回创世块的函数。创始块的索引为 0，有一个任意的数据值，一个属于 “前一个哈希” 参数的任意值。


```py
import datetime as date
def create_genesis_block():
    #  Manually construct a block with index 0 and arbitrary previous hash
    return Block(0, date.datetime.now(), "Genesis Block", "0")
```

现在我们已经创建一个创世块，接下来我们需要一个能够在区块链中生成后续区块的函数。这个函数接受区块链中的前一个区块作为参数，创建所要生成区块的数据，然后返回带有数据的新区块。当新区块对前面的区块信息进行哈希时，区块链的完整性将会得到进一步增强。如果我们不对以前的区块信息进行哈希，那么第三者就能够轻易地“篡改历史”，用一个他们自己的链替换我们的链。区块链的哈希就像是一个加密证明，它能够保证一旦一个区块被加入到区块链中，那么这个区块就永远无法被替换或者移除。


```py
def next_block(last_block):
    this_index = last_block.index + 1
    this_timestamp = date.datetime.now()
    this_data = "Hey! I'm block " + str(this_index)
    this_hash = last_block.hash
    return Block(this_index, this_timestamp, this_data, this_hash)
```

这就是整个部分最困难的地方了。现在，可以来创建我们的区块链了！在我们的案例中，区块链其实仅仅是一个 Python 的列表。列表的第一个元素是创世块。当然了，我们需要增加后续区块。因为这只是一个极简的区块链模型，我们仅添加 20 个新的区块。可以通过一个循环来添加。


```py
#  Create the blockchain and add the genesis block
blockchain = [create_genesis_block()]
previous_block = blockchain[0]

#  How many blocks should we add to the chain after the genesis block
num_of_blocks_to_add = 20

for i in range(0, num_of_blocks_to_add):
    block_to_add = next_block(previous_block)
    blockchain.append(block_to_add)
    previous_block = block_to_add
    #  Tell everyone about it!
    print("Block #{} has been added to the"
          "blockchain!".format(block_to_add.index))
    print("Hash: {}\n".format(block_to_add.hash))
```


来看一下效果：

![](images/127313-e5f1c1d75bcd1c70.png)

可以看出我们的链已经如期工作了。如果想要在控制台中看到更多信息，可以编辑源文件打印每个块的时间戳和数据。

这是 SnakeCoin 能够完成的事情了。如果想要将 SnakeCoin 达到今天可真实使用的区块链标准，我们还必须要加入更多特性，比如跟踪在多台机器上链的变化的服务层，限制在给定时间内能够加入的区块数量的工作量证明算法。

如果想要了解更多技术细节，可以查看原版的比特币白皮书。