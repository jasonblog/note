# Design Pattern 與 Double Dispatching


為了寫 Design Pattern 作業，所以上網查了一下。不過我覺得以這個作業而言，Double Dispatch 好像也沒有什麼用，如果不偷用 RTTI（不管是用 instanceof、getClass 或自己加上一個回傳型別的 Virtual Function），好像就沒有辦法確認 object 的真正的 class。

--

在談 Double Dispatch 之前我們必需先從 Polymorphism 說起。Polymorphism 有兩種：
Subtype Polymorphism: 前者就是指繼承類別之間的多型。例如：人和狗同時繼承動物類別，二者都會進食，不過二者的吃飯方式不同。我們會在動物宣告一個 Virtual Method，人和狗類別會各自 Override 自己的吃飯方式，利用 Virtual Method Invocation，在執行期喚起正確的函式。 (Method Overriding)
Type Polymorphism (Ad-hoc Polymorphism): 有一個同名的函式，編譯器會依據傳入參數的不同，選擇不同的函式，例如：同樣是 + operator，你把數字加數字與字串加字串就會有不同的結果。(Function Overloading)
值得一提的是 Virtual Method Invocation 的效率略低於一般的函式呼叫，所以 C++ 要求希望被 Virtual Invoke 的函式應該要額外加上 virtual 關鍵字。而 Function Overloading 因為是在編譯期決定呼叫的函式，所以不會影響效率。

我們先看一下什麼是 Type Polymorphism (Function Overloading)：

```cpp
class A;
class B;

class A {
public:
    void interact(A const &a) { cout << "class A vs A" << endl; }
    void interact(B const &b) { cout << "class A vs B" << endl; }
};

class B : public A {
public:
    void interact(A const &a) { cout << "class B vs A" << endl; }
    void interact(B const &b) { cout << "class B vs B" << endl; }
};

int main() {
    A a;
    B b;

    a.interact(a); // Invoke A::interact(A const&)
    a.interact(b); // Invoke A::interact(B const&)
    b.interact(a); // Invoke B::interact(A const&)
    b.interact(b); // Invoke B::interact(B const&)
    return 0;
}
```

我們可以注意到，透過 Function Overloading，我們可以在不同的時候喚起不同的函式。例如：a.interact(b) 就會喚起 void A::interact(B const &) 這個函式。

如果因為某些原因我必需使用類別 A 的 Reference 來指稱其子類別的 Instance，我把上面的程式改成這樣呢？

```cpp
int main() {
    A &aa = a;
    A &ba = b;

    a.interact(aa); // Invoke A::interact(A const&)
    a.interact(ba); // Invoke A::interact(A const&)
}
```

我們就會發現 Type Polymorphism 完全幫不了我們！為什麼？因為類別 A 的 Reference 所指向的物件的 Type 是可以隨 Runtime 的不同而有所變動，所以一般來說編譯器只能假定這個 Reference 只會是類別 A 的實例。從而只能喚起 A::interact(A const&) 函式。那我們該怎麼做呢？

此時我們會想聯想的 Subtype Polymorphism (Method Overriding) 的 Late Binding。首先我們要為上述的函式加上 virtual 關鍵字，使其可以動態地決定要喚起哪個函式。我分別 class A, class B 的 interact 函式的前面都加上了 virtual 關鍵字。（雖然一日 Virtual Method 終身 Virtual Method，所以加在 class A 裡面的 Method 就可以了，不過為了方便閱讀，我是建議在 class B 裡面的 Method 也加上 virtual）

```cpp
class A {
public:
    virtual void interact(A const &a) { cout << "class A vs A" << endl; }
    virtual void interact(B const &b) { cout << "class A vs B" << endl; }
};

class B : public A {
public:
    virtual void interact(A const &a) { cout << "class B vs A" << endl; }
    virtual void interact(B const &b) { cout << "class B vs B" << endl; }
};
```


這下問題解決了嗎？我們看看：

```cpp
int main() {
    aa.interact(aa); // Invoke A::interact(A const&)
    aa.interact(ba); // Invoke A::interact(A const&)
}
```
很顯然地，雖然 interact 是 Virtual Function，但是因為只有 this 會透過 Late Binding 而被延遲到執行期，編譯器還是會在編譯期施行 Method Overloading Resolution 從眾多 Overloaded Method 當中挑選特定版本生成程式碼，讓我們的程式於執行期呼叫 Virtual Method（進而依據 this 所屬類別進行 Dynamic Dispatch），所以這裡 Subtype Polymorphism 似乎幫不上忙。

不過，事實上 Subtype Polymorphism 是可以幫上忙的！只是我們需要一個小技巧！這就是所謂的 Double Dispatching，我們可以再寫一個函式叫作 apply，它固定傳入一個類別 A 的 Reference，不過我們會在函式的本體上動手腳！


```cpp
class A {
public:
    virtual void apply(A &obj) { obj.interact(*this); }
};

class B : public A {
public:
    virtual void apply(A &obj) { obj.interact(*this); }
};

int main() {
    aa.apply(aa); // Finally invoke A::interact(A const&)
    ba.apply(aa); // Finally invoke A::interact(B const&)

    aa.apply(ba); // Finally invoke B::interact(A const&)
    ba.apply(ba); // Finally invoke B::interact(B const&)
}
```

對，這裡我用了 *this，*this 的型別是什麼呢？對！就是擁有 apply 函式的類別。注意：我們必需要在每個類別加上相同的 apply 函式。因為每個 apply 函式中的 *this 型別是不一樣的。當我們用 object1.apply(object2) 呼叫時，apply 會幫我們找出 object1 的確切類別，再使用 object2.interact((THISCLASS &)object1) 來呼叫 interact 函式，因為 object1 的型別已經確定，所以第二次呼叫 interact 就是找出 object2 的確切類別，喚起正確的函式。

這大概是 Double Dispatch 的內容。