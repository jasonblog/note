# 軟體元件的使用


Google時代的程式撰寫 我是透過 jserv 的網站知道這篇文章。這個問題其實一直在我腦海揮之不去，軟體元件的重複利用的可行性太低。這問題要從元件的 coding style 說起，使用別人的程式碼，最讓人頭痛的，其實是 style 的不相容，軟體系統有單一 style 才能發展出自己的特色，讓閱讀者模式可尋。然而，每個 programmer 各有自己的 style，這使的許多人寧可重寫，也不願使用現成的元件。
##介面的設計

有人認為，只是拿來用而已，管他寫成什麼樣，以獨立目錄和其它 source 分開就好了。問題是，coding style 也包函介面的定義。每個 programmer 都有自己習慣的參數順序，變數取名方式。直接使用別人的 code，即使只是呼叫，也無異是讓別人的 style 混雜在自己的 style 裡。這是有潔僻 programmer 所無法忍受的，也是 code reader 最大的困難。我們大腦並不適合快速的在不同的 coding style 間轉換，每次 coding style 的切換，對大腦都是一種負擔，造成閱的中斷。我們喜歡流暢的 source code，正如我們喜歡流暢的文章。試想，一篇文章如果拆成數段，每段都由不同的人寫，你是否還能行雲流水？

##重新包裝

一般的作法是重新包裝元件的介面，也就是寫一層 wrapper，呼叫要使用的元件，而 programmer 的 source 則透過 wrapper 使用該元件。這似乎是可行的。但，這隱含著重大的困難；寫個 wrapper 有時要花的功夫，不下於重新寫過。我們雖然都覺的 wrapper 是很漂亮的解答，然而卻沒人想寫 wrapper。

##包裝過度

使用別人的元件，另一個問題是「包裝過度」。自從 OO 氾濫以來，已經演變到什麼東西都要包裝成 object/class。OO 原本是要解決 couple 的問題，但往往因 programmer 的不求甚解，而造成更多的 couple。最明顯的說是一堆 framework，往往要將資料以不同的 class 封裝，進而強迫使用者也要使用該 class，讓大家都和這些 class 產生 couple。如果讓多個不同來源的元件相互合作，那我們必定要寫一堆的 glue code，在這些 class 之間轉換。基於偷懶的心理，這些 code 一定散佈在四處八方。於是，一堆資料間轉來轉去，我們如何期待別人記得每一個 class 的功用!

##簡單就是美

簡單的藝術 提到，簡單就是美。介面的設計也是這樣，能使用 array 的，不要自以為高超的使用 linked list。能使用 x, y 的，不要使用 vector 或 point。能使用 pointer 的，也不要俏的使用 STL 的 iterator。這些都會造成使用上的困難，很少人只使用單一來源的元件。這些俏招，只會 programmer 的工作量；不斷的轉換。

##功能簡單

每個元件的功能一定得簡單，單一性。越是多功能，這樣的元件就越難使用。功能越多，表示選項越多。選項越多，表示 programmer 要花更多時間去了解，如何使用正確的參數，或選擇正確的 class 等等的。功能越雜，也代表更多相關性。這使的 programmer 必需注意到更多的相關功能，才能達到簡單的目的。例如，你只是要使用 encode 和 decode 以 MIME encoding 的資料，卻可能被迫使用一整套 parse MIME message 的元件，並傳遞許多參數，以選擇正確的編碼。

##範例程式

範例程式是了解元件用法的最快方式，許多元件提供了一堆文件，但 programmer 卻往往有看沒有懂。往往簡單幾行範例，效果比一長篇解釋好多了。雖然我在 windows 上 coding 的機會屈指可數，但我發覺許多人在看 MSDN 的說明時，都只看範例程式。範例程式提供一個元件的外觀和使用方法等資訊，programmer 可以很快的了解元件的外觀，而不必直接接觸到細節。

##OO 的使用

前面提到，有些人不管什麼都要包成 class。這樣看起來很精巧，但使用卻不方便。object 的目的，是將 state (data) 和 behavior 包裝成單元，收煉 source 的範圍，以方便管理和了解。但有些東西真的只是單純的 data，沒任何 behavior。而有些東西只是單純的 behavior，而沒有 state。面對這樣的情況，沒有必要硬是將 quick sort 或 serial number 包裝成一個 class。單純的 function 或是 structure，更能減少 programmer 的負擔，也更易融入各種 style。

##結論

KISS (Keep It Simple and Stupid)