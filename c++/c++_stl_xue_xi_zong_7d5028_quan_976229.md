# C++ STL 學習總結(全面)


STL就是Standard Template
Library，標準模板庫。這可能是一個歷史上最令人興奮的工具的最無聊的術語。從根本上說，STL是一些「容器」的集合，這些「容器」有list, vector,set,map等，STL也是算法和其它一些組件的集合。這裡的「容器」和算法的集合指的是世界上很多聰明人很多年的傑作。是C＋＋標準庫的一個重要組成部分，它由Stepanov and Lee等人最先開發，它是與C＋＋幾乎同時開始開發的；一開始STL選擇了Ada作為實現語言，但Ada有點不爭氣，最後他們選擇了C＋＋，C＋＋中已經有了模板。STL又被添加進了C＋＋庫。1996年，惠普公司又免費公開了STL，為STL的推廣做了很大的貢獻。STL提供了類型安全、高效而易用特性的STL無疑是最值得C++程序員驕傲的部分。每一個C＋＋程序員都應該好好學習STL。大體上包括container（容器）、algorithm（算法）和iterator（迭代器），容器和算法通過迭代器可以進行無縫連接。

## 一、基礎知識

### 1、泛型技術


泛型技術的實現方法有多種，比如模板，多態等。模板是編譯時決定，多態是運行時決定，其他的比如RTTI也是運行時確定。多態是依靠虛表在運行時查表實現的。比如一個類擁有虛方法，那麼這個類的實例的內存起始地址就是虛表地址，可以把內存起始地址強制轉換成int*，取得虛表，然後(int*)*(int*)取得虛表裡的第一個函數的內存地址，然後強制轉換成函數類型，即可調用來驗證虛表機制。

泛型編程（generic programming，以下直接以GP稱呼）是一種全新的程序設計思想，和OO，OB，PO這些為人所熟知的程序設計想法不同的是GP抽象度更高，基於GP設計的組件之間偶合度底，沒有繼承關係，所以其組件間的互交性和擴展性都非常高。我們都知道，任何算法都是作用在一種特定的數據結構上的，最簡單的例子就是快速排序算法最根本的實現條件就是所排序的對象是存貯在數組裡面，因為快速排序就是因為要用到數組的隨機存儲特性，即可以在單位時間內交換遠距離的對象，而不只是相臨的兩個對象，而如果用聯表去存儲對象，由於在聯表中取得對象的時間是線性的即O[n]，這樣將使快速排序失去其快速的特點。也就是說，我們在設計一種算法的時候，我們總是先要考慮其應用的數據結構，比如數組查找，聯表查找，樹查找，圖查找其核心都是查找，但因為作用的數據結構不同將有多種不同的表現形式。數據結構和算法之間這樣密切的關係一直是我們以前的認識。泛型設計的根本思想就是想把算法和其作用的數據結構分離，也就是說，我們設計算法的時候並不去考慮我們設計的算法將作用於何種數據結構之上。泛型設計的理想狀態是一個查找算法將可以作用於數組，聯表，樹，圖等各種數據結構之上，變成一個通用的，泛型的算法。

### 2、四種類型轉換操作符

`static_cast` 將一個值以符合邏輯的方式轉換。應用到類的指針上，意思是說它允許子類類型的指針轉換為父類類型的指針（這是一個有效的隱式轉換），同時，也能夠執行相反動作：轉換父類為它的子類。
例如：

```cpp
float x;
Count<(x);//把x作為整型值輸出
```

`dynamic_cast`            將多態類型向下轉換為其實際靜態類型。只用於對象的指針和引用。當用於多態類型時，它允許任意的隱式類型轉換以及相反過程。dynamic_cast會檢查操作是否有效。也就是說，它會檢查轉換是否會返回一個被請求的有效的完整對象。檢測在運行時進行。如果被轉換的指針不是一個被請求的有效完整的對象指針，返回值為NULL.     
例如：

```cpp
class Car;
class Cabriolet: public Car
{
    …
};
class Limousline: public Car
{
    …
};
void f(Car* cp)
{
    Cabriolet* p = dynamic_cast< Cabriolet > cp;
}
```
 
`reinterpret_cast`   轉換一個指針為其它類型的指針。它也允許從一個指針轉換為整數類型。反之亦然。這個操作符能夠在非相關的類型之間轉換。操作結果只是簡單的從一個指針到別的指針的值的二進制拷貝。在類型之間指向的內容不做任何類型的檢查和轉換。
例如：

```cpp
class A {}; 
class B {}; 
A *a = new A; 
B *b = reinterpret_cast(a);
```

`const_cast`一般用於強制消除對象的常量性。
例如：

```cpp
class C {}; 
const C *a = new C; 
C *b = const_cast(a); 
```

其它三種操作符是不能修改一個對象的常量性的。
 
###3、explicit修飾的構造函數不能擔任轉換函數。

在很多情況下，隱式轉換是有意的，並且是正當的。但有時我們不希望進行這種自動的轉換。
例如：為了避免這樣的隱式轉換，應該像下面這樣顯式聲明該帶單一參數的構造函數：

