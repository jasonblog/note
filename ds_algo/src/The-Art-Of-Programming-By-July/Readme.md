## About

**July的新書《編程之法：面試和算法心得》紙質版在本github上的基礎上做了極大徹底的改進、優化，無論是完整度、還是最新度、或質量上，都遠非博客、github所能相比。換言之，新書《編程之法》的質量遠高於博客、github。**

**此外，散落在網上其他任何地方的“編程之法”電子材料均是盜版自本github，更無質量可言。所以，July只唯一推薦《編程之法》紙質版。**

《編程之法》紙質版已於2015年10月14日陸續開賣！目前，京東、噹噹、亞馬遜等各大網店均已有現貨銷售。比如京東：http://item.jd.com/11786791.html 。

另，新書《編程之法：面試和算法心得》的勘誤在此文最末：http://blog.csdn.net/v_july_v/article/details/49302193

看過[結構之法算法之道blog](http://blog.csdn.net/v_july_v)的朋友可能知道，從2010年10月起，[July](http://weibo.com/julyweibo) 開始整理一個微軟面試100題的系列，他在整理這個系列的過程當中，越來越強烈的感覺到，可以從那100題中精選一些更為典型的題，每一題詳細闡述成章，不斷優化，於此，便成了程序員編程藝術系列。

原編程藝術系列從2011年4月至今，寫了42個編程問題，在創作的過程當中，得到了很多朋友的支持，特別是博客上隨時都會有朋友不斷留言，或提出改進建議，或show出自己的思路、代碼，或指正bug。為更好的改進、優化、增補編程藝術系列，特把博客上的這個**程序員編程藝術系列和博客內其它部分經典文章**同步到此，成立本項目。

若發現任何問題、錯誤、bug，或可以優化的每一段代碼，歡迎隨時pull request或發issue反饋，thanks。

## Start Reading
 * [中文目錄](ebook/zh/Readme.md) Enhancement in progress
 * [English Contents](ebook/en/Readme.md) Translation in progress


## How To Contribute
 * 邀請大家幫忙把github上的文章導出到word上，歡迎到這裡認領：https://github.com/julycoding/The-Art-Of-Programming-By-July/issues/337 」
 * 一章一章的測試所有代碼，指正 bug，修正錯誤。 「必選，可到這裡認領：https://github.com/julycoding/The-Art-Of-Programming-By-July/issues/210 」
 * 優化原文章上的C/C++ 代碼，優化後的代碼可以放到[ebook/code](ebook/code/)文件夾內，並注意代碼命名規範的問題：https://github.com/julycoding/The-Art-Of-Programming-By-July/issues/234 。 「必選」
 * 添加其它語言如Java、python、go 的代碼，放在[ebook/code](ebook/code/)文件夾內，同樣如上，注意代碼命名規範的問題。 「可選」
 * 重繪所有的圖片：https://github.com/julycoding/The-Art-Of-Programming-by-July/issues/80
 * 翻譯成英文版，參考[中文目錄](ebook/zh/Readme.md)，把翻譯後的文章編輯到這[English Version](ebook/en/Readme.md),注：不必逐字翻譯，精簡大氣即可（如有興趣翻譯，請到這裡領取感興趣的章節翻譯：https://github.com/julycoding/The-Art-Of-Programming-by-July/issues/84 )
 * 自己主導續寫新的章節；
 * 任何你想做的事情，包括痛批你覺得寫的爛的章節，所有你的意見都將改進此係列。

你可以做以上任何一件或幾件事情，如遇到任何問題或疑惑，咱們可以隨時討論：
<https://github.com/julycoding/The-Art-Of-Programming-by-July/issues?state=open>。
「如不知如何在github上提交及同步作者的更新，可參考此文：http://www.cnblogs.com/rubylouvre/archive/2013/01/24/2874694.html 」

## Code Style
本項目暫約定以下代碼風格(不斷逐條添加中)：
 - 關於空格
- 所有代碼使用4個空格縮進
- 運算符後使用一個空格
- "," 和for循環語句中的";" 後面跟上一個空格
- 條件、分支保留字，如 if for while else switch 後留出一個空格
- "[]", "."和"->" 前後不留空格
 - 用空行把大塊代碼分成邏輯上的“段落
 - 關於括號 
- 大括號另起一行
- 即便只有一行代碼也加大括號
 - C 指針中的指針符靠近類型名，如寫成int* p，而不寫成int *p
 - 關於標點
- 中文表述，使用中文全角的標點符號，如：（）、。，？
- 數學公式（包括文中混排的公式）和英文代碼，使用英文半角的標點符號，如：(),.?…
 - 關於註釋
- 註釋統一用中文
- 儘量統一用"//"，一般不用"/\*...\*/"
 - 關於命名
- 類名為大寫字母開頭的單詞組合
- 函數名比較長，由多個單詞組成的，每個單詞的首字母大寫，如int MaxSubArray()；函數名很短，由一個單詞組成，首字母小寫，比如int swap()
- 變量名比較長，由多個單詞組成的，首個單詞的首字母小寫，後面緊跟單詞的首字母大寫，如maxEnd；變量名很短，由一個單詞組成，首字母小寫，如left
- 變量儘量使用全名，能夠描述所要實現的功能，如 highestTemprature；對於已經公認了的寫法才使用縮寫，如 tmp mid prev next
- 變量名能“望文生義”，如v1, v2不如area, height
- 常量的命名都是大寫字母的單詞，之間用下劃線隔開，比如MY_CONSTANT
- il < 4384 和 inputLength < MAX_INPUT_LENGTH，後一種寫法更好
 - 一個函數只專注做一件事
 - 時間複雜度小寫表示，如O(nlogn)，而不寫成O(N*logN)
 - 正文中絕大部分採用C實現，少量C++代碼，即以C為主，但不去刻意排斥迴避C++；
 - 關於的地得
- 形容詞（代詞） + 的 + 名詞，例如：我的小蘋果
- 副詞 + 地 + 動詞，例如：慢慢地走
- 動詞 + 得 + 副詞，例如：走得很快
 - 關於參考文獻
- 格式：主要責任者.書名〔文獻類型標識 ] .其他責任者.版本.出版地：出版者，出版年.文獻數量.叢編項.附註項.文獻標準編號。例子：1 劉少奇.論共產黨員的修養.修訂 2 版.北京：人民出版社，1962.76 頁.
 - 專業術語
