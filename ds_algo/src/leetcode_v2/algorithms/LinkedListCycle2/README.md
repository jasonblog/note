## Linked List Cycle II

Given a linked list, return the node where the cycle begins. If there is no cycle, return null.

Follow up:
Can you solve it without using extra space? 

## Solution

設置兩個指針fast、low,fast每次前進2步，slow每次前進1步

如果存在環，則fast必先進入環，而slow後進入環，兩個指針必定相遇.否則若fast到達`NULL`，則不存在環。

假設存在環，則兩個指針相遇時，slow還沒有遍歷完，而fast已經在環內循環了n圈`(n >= 1)`.

設slow走了s步，則fast走了2s步，fast還等於s加上在環上多轉的圈數，設環長的r，則:

![](cycle.png)

```
s = LenA + x;
2s = nr + LenA + x;
=>
2LenA + 2x = nr + LenA + x
=>
LenA = nr - x

因此：第一次碰撞點Pos到join的距離等於頭指針到連接點join的距離

設此時p1 = head, p2 = pos = x; 每次走一步，則當p1到達join點時,
p2 = x + LenA = x + nr -x = nr
即p2也到達了join點，因此p1 與 p2相遇就是join點
```


## 求環的長度

第一次相遇時開始計數，fast和slow繼續走，當第二次相遇時，fast比slow多走了一圈，即r的長度

## 求鏈表長度

求到head到環入口的長度+環的長度