```cpp
class String
{
    int size;
    char* p;
public:        // 不要隱式轉換
    explicit String(int sz);
    String(const char* s, int size n = 0);  // 隱式轉換
};

void f()
{
    String s(10);
    s = 100; // 現在編譯時出錯；需要顯式轉換：
    s = String(100); // 好；顯式轉換
    s = "st";        // 好；此時允許隱式轉換
}
```

 
###4、命名空間namespace
   解決在使用不同模塊和程序庫時，出現名稱衝突問題。
   
###5、C++標準程序庫中的通用工具。

```sh
由類和函數構成。這些工具包括：
數種通用類型
一些重要的C函數
數值極值
```

##二、STL六大組件<br>
容器（Container）<br>
算法（Algorithm）<br>
迭代器（Iterator）<br>
仿函數（Function object）<br>
適配器（Adaptor）<br>
空間配置器（allocator）<br>

###1、容器

作為STL的最主要組成部分－－容器，分為向量（vector），雙端隊列(deque)，表(list)，隊列（queue），堆棧(stack)，集合(set)，多重集合(multiset)，映射(map)，多重映射(multimap)。

<table border="1" cellspacing="0" cellpadding="0" style="color:#333333;font-family:Arial;font-size:14px;line-height:26px;background-color:#FFFFFF;">
	<tbody>
		<tr>
			<td valign="top">
				<p>
					容器
				</p>
			</td>
			<td valign="top">
				<p>
					特性
				</p>
			</td>
			<td valign="top">
				<p>
					所在頭文件
				</p>
			</td>
		</tr>
		<tr>
			<td valign="top">
				<p>
					向量vector
				</p>
			</td>
			<td valign="top">
				<p>
					可以用常數時間訪問和修改任意元素，在序列尾部進行插入和刪除時，具有常數時間複雜度，對任意項的插入和刪除就有的時間複雜度與到末尾的距離成正比，尤其對向量頭的添加和刪除的代價是驚人的高的
				</p>
			</td>
			<td valign="top">
				<p>
					<vector>
				</vector></p>
			</td>
		</tr>
		<tr>
			<td valign="top">
				<p>
					雙端隊列deque
				</p>
			</td>
			<td valign="top">
				<p>
					基本上與向量相同，唯一的不同是，其在序列頭部插入和刪除操作也具有常量時間複雜度
				</p>
			</td>
			<td valign="top">
				<p>
					<deque>
				</deque></p>
			</td>
		</tr>
		<tr>
			<td valign="top">
				<p>
					表list
				</p>
			</td>
			<td valign="top">
				<p>
					對任意元素的訪問與對兩端的距離成正比，但對某個位置上插入和刪除一個項的花費為常數時間。
				</p>
			</td>
			<td valign="top">
				<p>
					<list>
				</list></p>
			</td>
		</tr>
		<tr>
			<td valign="top">
				<p>
					隊列queue
				</p>
			</td>
			<td valign="top">
				<p>
					插入只可以在尾部進行，刪除、檢索和修改只允許從頭部進行。按照先進先出的原則。
				</p>
			</td>
			<td valign="top">
				<p>
					<queue>
				</queue></p>
			</td>
		</tr>
		<tr>
			<td valign="top">
				<p>
					堆棧stack
				</p>
			</td>
			<td valign="top">
				<p>
					堆棧是項的有限序列，並滿足序列中被刪除、檢索和修改的項只能是最近插入序列的項。即按照後進先出的原則
				</p>
			</td>
			<td valign="top">
				<p>
					<stack>
				</stack></p>
			</td>
		</tr>
		<tr>
			<td valign="top">
				<p>
					集合set
				</p>
			</td>
			<td valign="top">
				<p>
					由節點組成的紅黑樹，每個節點都包含著一個元素，節點之間以某種作用於元素對的謂詞排列，沒有兩個不同的元素能夠擁有相同的次序，具有快速查找的功能。但是它是以犧牲插入刪除操作的效率為代價的
				</p>
			</td>
			<td valign="top">
				<p>
					<set>
				</set></p>
			</td>
		</tr>
		<tr>
			<td valign="top">
				<p>
					多重集合multiset
				</p>
			</td>
			<td valign="top">
				<p>
					和集合基本相同，但可以支持重複元素具有快速查找能力
				</p>
			</td>
			<td valign="top">
				<p>
					<set>
				</set></p>
			</td>
		</tr>
		<tr>
			<td valign="top">
				<p>
					映射map
				</p>
			</td>
			<td valign="top">
				<p>
					由{鍵，值}對組成的集合，以某種作用於鍵對上的謂詞排列。具有快速查找能力
				</p>
			</td>
			<td valign="top">
				<p>
					<map>
				</map></p>
			</td>
		</tr>
		<tr>
			<td valign="top">
				<p>
					多重集合multimap
				</p>
			</td>
			<td valign="top">
				<p>
					比起映射，一個鍵可以對應多了值。具有快速查找能力
				</p>
			</td>
			<td valign="top">
				<p>
					<map>
				</map></p>
			</td>
		</tr>
	</tbody>
