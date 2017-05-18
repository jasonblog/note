## Invert Binary Tree

Invert a binary tree.
```
     4
   /   \
  2     7
 / \   / \
1   3 6   9
```
to
```
     4
   /   \
  7     2
 / \   / \
9   6 3   1
```

Trivia:
This problem was inspired by this original tweet by Max Howell:

>> Google: 90% of our engineers use the software you wrote (Homebrew), but you can’t invert a binary tree on a whiteboard so fuck off.

## Solution

递归法，先逆转左孩子、右孩子，再交换左右孩子即可!