- 統一一律用“樹結點”，而不是“樹節點”。
- 用左子樹、右子樹表示樹的左右子樹沒問題，但是否用左孩子、右孩子表示樹或子樹的左右結點？
 - ..
 - 此外，更多C++ 部分可參考Google C++ Style Guide，中文版見：http://zh-google-styleguide.readthedocs.org/en/latest/contents/ ；

有何問題或補充意見，咱們可以隨時到這裡討論：https://github.com/julycoding/The-Art-Of-Programming-By-July/issues/81 。

## Ver Note
 - 2010年10月11日，在CSDN上正式開博，感謝博客上所有讀者的訪問、瀏覽、關注、支持、留言、評論、批評、指正；
 - 2011年1月，在學校的時候，第一家出版社聯繫出書，因“時機未到，尚需積累”的原因婉拒，隨後第二家、第三家出版社陸續聯繫，因總感覺寫書的時機還沒到，一律婉拒；
 - 2011年10月， 當時在圖靈教育的楊海玲老師（現在人民郵電信息技術分社）再度聯繫出書，再度認為“時機未到”；
 - 2013年12月30日，本項目在眾多朋友的努力之下，衝到github流行趨勢排行榜全球第一，自己也在眾人助推下衝到全球開發者第一。
 - 2014年1月18日，想通了一件事：如果什麼都不去嘗試，那麼將年年一事無成，所以元旦一過，便正式確認今2014年之內要把拖了近3年之久的書出版出來；
 - 2013年12月-2014年3月，本github的Contributors 轉移結構之法算法之道blog的部分經典文章到本github上，感謝這近100位Contributors，包括但不限於：