</table>

STL容器能力表：


![](./images/140413.JPG)


###2、算法
算法部分主要由頭文件，和組成。< algorithm>是所有STL頭文件中最大的一個，它是由一大堆模版函數組成的，可以認為每個函數在很大程度上都是獨立的，其中常用到的功能範 圍涉及到比較、交換、查找、遍歷操作、複製、修改、移除、反轉、排序、合併等等。體積很小，只包括幾個在序列上面進行簡單數學運算的模板函數，包括加法和乘法在序列上的一些操作。中則定義了一些模板類，用以聲明函數對象。

STL的算法也是非常優秀的，它們大部分都是類屬的，基本上都用到了C＋＋的模板來實現，這樣，很多相似的函數就不用自己寫了，只要用函數模板就可以了。
我們使用算法的時候，要針對不同的容器，比如：對集合的查找，最好不要用通用函數find（），它對集合使用的時候，性能非常的差，最好用集合自帶的find（）函數，它針對了集合進行了優化，性能非常的高。
 
###3、迭代器
它的具體實現在中，我們完全可以不管迭代器類是怎麼實現的，大多數的時候，把它理解為指針是沒有問題的（指針是迭代器的一個特例，它也屬於迭代器），但是，決不能完全這麼做。


<table border="1" cellspacing="0" cellpadding="0" style="color:#333333;font-family:Arial;font-size:14px;line-height:26px;background-color:#FFFFFF;">
	<tbody>
		<tr>
			<td valign="top" colspan="3">
				<p align="center">
					迭代器功能
				</p>
			</td>
		</tr>
		<tr>
			<td valign="top">
				<p>
					輸入迭代器
				</p>
				<p>
					Input iterator
				</p>
			</td>
			<td valign="top">
				<p>
					向前讀
				</p>
				<p>
					Reads forward
				</p>
			</td>
			<td valign="top">
				<p>
					istream
				</p>
			</td>
		</tr>
		<tr>
			<td valign="top">
				<p>
					輸出迭代器
				</p>
				<p>
					Output iterator
				</p>
			</td>
			<td valign="top">
				<p>
					向前寫
				</p>
				<p>
					Writes forward
				</p>
			</td>
			<td valign="top">
				<p>
					ostream,inserter
				</p>
			</td>
		</tr>
		<tr>
			<td valign="top">
				<p>
					前向迭代器
				</p>
				<p>
					Forward iterator
				</p>
			</td>
			<td valign="top">
				<p>
					向前讀寫
				</p>
				<p>
					Read and Writes forward
				</p>
			</td>
			<td valign="top">
				<p>
					&nbsp;
				</p>
			</td>
		</tr>
		<tr>
			<td valign="top">
				<p>
					雙向迭代器
				</p>
				<p>
					Bidirectional iterator
				</p>
			</td>
			<td valign="top">
				<p>
					向前向後讀寫
				</p>
				<p>
					Read and Writes forward and
				</p>
				<p>
					backward
				</p>
			</td>
			<td valign="top">
				<p>
					list,set,multiset,map,mul
				</p>
				<p>
					timap
				</p>
			</td>
		</tr>
		<tr>
			<td valign="top">
				<p>
					隨機迭代器
				</p>
				<p>
					Random access iterator
				</p>
			</td>
			<td valign="top">
				<p>
					隨機讀寫
				</p>
				<p>
					Read and Write with random
				</p>
				<p>
					access
				</p>
			</td>
			<td valign="top">
				<p>
					vector,deque,array,string
				</p>
			</td>
		</tr>
	</tbody>
</table>


###4、仿函數
仿函數，又或叫做函數對象，是STL六大組件之一；仿函數雖然小，但卻極大的拓展了算法的功能，幾乎所有的算法都有仿函數版本。例如，查找算法find_if就是對find算法的擴展，標準的查找是兩個元素相等就找到了，但是什麼是相等在不同情況下卻需要不同的定義，如地址相等，地址和郵編都相等，雖然這些相等的定義在變，但算法本身卻不需要改變，這都多虧了仿函數。仿函數(functor)又稱之為函數對象（function object），其實就是重載了()操作符的struct，沒有什麼特別的地方。
如以下代碼定義了一個二元判斷式functor：

```cpp
struct IntLess {
    bool operator()(int left, int right) const
    {
        return (left < right);
    };
};
```

####為什麼要使用仿函數呢？
```
1).仿函數比一般的函數靈活。
2).仿函數有類型識別，可以作為模板參數。
3).執行速度上仿函數比函數和指針要更快的。
```

####怎麼使用仿函數？
除了在STL裡，別的地方你很少會看到仿函數的身影。而在STL裡仿函數最常用的就是作為函數的參數，或者模板的參數。
在STL裡有自己預定義的仿函數，比如所有的運算符，=，-，*，、比如'<'號的仿函數是less

