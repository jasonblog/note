# Multi-pthread note

## 設計想法
1. 多個thread 合作考慮共用的 resource 是什麼？
2. 共用的resource 之間要怎麼加鎖解鎖