- Boshen（除我之外，貢獻本github的次數最多）
- sallen450
- marchtea（專門為本github書稿弄了一個HTML網頁）
- nateriver520（勸我把書稿放在github上，才有了本github）
 - 2014年3月，通讀全部文章，修正明顯錯誤，並邀請部分朋友review本github上的全部文章，包括cherry、王威揚、鄔勇、高增琪、武博文、楊忠寶等；
 - 2014年4月
- 整個4月，精簡篇幅，調整目錄，Contributors 貢獻其它語言代碼，並翻譯部分文章；
- 4月25日，跟人民郵電出版社信息技術分社簽訂合同，書名暫定《程序員編程藝術：面試和算法心得》，有更好的名字再替換。
 - 2014年5月，逐章逐節逐行逐字優化文字描述，測試重寫優化每一段每一行每一個代碼，確定代碼基本風格；
 - 2014年6月
- 第一週，壓縮篇幅，寧願量少，但求質精；
- 第二週，全面 review；
- 第三週，本github的部分Contributors 把全部文章從github轉到word上，這部分contributors 包括包括：zhou1989、qiwsir、DogK、x140yu、ericxk、zhanglin0129、idouba.net、gaohua、kelvinkuo等；
- 第四周，繼續在Word 上做出最後徹底的改進，若未發現bug或pull request，本github將暫不再改動；
- 6月30日，與出版社約定的交稿日期延期，理由：目前版本不是所能做到的最好的版本。
 - 2014年7月，邀請部分好友進行第一輪審稿，包括曹鵬、鄒偉、林奔、王婷、何歡，其中，曹鵬重寫優化了部分代碼。此外，葛立娜對書稿中的語言描述做了不少改進；
 - 2014年8月
- 8月上旬，新增KMP一節內容；
- 8月下旬，重點修改SVM一節內容；
 - 2014年9月
- 9月上旬，和一些朋友一起重繪稿件中的部分圖和公式，這部分朋友包括顧運（@陳笙）、mastermay、在山東大學讀研二的豐俊丙、廈門大學電子工程系陳友和等等；
- 9月下旬，再度邀請另一部分好友進行第二輪審稿，包括許利傑、王亮、陳贏、李祥老師、litaoye等，並在微博上公開徵集部分讀者審稿，包括李元超、劉琪等等；
 - 2014年10月
- 10月8日起，開始一章一章陸續交Word 稿給出版社初審
- 10月9日，第一章、字符串完成修改；
- 10月10日，第二章、數組完成修改；
- 10月22日，第三章、樹完成修改；
 - 2014年11月
- 11月5日，第三章、樹完成第二版修改，主要修正部分圖片、公式、語言描述的錯誤；
 - 2014年12月
- 12月1日，第四章、查找完成修改。至此，前4 章的修改稿交付出版社。 
- 12月8日，第五章、動態規劃完成修改，等出版社反饋中。一個人堅持有點枯燥。
- 12月31日，第六章仍未修改完。
 - 2015年1月
- 1月12日凌晨，第六章、海量數據處理完成修改，交付出版社。
 - 2015年4月
- 4月27日凌晨，交完第七章初稿，接下來編輯老師反饋，我修改審閱反饋稿。且書名由原來的《程序員編程藝術：面試和算法心得》暫時改為《編程之法：面試和算法心得》。
 - 2015年5月
- 5月2日，開始寫書的前言，大致是：為何要寫這本書，寫的過程是怎樣的；這是本什麼書，有何特色，內容是什麼，為什麼這麼寫；寫給誰看，怎麼看更好。當然我還會加一些自己覺得比較個性化的內容。
- 5月5日，審閱完編輯老師的第一章反饋，併合並。
- 5月6日，審閱完第二章的一半。海玲姐兩位老師給出了大量細緻、詳盡的修改建議，包括文字表述、語言表達、標點符號、字體格式、出版規範，尤其是正斜體、大小寫、上下角。
- 5月15日，和海玲姐審完第一、二章，標點、術語、表述、邏輯、圖片、代碼等一切細節。書稿進入一審階段。
 - 2015年6月