```cpp
template<class _Ty>
struct less   : public binary_function<_Ty, _Ty, bool> {
    // functor for operator<
    bool operator()(
        const _Ty& _Left,
        const _Ty& _Right
    ) const
    {
        // apply operator< to operands
        return (_Left < _Right);
    }
};
```
從上面的定義可以看出，less從binary_function<...>繼承來的，那麼binary_function又是什麼的？

```cpp
template<class _Arg1, class _Arg2, class _Result> struct
    binary_function { // base class for binary functions
    typedef _Arg1 first_argument_type;
    typedef _Arg2 second_argument_type;
    typedef _Result result_type;
};
```

其實binary_function只是做一些類型聲明而已，別的什麼也沒做，但是在STL裡為什麼要做這些呢？如果你要閱讀過STL的源碼，你就會發現，這樣的用法很多，其實沒有別的目的，就是為了方便，安全，可復用性等。但是既然STL裡面內定如此了，所以作為程序員你必須要遵循這個規則,否則就別想安全的使用STL。
比如我們自己定一個仿函數。

可以這樣：


```cpp
template <typename type1, typename type2> class func_equal : public binary_function<type1, type2, bool>
{
    inline bool operator()(
        type1 t1,
        type2 t2) const //這裡的const不能少
    {
        return t1 == t2;//當然這裡要overload==
    }
}
```


我們看這一行： inline bool operator()(type1 t1,type2 t2) const`//這裡的const不能少` inline是聲明為內聯函數，我想這裡應該不用多說什麼什麼了，關鍵是為什麼要聲明為const的？要想找到原因還是看源碼，加入如果我們這裡寫一行代碼，find_if(s.begin(),s.end(),bind2nd(func_equal(),temp)),在bind2nd函數裡面的參數是const類型的，`const類型的對象，只能訪問cosnt修飾的函數！`


與binary_function(二元函數)相對的是unary_function(一元函數),其用法同binary_function

struct unary_function { typedef _A argument_type; typedef _R result_type; };

注：仿函數就是重載()的class，並且重載函數要為const的，如果要自定義仿函數，並且用於STL接配器，那麼一定要從binary_function或者，unary_function繼承。


###5、適配器
適配器是用來修改其他組件接口的STL組件，是帶有一個參數的類模板（這個參數是操作的值的數據類型）。STL定義了3種形式的適配器：容器適配器，迭代器適配器，函數適配器。

容器適配器：包括棧（stack）、隊列(queue)、優先(priority_queue)。使用容器適配器，stack就可以被實現為基本容器類型（vector,dequeue,list）的適配。可以把stack看作是某種特殊的vctor,deque或者list容器，只是其操作仍然受到stack本身屬性的限制。queue和priority_queue與之類似。容器適配器的接口更為簡單，只是受限比一般容器要多。

迭代器適配器：修改為某些基本容器定義的迭代器的接口的一種STL組件。反向迭代器和插入迭代器都屬於迭代器適配器，迭代器適配器擴展了迭代器的功能。

函數適配器：通過轉換或者修改其他函數對象使其功能得到擴展。這一類適配器有否定器（相當於"非"操作）、綁定器、函數指針適配器。函數對象適配器的作用就是使函數轉化為函數對象，或是將多參數的函數對象轉化為少參數的函數對象。

例如：

在STL程序裡，有的算法需要一個一元函數作參數，就可以用一個適配器把一個二元函數和一個數值，綁在一起作為一個一元函數傳給算法。 例如： find_if(coll.begin(), coll.end(), bind2nd(greater <int>(), 42)); 這句話就是找coll中第一個大於42的元素。 greater <int>()，其實就是">"號，是一個2元函數 bind2nd的兩個參數，要求一個是2元函數，一個是數值，結果是一個1元函數。 bind2nd就是個函數適配器。



###6、空間配置器

`STL的內存配置器在我們的實際應用中幾乎不用涉及，但它卻在STL的各種容器背後默默做了大量的工作，STL內存配置器為容器分配並管理內存。統一的內存管理使得STL庫的可用性、可移植行、以及效率都有了很大的提升。`

SGI-STL的空間配置器有2種，一種僅僅對c語言的malloc和free進行了簡單的封裝，而另一個設計到小塊內存的管理等，運用了內存池技術等。在SGI-STL中默認的空間配置器是第二級的配置器。

SGI使用時std::alloc作為默認的配置器。

A）．alloc把內存配置和對象構造的操作分開，分別由alloc::allocate()和::construct()負責，同樣內存釋放和對象析夠操作也被分開分別由alloc::deallocate()和::destroy()負責。這樣可以保證高效，因為對於內存分配釋放和構造析夠可以根據具體類型(type traits)進行優化。比如一些類型可以直接使用高效的memset來初始化或者忽略一些析構函數。對於內存分配alloc也提供了2級分配器來應對不同情況的內存分配。

B）．第一級配置器直接使用malloc()和free()來分配和釋放內存。第二級視情況採用不同的策略：當需求內存超過128bytes的時候，視為足夠大，便調用第一級配置器；當需求內存小於等於128bytes的時候便採用比較複雜的memeory pool的方式管理內存。

