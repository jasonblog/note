---
title: 数据结构 -- Tree
layout: post
comments: true
language: chinese
category: [program]
keywords: 数据结构,树,tree
description: 抽象地说，基本上有序列的地方就可以应用树，因为树结构即是一种序列索引结构，这里简单介绍常见的树结构。
---

抽象地说，基本上有序列的地方就可以应用树，因为树结构即是一种序列索引结构，这里简单介绍常见的树结构。

<!-- more -->

## 简介

二叉树 (Binary tree) 每个节点最多只有两个分支的树结构，其分支通常被称为 "左子树" 和 "右子树" 。二叉搜索树 (Binary Search Tree) 是二叉树的一种，对于所有节点，其左子树的值小于根结点的值，右子树的值大于根结点的值。

查找、插入和删除的效率与数的高度 ```log n``` 相关，最坏可能达到 ```O(n)``` ，如下是其最坏情况，几乎达到了线性查找。

![tree]({{ site.url }}/images/structure/tree_unbalanced_bst.jpg "tree"){: .pull-center }

为了保证树的高度，也就出现了如下的平衡二叉树，例如 AVL Tree、Red-Black Tree。

## AVL Tree

自平衡二叉查找树 (AVL Tree) 中任意节点的两个子树的高度差最大为 1，查找、插入和删除在平均和最坏情况下都是 ```O(log n)```，插入和删除可能需要通过一次或多次树旋转来重新平衡这个树。这种二叉树得名于其发明者 G.M. Adelson-Velsky 和 E.M. Landis 在 1962 年的论文《An algorithm for the organization of information》。

![avl tree example]({{ site.url }}/images/structure/tree_avl_example.png "avl tree example"){: .pull-center width="90%" }

一般每个节点会保存一个平衡因子 (Balance Factor) 或者通过子树高度计算，一般是左子树的高度减去它的右子树的高度，平衡因子为 1、0 或 -1 的节点被认为是平衡的，而 -2、2 的节点被认为是不平衡的，并需要重新平衡这个树。

为了保持平衡，AVL Tree 可能需要执行四种的旋转方式：

![avl tree example]({{ site.url }}/images/structure/tree_avl_rotations.png "avl tree example"){: .pull-center width="90%" }

<!--
http://btechsmartclass.com/DS/U5_T2.html
https://www.tutorialspoint.com/data_structures_algorithms/avl_tree_algorithm.htm
-->

## Red-Black Tree

Red-Black Tree 和 AVL Tree 是常用的平衡二叉搜索树，可以保证插入、查找、删除操作的最坏情况都是 ```O(log n)``` 。

<!--
Both red-black trees and AVL trees are the most commonly used balanced binary search trees and they support insertion, deletion and look-up in guaranteed O(logN) time. However, there are following points of comparison between the two:

    AVL trees are more rigidly balanced and hence provide faster look-ups. Thus for a look-up intensive task use an AVL tree.
    For an insert intensive tasks, use a Red-Black tree.
    AVL trees store the balance factor at each node. This takes O(N) extra space. However, if we know that the keys that will be inserted in the tree will always be greater than zero, we can use the sign bit of the keys to store the colour information of a red-black tree. Thus, in such cases red-black tree takes O(1) extra space.
In general, the rotations for an AVL tree are harder to implement and debug than that for a Red-Black tree.

http://web.mit.edu/~emin/Desktop/ref_to_emin/www.old/source_code/red_black_tree/index.html

https://github.com/mirek/rb_tree
-->








{% highlight text %}
{% endhighlight %}
