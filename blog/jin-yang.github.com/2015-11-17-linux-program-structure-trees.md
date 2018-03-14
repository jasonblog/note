---
title: 數據結構 -- Tree
layout: post
comments: true
language: chinese
category: [program]
keywords: 數據結構,樹,tree
description: 抽象地說，基本上有序列的地方就可以應用樹，因為樹結構即是一種序列索引結構，這裡簡單介紹常見的樹結構。
---

抽象地說，基本上有序列的地方就可以應用樹，因為樹結構即是一種序列索引結構，這裡簡單介紹常見的樹結構。

<!-- more -->

## 簡介

二叉樹 (Binary tree) 每個節點最多隻有兩個分支的樹結構，其分支通常被稱為 "左子樹" 和 "右子樹" 。二叉搜索樹 (Binary Search Tree) 是二叉樹的一種，對於所有節點，其左子樹的值小於根結點的值，右子樹的值大於根結點的值。

查找、插入和刪除的效率與數的高度 ```log n``` 相關，最壞可能達到 ```O(n)``` ，如下是其最壞情況，幾乎達到了線性查找。

![tree]({{ site.url }}/images/structure/tree_unbalanced_bst.jpg "tree"){: .pull-center }

為了保證樹的高度，也就出現瞭如下的平衡二叉樹，例如 AVL Tree、Red-Black Tree。

## AVL Tree

自平衡二叉查找樹 (AVL Tree) 中任意節點的兩個子樹的高度差最大為 1，查找、插入和刪除在平均和最壞情況下都是 ```O(log n)```，插入和刪除可能需要通過一次或多次樹旋轉來重新平衡這個樹。這種二叉樹得名於其發明者 G.M. Adelson-Velsky 和 E.M. Landis 在 1962 年的論文《An algorithm for the organization of information》。

![avl tree example]({{ site.url }}/images/structure/tree_avl_example.png "avl tree example"){: .pull-center width="90%" }

一般每個節點會保存一個平衡因子 (Balance Factor) 或者通過子樹高度計算，一般是左子樹的高度減去它的右子樹的高度，平衡因子為 1、0 或 -1 的節點被認為是平衡的，而 -2、2 的節點被認為是不平衡的，並需要重新平衡這個樹。

為了保持平衡，AVL Tree 可能需要執行四種的旋轉方式：

![avl tree example]({{ site.url }}/images/structure/tree_avl_rotations.png "avl tree example"){: .pull-center width="90%" }

<!--
http://btechsmartclass.com/DS/U5_T2.html
https://www.tutorialspoint.com/data_structures_algorithms/avl_tree_algorithm.htm
-->

## Red-Black Tree

Red-Black Tree 和 AVL Tree 是常用的平衡二叉搜索樹，可以保證插入、查找、刪除操作的最壞情況都是 ```O(log n)``` 。

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