C）．無論allocal被定義為第一級配置器還是第二級，SGI還為它包裝一個接口，使得配置的接口能夠符合標準即把配置單位從bytes轉到了元素的大小：

```cpp
template<classT, classAlloc>
class simple_alloc
{
public:
    static T* allocate(size_tn)
    {
        return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T));
    }

    static T* allocate(void)
    {
        return (T*)Alloc::allocate(sizeof(T));
    }

    static voiddeallocate(T* p, size_tn)
    {
        if (0 != n) {
            Alloc::deallocate(p, n * sizeof(T));
        }
    }

    static voiddeallocate(T* p)
    {
        Alloc::deallocate(p, sizeof(T));
    }
}
```

d）．內存的基本處理工具，它們均具有commt or rollback能力。

template<classInputIterator,classForwardIterator>
ForwardIterator
uninitialized_copy(InputIteratorfirst,InputIteratorlast,ForwardIteratorresult);


template<classForwardIterator,classT>
void uninitialized_fill(ForwardIteratorfirst,ForwardIteratorlast,constT& x);


template<classForwardIterator,classSize,class T>
ForwardIterator
uninitialized_fill_n(ForwardIteratorfirst,ForwardIteratorlast,constT& x)


##三、具體容器、算法
####1、所有容器都提供了一個默認的構造函數，一個拷貝構造函數。
例如：

```cpp
list<int> l;
vector<int> ivector(l.begin(),l.end());
int array[]={1,2,3,4};
set<int> iset(array,array+sizeof(array)/sizeof(array[0]));
```

####2、與大小相關的函數
size(),empty(),max_size()

####3、返回迭代器的函數
begin(),end(),rbegin(),rend()

####4、比較操作
==,!=,<,>,>=....

 


###Vector詳解：
```cpp
capacity(),返回vector能夠容納的元素個數。
size(),返回vector內現有元素的個數。
```
####賦值操作：

```cpp
c1=c2; 把c2的全部元素指派給c1
c.assign(n,elem);複製n個elem，指派給c
c.assign(beg,end);將區間beg，end內的元素指派給c
c1.swap(c2);將c1,c2元素互換
swap(c1,c2);同上
```

####元素存取
```cpp
c.at(index);
c[index];
c.front();返回第一個元素
c.back();
```
 
####插入和刪除：
```cpp
c.insert(pos.elem);
c.insert(pos,n.elem); 插入n個elem
c.insert(pos,beg,end); 在pos出插入beg，end區間內的所有元素。
c.push_back(elem);
c.pop_back();
c.erase(pos); 刪除pos上的元素，返回下一個元素
c.erase(beg,end);
c.resize(num);將元素數量改為num，如果size變大了，多出來的新元素都要一default方式構建。
c.resize(num,elem);將元素數量改為num，如果size變大了，多出來的新元素是elem的副本。
c.clear();刪除所有。
```

####vector的reserve和resize

reserve只分配空間，而不創建對象，size()不變。而resize分配空間而且用空對象填充.

reserve是容器預留空間，但並不真正創建元素對象，在創建對象之前，不能引用容器內的元素，因此當加入新的元素時，需要用push_back()/insert()函數。

resize是改變容器的大小，並且創建對象，因此，調用這個函數之後，就可以引用容器內的對象了，因此當加入新的元素時，用operator[]操作符，或者用迭代器來引用元素對象。

再者，兩個函數的形式是有區別的，reserve函數之後一個參數，即需要預留的容器的空間；resize函數可以有兩個參數，第一個參數是容器新的大小，第二個參數是要加入容器中的新元素，如果這個參數被省略，那麼就調用元素對象的默認構造函數。

vector有而deque無的：capacity(), reserve();

deque有而vector無的：push_front(elem), pop_front(); push_back(elem), pop_back();

STL提供的另兩種容器`queue、stack`，其實都只不過是一種adaptor，它們簡單地修飾deque的界面而成為另外的容器類型

 

###List詳解：

