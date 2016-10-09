# A05: introspect


﻿# A05: introspect

## 預期目標
* 為 [code review](https://en.wikipedia.org/wiki/Code_review) 做好必要的準備工作，練習 [Software peer review](https://en.wikipedia.org/wiki/Software_peer_review)
* 歌德說：「要欣賞自己的價值，就得給世界增添價值」，從反省和觀摩中重新檢視自己作品的具體突破，來日提出增添價值的途徑
* [軟體工程師要學會說故事](https://ruddyblog.wordpress.com/2016/06/18/)，從良性詳盡的批評開始

## 作業要求
* 在 [2016q3 Homework1 作業區](/s/H1B7-hGp) 挑出自己以外的 3 項學生開發成果，在開發紀錄後方標注 "::: Reviewed by 你的GitHub帳號名稱"，像是
> 開發紀錄(phonebook) / github / youtube_link ::: Reviewed by <`jserv`>
* 每份開發成果至多只能被 2 個人批評，"Reviewed by" 後面的 GitHub 帳號用逗號 `,` 分隔
* 選定開發紀錄後，編輯內文，加上 `Reviewed by 你的GitHub帳號名稱` 的段落，[示範的 Review](/s/BJjL6cQ6)，你的意見要寫在共筆的最上方，僅次於 "contributed by"。要從以下方面探討:
	* 程式碼的 coding style, git commit messages
	* 實驗設計的不足處、涵蓋程度是否全面，以及後續的改進空間

## Git commit messages
* 詳閱 [How to Write a Git Commit Message](http://chris.beams.io/posts/git-commit/)
* 檢視同學們的程式碼，提出他們撰寫的訊息是否符合前述原則。如果不符合，就直接寫在 Review 內容中

## 示範: [Reviewed by `jserv`](/s/BJjL6cQ6)
* 沒有嘗試不同的 data set，原本的程式輸入是已排序、非典型英文姓氏，這與現實不匹配
* 實做提到透過引入 hash 加速 `fineName()` 的操作，但缺乏不同 hash function 的效能比較和設計取捨
* 在 `append()` 中，`malloc()` 是個顯著的時間開銷，缺乏減緩效能衝擊的方案
* 卻乏搜尋演算法的評估和效能分析
* 考慮到電話簿需要作到動態資料新增和刪除，若引入 hash，面對大量資料時，會有什麼影響？
* 儘管已經整理頗多 perf 和效能測量的資料，但並未反映到此程式效能分析，除了 cache miss，還請一併探討 branch prediction accuracy 等議題
* `main.c` 無法透過 function pointer 來切換和比較不同實做的效能落差，應該先設計一份可通用的軟體界面，然後將 binary tree, hash table, trie 等不同實做機制加入
* 將 `append()` 和 `findName()` 時間加入統計的意義不大，真實應用往往是個別操作，特別在圖表的呈現
* [commit f561e](https://github.com/grapherd/phonebook-1/commit/f561efa41ca9546e87a8e883096bcf3e78c5b64f) 的 Git commit 訊息不需要特別指出檔名，而且不容易理解具體行為和影響，為何要作這樣的修改？
> phonebook_opt.[ch]: move out dict