# 如何使用Thread和Handler



在如何使用Thread和Handler這篇文章中, 介紹Handler跟Thread的關係,
但是其實這樣做並不是很好, 如果只是為了更新一個簡單的UI,
其實可以用Main Thread來做就好。

怎麼做呢? 很簡單這個範例提供兩個方法:

runOnUiThread
Handler
這兩個都會跑在MainThread上面。

```java
runOnUiThread(new Runnable(){
    @Override
    public void run() {
        //UI更新

    }
});
new Handler(getMainLooper()).post(new Runnable(){

    @Override
    public void run() {
        //UI更新

    }
    
}); 
```

方法如上 其實非常簡單 所以也不需要範例程式

這兩個差別在哪裡?

##Handler

```java
Runnable task = getTask();
new Handler(Looper.getMainLooper()).post(task);
```

##runOnUiThread

```java
public final void runOnUiThread(Runnable action) {
    if (Thread.currentThread() != mUiThread) {
        mHandler.post(action);
    } else {
        action.run();
    }
}
```
runUiOnThread會去檢查是否在MainThread, 如果是就直接post, 否則則會透過Handler去傳送message。

