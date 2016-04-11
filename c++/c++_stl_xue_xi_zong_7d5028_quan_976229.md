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