```cpp
for_each  (.begin(), .end(), “函數”);

count (.begin(), .end(), 100, jishuqi);

返回對象等於100的個數jishuqi值。

count_if() 帶一個函數對象的參數(上面“100”的這個參數)。函數對象是一個至少帶有一個operator()方法的類。這個類可以更復雜。

find(*.begin().*end(),“要找的東西”)；

如果沒有找到指出的對象，就會返回*.end()的值，要是找到了就返回一個指著找到的對象的iterator

fine_if()；與count_if()類似，是find的更強大版本。

STL通用算法search()用來搜索一個容器，但是是搜索一個元素串，不象find()和find_if() 只搜索單個的元素。

search算法在一個序列中找另一個序列的第一次出現的位置。

search(A.begin(), A.end(), B.begin(), B.end());

在A中找B這個序列的第一次出現。

要排序一個list，我們要用list的成員函數sort()，而不是通用算法sort()。

list容器有它自己的sort算法，這是因為通用算法僅能為那些提供隨機存取裡面元素 的容器排序。

list的成員函數push_front()和push_back()分別把元素加入到list的前面和後面。你可以使用insert() 把對象插入到list中的任何地方。

insert()可以加入一個對象，一個對象的若干份拷貝，或者一個範圍以內的對象。

list成員函數pop_front()刪掉list中的第一個元素，pop_back()刪掉最後一個元素。函數erase()刪掉由一個iterator指出的元素。還有另一個erase()函數可以刪掉一個範圍的元素。

list的成員函數remove()用來從list中刪除元素。

*.remove("要刪除的對象");

通用算法remove()使用和list的成員函數不同的方式工作。一般情況下不改變容器的大小。

remove(*.begin(),*.end(),"要刪除的對象");

使用STL通用算法stable_partition()和list成員函數splice()來劃分一個list。

stable_partition()是一個有趣的函數。它重新排列元素，使得滿足指定條件的元素排在不滿足條件的元素前面。它維持著兩組元素的順序關係。

splice 把另一個list中的元素結合到一個list中。它從源list中刪除元素。
```
 

###Set Map詳解：

STL map和set的使用雖不復雜，但也有一些不易理解的地方，如：

為何map和set的插入刪除效率比用其他序列容器高？

為何每次insert之後，以前保存的iterator不會失效？

為何map和set不能像vector一樣有個reserve函數來預分配數據？

當數據元素增多時（10000到20000個比較），map和set的插入和搜索速度變化如何？

C++ STL中標準關聯容器set, multiset, map, multimap內部採用的就是一種非常高效的平衡檢索二叉樹：紅黑樹，也成為RB樹(Red-Black Tree)。RB樹的統計性能要好於一般的平衡二叉樹(AVL-樹).

為何map和set的插入刪除效率比用其他序列容器高？

大部分人說，很簡單，因為對於關聯容器來說，不需要做內存拷貝和內存移動。說對了，確實如此。map和set容器內所有元素都是以節點的方式來存儲，其節點結構和鏈表差不多，指向父節點和子節點。這裡的一切操作就是指針換來換去，和內存移動沒有關係。

為何每次insert之後，以前保存的iterator不會失效？(同解)

為何map和set不能像vector一樣有個reserve函數來預分配數據？

究其原理來說時，引起它的原因在於在map和set內部存儲的已經不是元素本身了，而是包含元素的節點。

其實你就記住一點，在map和set內面的分配器已經發生了變化，reserve方法你就不要奢望了。

當數據元素增多時（10000和20000個比較），map和set的插入和搜索速度變化如何？

如果你知道log2的關係你應該就徹底瞭解這個答案。在map和set中查找是使用二分查找，也就是說，如果有16個元素，最多需要比較4次就能找到結果，有32個元素，最多比較5次。那麼有10000個呢？最多比較的次數為log10000，最多為14次，如果是20000個元素呢？最多不過15次。

 

###泛型算法：

所有算法的前兩個參數都是一對iterators：[first，last)，用來指出容器內一個範圍內的元素。

每個算法的聲明中，都表現出它所需要的最低層次的iterator類型。


常用算法：
```cpp
accumulate() 元素累加

adjacent_difference() 相鄰元素的差額

adjacent_find() 搜尋相鄰的重複元素

binary_search() 二元搜尋

copy() 複製

copy_backward() 逆向複製

count() 計數

count_if() 在特定條件下計數

equal() 判斷相等與否

equal_range() 判斷相等與否（傳回一個上下限區間範圍）

fill() 改填元素值

fill_n() 改填元素值，n 次

find() 搜尋

find_if() 在特定條件下搜尋

find_end() 搜尋某個子序列的最後一次出現地點

find_first_of() 搜尋某些元素的首次出現地點

for_each() 對範圍內的每一個元素施行某動作

generate() 以指定動作的運算結果充填特定範圍內的元素

generate_n() 以指定動作的運算結果充填 n 個元素內容

includes() 涵蓋於

inner_product() 內積

inplace_merge() 合併並取代（覆寫）

iter_swap() 元素互換

lexicographical_compare() 以字典排列方式做比較

lower_bound() 下限

max() 最大值

max_element() 最大值所在位置

min() 最小值

min_element() 最小值所在位置

merge() 合併兩個序列

mismatch() 找出不吻合點

next_permutation() 獲得下一個排列組合

泛型演算法（Generic Algorithms）與 Function Obje4 cts

nth_element() 重新安排序列中第n個元素的左右兩端

partial_sort() 局部排序

partial_sort_copy() 局部排序並複製到它處

partial_sum() 局部總和

partition() 切割

prev_permutation() 獲得前一個排列組合

random_shuffle() 隨機重排

remove() 移除某種元素（但不刪除）

remove_copy() 移除某種元素並將結果複製到另一個 container

remove_if() 有條件地移除某種元素

remove_copy_if() 有條件地移除某種元素並將結果複製到另一個 container

replace() 取代某種元素

replace_copy() 取代某種元素，並將結果複製到另一個 container

replace_if() 有條件地取代

replace_copy_if() 有條件地取代，並將結果複製到另一個 container

reverse() 顛倒元素次序

reverse_copy() 顛倒元素次序並將結果複製到另一個 container

rotate() 旋轉

rotate_copy() 旋轉，並將結果複製到另一個 container

search() 搜尋某個子序列

search_n() 搜尋「連續發生 n 次」的子序列

set_difference() 差集

set_intersection() 交集

set_symmetric_difference() 對稱差集

set_union() 聯集

sort() 排序

stable_partition() 切割並保持元素相對次序

stable_sort() 排序並保持等值元素的相對次序

swap() 置換（對調）

swap_range() 置換（指定範圍）

transform() 以兩個序列為基礎，交互作用產生第三個序列

unique() 將重複的元素摺疊縮編，使成唯一

unique_copy() 將重複的元素摺疊縮編，使成唯一，並複製到他處

upper_bound() 上限
```
 

 