- 6月28日，經過反覆修改、確認，書稿第一、二、三章基本定稿。 
 - 2015年7月
- 7月10日，書稿全部七章基本定稿，即將進入二審。
- 7月23日，補齊前言、封底、內容提要、邀請曹鵬、鄒偉兩位博士寫推薦序，書稿進入二審，出版社重繪全部圖片和公式。
 - 2015年8月
- 8月6日，三審結束。書稿取得階段性的勝利。 
- 8月下旬，發稿審批。
 - 2015年9月
- 9月上旬，排版校對，出膠片、印刷、裝訂成書 
- 9月21日，幾經易稿，終於敲定新書封面。
- 9月22日，開始印刷。
 - 2015年10月
- 進入10月份，萬眾期待的《編程之法》，終於終於要來了！
- 10月13日晚，終於拿到第一批樣書。
- 10月14日下午三點半，我的新書《編程之法》終於在異步社區上首發開賣！
- 10月28日，新書正式上架京東。目前京東、噹噹、亞馬遜等各大網店均已有現貨銷售。

## Contributors
感謝所有貢獻的朋友：https://github.com/julycoding/The-Art-Of-Programming-by-July/graphs/contributors ，因為有各位之力，本項目才能於13年年底衝到github流行趨勢排行榜全球第一。非常期待你的加入，thanks。

同時，歡迎加入《編程之法》討論交流QQ群：74631723，需要寫驗證信息。

