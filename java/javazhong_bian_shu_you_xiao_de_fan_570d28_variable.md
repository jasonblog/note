# Java中變數有效的範圍(Variable Scope)

所謂的有效範圍，就是指變數在程式中可以被使用的範圍。

Java支援四種不同存取範圍的變數：


<ul>
<li><span style="color:#0000ff;"><strong>區塊變數(block):</strong></span> 只能在<span style="color:#ff0000;">區塊內</span>存取。</li>
<li><strong><span style="color:#0000ff;">區域變數(local):</span></strong> 在<span style="color:#ff0000;">方法中</span>宣告，只能在<span style="color:#ff0000;">方法內存取</span>。</li>
<li><span style="color:#0000ff;"><strong>實例變數(instance):</strong></span> 在<span style="color:#ff0000;">類別中宣告</span>，可以被類別內<span style="color:#ff0000;">除了宣告為static以外的任何方法</span>存取。<br>
簡單說就是<span style="color:#ff0000;">跟物件有關</span>，跟類別無關。亦稱為非靜態(non-static)變數。</li>
<li><strong><span style="color:#0000ff;">類別變數(class):</span></strong> 在<span style="color:#ff0000;">類別中宣告</span>，可以被類別內<span style="color:#ff0000;">任何方法存取</span>。<br>
簡單說就是<span style="color:#ff0000;">跟類別有關</span>，跟物件無關。亦稱為靜態(static)變數。</li>
</ul>


```java
class A{
    int a; //instance
    static int b; // class
    void method(){
        int x; //local
        for(int i=0; i < 10; i++)  //i為y block
        }
    }
}
```