###四、注意細節：

```cpp
1、auto_ptr 不能用new[]所生成的array作為初值，因為釋放內存時用的是delete，而不是delete[]

2、就搜尋速度而言，hash table通常比二叉樹還要快5~10倍。hash table不是C++標準程序庫的一員。

迭代器使用過程中優先選用前置式遞增操作符（++iter）而不是選擇後置式遞增操作符（iter++）。

3、迭代器三個輔助函數：advance(),distance(),iter_swap()。

advance()可令迭代器前進
distance()可處理迭代器之間的距離。
iter_swap()可交換兩個迭代器所指內容。

4、hasp函數 makeheap()、push_heap()、pop_heap()、sort_heap()

5、’/0’在string之中並不具有特殊意義，但是在一般C形式的string中卻用來標記字符串結束。在string中，字符 ‘/0’和其他字符的地位完全相同。string中有三個函數可以將字符串內容轉換成字符數組或C形式的string。

data()     以字符數組的形式返回字符串內容。但末未追加’/0’字符，返回類型並非有效的C形式string。

c_str()    以C形式返回字符串內容（在末尾端添加’/0’字符）。

copy()    將字符串內容複製到“調用者提供的字符數組”中，不添加’/0’字符。

6、容器中用empty來代替檢查size是否為0；當使用new得到指針的容器時，切記在容器銷燬前delete那些指針；千萬不要把auto_ptr放入容器中。

7、儘量使用vector和string來代替動態申請的數組；避免使用vector<bool>，vector<bool>有兩個問題．第一，它不是一個真正STL容器，第二，它並不保存bool類型。

8、迭代器使用過程中，儘量使用iterator代替const_iterator，reverse_iterator和const_reverse_iterator；使用distance和advance把const_iterators轉化成iterators。

typedef deque<int> IntDeque;  // 和以前一樣

typedef IntDeque::iterator Iter;

typedef IntDeque::const_iterator ConstIter;

IntDeque  d;

ConstIter ci;

...     // 讓ci指向d

Iter i(d.begin());    // 初始化i為d.begin()

advance(i, distance(i, ci));  // 調整i，指向ci位置

9、避免對set和multiset的鍵值進行修改。

10、永遠讓比較函數對相同元素返回false。

11、排序選擇：

1）如果你需要在vector、string、deque或數組上進行完全排序，你可以使用sort或stable_sort。

2）如果你有一個vector、string、deque或數組，你只需要排序前n個元素，應該用partial_sort。

3）如果你有一個vector、string、deque或數組，你需要鑑別出第n個元素或你需要鑑別出最前的n個元素，而不用知道它們的順序，nth_element是你應該注意和調用的。

4）如果你需要把標準序列容器的元素或數組分隔為滿足和不滿足某個標準，你大概就要找partition或stable_partition。

5）如果你的數據是在list中，你可以直接使用partition和stable_partition，你可以使用list的sort來代替sort和stable_sort。如果你需要partial_sort或nth_element提供的效果，你就必須間接完成這個任務。12、如果你真的想刪除東西的話就在類似remove的算法後接上erase。remove從一個容器中remove元素不會改變容器中元素的個數，erase是真正刪除東西。 13、提防在指針的容器上使用類似remove的算法，在調用類似remove的算法前手動刪除和廢棄指針。14、儘量用成員函數代替同名的算法，有些容器擁有和STL算法同名的成員函數。關聯容器提供了count、find、lower_bound、upper_bound和equal_range，而list提供了remove、remove_if、unique、sort、merge和reverse。大多數情況下，你應該用成員函數代替算法。這樣做有兩個理由。首先，成員函數更快。其次，比起算法來，它們與容器結合得更好（尤其是關聯容器）。那是因為同名的算法和成員函數通常並不是是一樣的。 15、容器中使用自定義的結構體時，如果用到拷貝與賦值，結構體需要重載operator=符號；比較容器分成相等與不等，相等時重載operator==符號，不等時重載operator<符號。比如set、map、multiset、multimap、priority_queue等容器類要求重載operator<符號。 16、Map/Multimap，Sets/Multisets都不能用push_back,push_front,因為它是自動排序的。

Set內的相同數值的元素只能出現一次，Multisets內可包含多個數值相同的元素。

Map內的相同數值的元素只能出現一次，Multimap內可包含多個數值相同的元素。內部由二叉樹實現，便於查找。

17、string 與 數字之間的轉換，轉換的方法有很多種，一般使用stringstream來實現轉換。比如：

```
```cpp
#include  <iostream>
#include  <sstream>
#include  <string>
using namespace std;

int main()
{
    int   i = 0;
    string   temp;
    stringstream   s;

    //string轉換為數字
    temp = "1234";
    s << temp;
    s >> i;
    cout << i << endl;

    //數字轉換為string
    i = 256;
    s << i;
    temp = s.str();
    cout << temp << end;

    return   0;
}
```