孤軍奮戰的時代早已遠去，我們只有團結起來，才能幫助到更多人。[@研究者July](http://weibo.com/julyweibo)，始於二零一三年十二月十四日。

## Expressing Thanks

- 感謝我博客上所有讀者的訪問、瀏覽、關注、支持、留言、評論、批評、指正，僅以本書獻給我博客的所有讀者。
- 感謝Boshen、sallen450、marchtea、nateriver520等朋友幫我把博客上的部分經典文章移到GitHub上。
- 感謝zhou1989、qiwsir、DogK、x140yu、ericxk、zhanglin0129、idouba.net、gaohua、kelvinkuo等朋友幫我把GitHub上的文章轉為Word文件。
- 感謝顧運、mastermay、豐俊丙、陳友和等朋友幫忙重繪書中的部分圖和重錄書中的部分公式。
- 感謝cherry、王威揚、鄔勇、高增琪、武博文、楊忠寶、葛立娜、林奔、王婷、何歡、許利傑JerryLead、王亮、陳贏、李祥老師、litaoye、李元超、劉琪、weedge、Frankie等眾多朋友幫忙審校書稿。
- 特別感謝曹鵬、鄒偉兩位博士。感謝他們非常認真細緻地看完了全部書稿，給出了非常多的建設性意見，併為本書作序。
- 最後，再次感謝楊海玲老師以及出版社的編輯們。感謝楊海玲老師給出了大量細緻的修改建議，並且非常耐心地與我一輪一輪討論和修改書稿。

感謝以上諸位，正因為他們的幫助，紙書《編程之法：面試和算法心得》的質量才不斷提升，從而給廣大讀者呈現的是更好的作品。

## Copyright
本電子書的版權屬於July 本人，嚴禁他人出版或用於商業用途，違者必究法律責任。July、二零一四年五月十一日晚。

## July' PDF
- July’ PDF
 - 《支持向量機通俗導論（理解SVM的三層境界）》Latex排版精細版：http://vdisk.weibo.com/s/zrFL6OXKgnlcp ；Latex版本②：https://raw.githubusercontent.com/liuzheng712/Intro2SVM/master/Intro2SVM.pdf 。
 - 原《程序員編程藝術第一~三十七章PDF》：http://download.csdn.net/detail/v_july_v/6694053 ，本github上的文章已經對此PDF進行了極大的優化和改進。
 - 《微軟面試100題系列之PDF》：http://download.csdn.net/detail/v_july_v/4583815
 - 《十五個經典算法研究與總結之PDF》：http://download.csdn.net/detail/v_july_v/4478027
 - 編程藝術HTML網頁版：http://taop.marchtea.com/
 - 截止到2014年12.9日，結構之法算法之道blog所有155篇博文集錦CHM文件下載地址：http://pan.baidu.com/s/1gdrJndp
- July團隊高校講座PPT
 - 2014年4月29日《武漢華科大第5次面試&算法講座PPT》：http://pan.baidu.com/s/1hqh1E9e ；
 - 2014年9月3日西電第8次面試&算法講座視頻：http://v.youku.com/v_show/id_XNzc2MDYzNDg4.html ；PPT：http://pan.baidu.com/s/1pJ9HFqb ；
 - 北京10月機器學習班的所有上課PPT：http://yun.baidu.com/share/home?uk=4214456744&view=share#category/type=0；
- July新書初稿的4個PDF
 - B樹的PDF：http://yun.baidu.com/s/1jGwup5k ；
 - 海量數據處理的PDF：http://yun.baidu.com/s/1dDreICL ；
 - 支持向量機的PDF：http://yun.baidu.com/s/1ntwof7j ；
 - KMP的PDF：http://yun.baidu.com/s/1eQel3PK ；
- 七月在線經典課程
 - 4 月機器學習在線班：https://www.julyedu.com/course/getDetail/35 。歷時一年打磨、入門實戰首選、實時答疑。
 - 5 月深度學習班：https://www.julyedu.com/course/getDetail/37 。由淺入深、涵蓋CNN、RNN及其工程應用。
 - 《6 月數據挖掘班》：https://www.julyedu.com/course/getDetail/40 ，海量真實數據、包含大量工程應用、實踐案例。
 - 《機器學習中的數學班》：https://www.julyedu.com/course/getDetail/41 。專為複習、鞏固機器學習中所需的數學基礎。
 - 《Python數據分析班》：https://www.julyedu.com/course/getDetail/43 ，入門數學科學的最佳課程。
 - 《計算機視覺班》：https://www.julyedu.com/course/getDetail/44 ，從CV基礎到深度學習實戰，9.3日開班。
 - 《機器學習與量化交易項目班》：https://www.julyedu.com/course/getDetail/46 ，項目驅動、真實數據，市場上極為難得的項目班，16年9.24日開班。
 - 10 月機器學習算法班 [王牌課程 八大升級]：https://www.julyedu.com/course/getDetail/47 。課程實戰性極強。16年10.15日開班。
 - 11月深度學習班 [通向無人駕駛的必經之路]：https://www.julyedu.com/course/getDetail/49 。專注直播答疑、解決疑難問題、入門DL首選、BAT技術主管上課。
- July’ blog
 - CNN筆記：通俗理解卷積神經網絡 http://blog.csdn.net/v_july_v/article/details/51812459
  - 結構之法 算法之道ML/DL系列博客：http://blog.csdn.net/v_JULY_v/article/category/1061301
 - 教你從頭到尾利用DL學梵高作畫 ① CPU版教程：http://blog.csdn.net/v_july_v/article/details/52683959 ；② GPU版教程：http://blog.csdn.net/v_july_v/article/details/52658965
 - 學汪峰作詞 MAC torch char-rnn 教程：https://ask.julyedu.com/question/7405
 - 基於torch學汪峰寫歌詞、聊天機器人、圖像著色/生成、看圖說話、字幕生成：http://blog.csdn.net/v_july_v/article/details/52796239
 - 教你從頭到尾利用DQN自動玩flappy bird（全程命令提示，GPU+CPU版）：http://blog.csdn.net/v_july_v/article/details/52810219
 - beautiful soup和網絡爬蟲初步：http://mp.weixin.qq.com/s?__biz=MzI4MTQ2NjU5NA==&mid=2247483666&idx=1&sn=af802b39d42d9073e24c086790457004&chksm=eba9829fdcde0b8929ae724f0b41d49091c88fc1e07f697c5030516f33b1bcfcffdace83bcf7&mpshare=1&scene=23&srcid=1102C9RJJ1RXE6bHwFQzLy6W#rd
- 持續更新..