18、對於自定義的結構體，放入容器中，最好不要對容器進行內存初始化（不要調用memset,zeromemory函數），否則如果結構體中有指針類型的變量時，就會出現問題。

19、Vector的函數洩漏問題

定義了一個
```cpp
struct temp
{
    char name[256];
    int i;
}

Vector<temp> vect;
```

當對這個vect執行pushback一些temp的結構體後，執行clear這樣是否會內存洩露？可以釋放掉temp結構體中的name內存嗎？

解決方法：

`不行，clear只是把那些元素全部刪除掉，並不是釋放內存。再者，你這樣的定義容器是不需要釋放內存的，如果你這樣定義，`

```cpp
std::vector <temp> *pVec 就需要了。
先pVec->clear()
再
pVec->swap( (std::vector <temp>)(*pVec) )。
```

就能實現內存的釋放。

 

20、stl之map erase方法的正確使用

STL的map表裡有一個erase方法用來從一個map中刪除掉指令的一個節點，不存在任何問題。 如果刪除多一個節點時，需要使用正確的調用方法。比如下面的方法是有問題： 

```cpp
for (ITER iter = mapTest.begin(); iter != mapTest.end(); ++iter)
{
    cout << iter->first << ":" << iter->second << endl;
    mapTest.erase(iter);
}
```

這是一種`錯誤`的寫法,會導致程序行為不可知.究其原因是map 是關聯容器,對於關聯容器來說，如果某一個元素已經被刪除，那麼其對應的迭代器就失效了，不應該再被使用；否則會導致程序無定義的行為。

正確的使用方法： 1).使用刪除之前的迭代器定位下一個元素。STL建議的使用方式 

```cpp
for (ITER iter = mapTest.begin(); iter != mapTest.end();)
{
    cout << iter->first << ":" << iter->second << endl;
    mapTest.erase(iter++);
}
```

或者 

```cpp
for (ITER iter = mapTest.begin(); iter != mapTest.end();)
{
    ITER iterTmp = iter;
    iter++;
    cout << iterTmp->first << ":" << iterTmp->second << endl;
    mapTest.erase(iterTmp);
}
```

2). erase() 成員函數返回下一個元素的迭代器 

```cpp
for (ITER iter = mapTest.begin(); iter != mapTest.end();)
{
    cout << iter->first << ":" << iter->second << endl;
    iter = mapTest.erase(iter);
}
```

##推薦書籍：

《C++標準程序庫》
本書將焦點放在標準模板庫（Standard Template Library）身上，檢驗其中的容器（containers）、迭代器（iterators）、仿函數（functors）和算法（algorithms）。你還可以找到特殊容器、字符串（strings）、數值類別、國際化議題、IOStream。每一個組件都有深刻的呈現，包括其介紹、設計、運用實例、細部解說、陷阱、意想不到的危險，以及相關類別和函數的確切標記（signature）和定義。一份見解深刻的基礎概念介紹和一個程序庫綜合鳥瞰，會對新手帶來快速的提升。

《泛型編程與STL》

闡述了泛型程序設計的中心觀念:concepts,modeling, refinement,併為你展示這些觀念如何導出 STL的基礎概念:iterators, containers, function objects.循此路線,你可以把 STL想象為一個由 concepts(而非明確之functions或 classes)組成的 library.你將學習其正式結構並因此獲得其潛在威力之完整優勢.

《Effective STL》

闡述瞭如何有效地使用STL（Standard Template Library,標準模板庫）進行編程。書中講述瞭如何將STL組件組合在一起，從而利用庫的設計。這些內容會幫助你針對簡單的問題開發出簡單、直接的解決方案，並且針對複雜的問題開發出精緻的解決方案。書中還描述了常見的STL使用錯誤，並告訴你如何避免這些錯誤。

《STL源碼剖析》

瞭解源碼，看到vector的實現、list的實現、heap的實現、deque的實現、RB-tree的實現、hash-table的實現、set/map的實現；你將看到各種算法（排序、搜尋、排列組合、數據移動與複製…）的實現；你甚至將看到底層的memory pool和高階抽象的traits機制的實現。
