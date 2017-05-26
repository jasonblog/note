# 移植 Linux Kernel 造成無法開機之解決方案以及除錯工具


<div class="entry-content">
		<p>一般在以下情況, 我們會進行移植 Linux Kernel 的動作.</p>
<h4><strong><span style="color:#0000ff;">1. 將新版 Linux Kernel 移植到全新 SoC 上</span></strong></h4>
<p>開發人員為 SoC 廠商(e.g. MTK, TI, Allwinner) R&amp;D engineer.</p>
<p>在此階段的移植, 需要寫 dtsi, low-level code (位於 arch// 底下) 以及 SoC 相關的底層驅動程式. 並且須確保 SoC 功能運行正常.</p>
<h4><span style="color:#0000ff;"><strong>2. 將新版 Linux Kernel 移植到全新平臺上</strong></span></h4>
<p>開發人員為硬體廠 的 R&amp;D engineer 或者是開發板 (e.g. beagleboard, RPi) 上游開發人員.</p>
<p>在此階段的移植, 需要建立 dts 來描述硬體資訊, 並適度修改 dtsi. 此外也需要根據硬體/ 週邊/ 平臺資訊來建立屬於板子的 Kernel &nbsp;config. 檔.&nbsp;並且須確保硬體/ 週邊/ 平臺功能運行正常.</p>
<h4></h4>
<h4><strong><span style="color:#0000ff;">3. 將新版 Linux Kernel 移植到舊有平臺上進行 Kernel 版本升級</span></strong></h4>
<p>開發人員為任何有興趣的 Maker. 只要有開發版即可進行 (e.g. beagleboard, RPi).&nbsp;一般來說, Linux Kernel 移植大多屬於此範疇.</p>
<p>在此階段的移植, 需要根據 dtsi 的版本差異來微調 dts 格式. 此外也需要根據 Kernel 版本差異資訊來微調板子的 Kernel &nbsp;config. 檔.</p>
<p>此外也須參照舊有 Kernel source 上的 in-house patch 來修改新版 Kernel source.</p>
<p>* &nbsp;另外還有像將 Linux Kernel 移值到全新 CPU architecture 上面這種較罕見的情況.</p>
<hr>
<p>在移植 Linux Kernel 時, <span style="color:#0000ff;">最重要的第一步就是讓機器可以</span> <strong><span style="color:#ff0000;">" 開機 “</span></strong>. Kernel 能正常開機往下執行, 我們才能進行後續移植動作.</p>
<p>在無法開機的情況下 (請見下圖), 由於沒有任何錯誤訊息, 所以除錯上會比較困難.</p>
<p>一旦可以開機, 畫面有訊息列印出來, 即便有其他問題, 相對起來也比較好解.</p>


<p>
<img src="images/2017-05-15_200559.png">


</p>
<p>最常遇到開不了機的現象就是 hang 在 <span style="color:#ff0000;">“</span><strong><span style="color:#ff0000;">Starting Kernel"</span> </strong>這邊.</p>
<p>在這個階段 bootloader 把 Linux Kernel 載入 Memory 位址 0x80008000, 並將 CPU 控制權交給 Linux Kernel. 此時理因跳轉到 Linux Kernel 第一條指令 [1], 接著 Kernel 開始自解壓縮進行後續開機流程 [2].</p>
<p>如果在這個階段卡住, 代表 Linux Kernel 沒有正確運行, 原因可分成下列幾點</p>
<table class="tg" style="height:168px;" width="769">
<tbody>
<tr>
<th style="text-align:center;" width="20%">無法開機根因</th>
<th style="text-align:center;"><b>可能發生時機</b></th>
<th style="text-align:center;"><b>解決方法</b></th>
</tr>
<tr>
<td style="text-align:center;" width="20%"><strong><span style="color:#0000ff;">SoC&nbsp;low-level code 有問題</span></strong></td>
<td style="text-align:center;">將 Linux&nbsp;移植到新 SoC 上</td>
<td style="text-align:center;">
<p style="text-align:left;">修改 low-level code, 並確保 SoC 裡功能可正常運行. 有可能需要在 driver 中以 work-around 手法避掉問題.</p>
</td>
</tr>
<tr>
<td style="text-align:center;" width="20%"><span style="color:#0000ff;"><strong>舊有 DTS 與 新板 DTSI不相容</strong></span></td>
<td>
<p style="text-align:left;">將新 Linux Kernel 移植到舊有平臺上進行Kernel 升級</p>
</td>
<td style="text-align:left;">閱讀 dtsi git 修改歷程, 並詳細閱讀 <span style="color:#ff0000;">“Documentation/devicetree/bindings/"</span> 相關文件.</td>
</tr>
<tr>
<td style="text-align:center;" width="20%"><span style="color:#0000ff;"><strong>硬體設計有問題</strong></span></td>
<td style="text-align:center;">
<ul>
<li style="text-align:left;">將 Linux&nbsp;移植到新 SoC上</li>
<li style="text-align:left;">將新 Linux Kernel 移植到全新平臺上</li>
</ul>
</td>
<td style="text-align:left;">和 Hardware/ Bootloader 部門 R&amp;D 工程師協同合作, 一起看硬體線路除蟲</td>
</tr>
<tr>
<td style="text-align:center;" width="20%"><strong><span style="color:#0000ff;">Console port 設定錯誤</span></strong></td>
<td style="text-align:left;">
<ul>
<li>將 Linux&nbsp;移植到新 SoC上</li>
<li>將新 Linux Kernel 移植到全新平臺上</li>
<li>將新 Linux Kernel 移植到舊有平臺上進行Kernel 升級</li>
</ul>
</td>
<td style="text-align:left;">
<p style="text-align:left;">每一塊板子的 Console port device node 都需根據硬體設計以及 driver 設計來進行設定. 如果設定錯誤, 會導致訊息出不來</p>
<p style="text-align:left;">一般情況下 console device node 為 ttyS0, baud: 115200</p>
</td>
</tr>
<tr>
<td style="text-align:center;" width="20%"><span style="color:#0000ff;"><strong>Boot code program (包含bootloader) 與 Kernel 不相容</strong></span></td>
<td style="text-align:center;">
<ul>
<li style="text-align:left;">將 Linux&nbsp;移植到新 SoC上</li>
<li style="text-align:left;">將新 Linux Kernel 移植到全新平臺上</li>
<li style="text-align:left;">將新 Linux Kernel 移植到舊有平臺上進行Kernel 升級</li>
</ul>
</td>
<td style="text-align:center;">
<p style="text-align:left;">需確認 boot code program 版本與 Kernel 版本是相對應的. 尤其有些 SoC 的設計會有多個 boot code program.</p>
</td>
</tr>
<tr>
<td style="text-align:center;" width="20%"><span style="color:#0000ff;"><strong>DTB 跟 Kernel memory 位址衝突</strong></span></td>
<td style="text-align:center;">
<ul>
<li style="text-align:left;">將 Linux&nbsp;移植到新 SoC上</li>
<li style="text-align:left;">將新 Linux Kernel 移植到全新平臺上</li>
<li style="text-align:left;">將新 Linux Kernel 移植到舊有平臺上進行Kernel 升級</li>
</ul>
</td>
<td style="text-align:left;">boot code program 需要注意避免將兩者&nbsp;memory 位址設太近導致衝突.</td>
</tr>
<tr>
<td style="text-align:center;" width="20%"><span style="color:#0000ff;"><strong>DTB 檔跟 Kernel 並不相容</strong></span></td>
<td>
<ul>
<li>將 Linux&nbsp;移植到新 SoC上</li>
<li>將新 Linux Kernel 移植到全新平臺上</li>
<li>將新 Linux Kernel 移植到舊有平臺上進行Kernel 升級</li>
</ul>
</td>
<td style="text-align:left;">記得重新編譯 Kernel 時, 也要一併重新編譯 DTB. 並確保兩者是成對的.</td>
</tr>
<tr>
<td style="text-align:center;" width="20%"><span style="color:#0000ff;"><strong>Kernel config 檔中的 ARCH 相關資訊設定錯誤 </strong></span></td>
<td>
<ul>
<li>將 Linux&nbsp;移植到新 SoC上</li>
<li>將新 Linux Kernel 移植到全新平臺上</li>
<li>將新 Linux Kernel 移植到舊有平臺上進行Kernel 升級</li>
</ul>
</td>
<td style="text-align:left;">確定 SoC 種類, 並到Kernel 選單 – &nbsp;System Type 中選取對應的選項</td>
</tr>
</tbody>
</table>
<h4></h4>
<h4><span style="color:#000000;"><strong>下列為一些除錯工具以及技巧</strong></span></h4>
<h4><strong><span style="color:#0000ff;">1. 檢查 dtsi/ dts 是否正確</span></strong></h4>
<p>除了閱讀 dtsi git 修改歷程, 並詳細閱讀 <span style="color:#ff0000;">“Documentation/devicetree/bindings/"</span> 相關文件外, 也可以使用 debug tool 來檢查</p>
<ul>
<li><span style="color:#000000;"><strong>dtx_diff</strong></span> – 用來比較不同 device trees 的差異. 支援 dts/dtsi, dtb 以及 file system tree 格式.</li>
</ul>
<p style="padding-left:60px;">⇒ 位於 <span style="color:#ff0000;">scripts/dtc/dtx_diff</span>, 於 Kernel 4.6 加入</p>
<ul>
<li><span style="color:#000000;"><strong>dt_to_config</strong></span> – 能根據 device tree 資訊來找相對應的 driver, 確認兩邊是相對應的.</li>
</ul>
<p style="padding-left:60px;">⇒ 位於 <span style="color:#ff0000;">scripts/dtc/dt_to_config</span>,&nbsp;於 Kernel 4.8 加入</p>


<img src="images/2017-05-17_102828.png">



<p>其他還有像 dt_stat, dt_node_info&nbsp;[3][4][5] 等等的除錯工具可使用.</p>
<hr>
<h4><strong><span style="color:#0000ff;">2. 開啟 early printk</span></strong></h4>
<p>Kernel 啟動時, 有可能因為 console&nbsp;還沒初始化, 導致一些錯誤訊息無法輸出顯示. 這時可以使用 earlyprintk [6] 提早輸出錯誤訊息.</p>
<h5><strong><span style="color:#000000;">√ 開啟 Kernel 選項</span></strong></h5>

<img src="images/2017-05-16_215007.png">


<h5><strong><span style="color:#000000;">√ 設定 bootargs</span></strong></h5>
<p>在 bootargs 中加入 “earlyprintk" 字串</p>
<div><div id="highlighter_272609" class="syntaxhighlighter  bash"><table border="0" cellpadding="0" cellspacing="0"><tbody><tr><td class="gutter"><div class="line number1 index0 alt2">1</div></td><td class="code"><div class="container"><div class="line number1 index0 alt2"><code class="bash plain">setenv bootargs ${bootargs} earlyprintk</code></div></div></td></tr></tbody></table></div></div>
<p>設定完重開機即啟動 earlyprintk 功能.</p>
<p><span style="color:#ff0000;">下圖為開啟 earlyprintk 才能印出的錯誤訊息</span> – <strong><span style="color:#0000ff;">這段訊息代表 Kernel&nbsp;ARCH 相關資訊設定錯誤.&nbsp;</span></strong></p>
<blockquote><p>Error: unrecognized/unsupported machine ID (r1 = 0x00000e05).</p>
<p>Available machine support:</p>
<p>ID (hex)&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; NAME<br>
ffffffff&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Generic DT based system<br>
ffffffff&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Generic DRA72X (Flattened Device Tree)<br>
ffffffff&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Generic DRA74X (Flattened Device Tree)<br>
ffffffff&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Generic AM43 (Flattened Device Tree)<br>
ffffffff&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Generic OMAP5 (Flattened Device Tree)<br>
ffffffff&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Generic AM33XX (Flattened Device Tree)</p></blockquote>
<hr>
<h5><strong><span style="color:#000000;">√ earlyprintk 運作原理</span></strong></h5>
<p><span style="color:#0000ff;">earlyprintk 位於 arch/arm/kernel/early_printk.c</span></p>
<p>由下圖可得知, 主要呼叫 printch() 來輸出字元</p>

<img src="images/e69caae591bde5908d.png">



<p><span style="color:#0000ff;">printch() 位於 arch/arm/kernel/debug.S</span></p>
<p>由下圖可知 printch() 呼叫<span style="color:#0000ff;"> addruart_current</span></p>


<img src="images/2017-05-17_113828.png">


<p><span style="color:#0000ff;">addruart_current</span> 為巨集, 會再呼叫 <span style="color:#0000ff;">addruart&nbsp;</span></p>

<img src="images/2017-05-17_114026.png">


<p><span style="color:#0000ff;">addruart</span> 則位於&nbsp;<span style="color:#0000ff;">arch/arm/include/debug/&lt;SoC&gt;.S 中</span></p>
<p><span style="color:#0000ff;">addruart</span>&nbsp; 實作上會因不同 SoC 特性而有所差異. 但大多是以組語設定 <strong><span style="color:#ff0000;">UART_PHYS</span> </strong>以及 <strong><span style="color:#ff0000;">UART_VIRT</span></strong>.</p>
<hr>
<h4><strong><span style="color:#0000ff;">3. 檢查 DTB 跟 Kernel memory 位址</span></strong></h4>
<p>我們可以在 bootloader 底下執行下列指令來進行開機</p>
<div><div id="highlighter_927110" class="syntaxhighlighter  bash"><table border="0" cellpadding="0" cellspacing="0"><tbody><tr><td class="gutter"><div class="line number1 index0 alt2">1</div><div class="line number2 index1 alt1">2</div><div class="line number3 index2 alt2">3</div></td><td class="code"><div class="container"><div class="line number1 index0 alt2"><code class="bash plain">fatload mmc 0 80000000 board.dtb</code></div><div class="line number2 index1 alt1"><code class="bash plain">fatload mmc 0 81000000 uImage</code></div><div class="line number3 index2 alt2"><code class="bash plain">bootm 81000000 - 80000000</code></div></div></td></tr></tbody></table></div></div>
<p>&nbsp;</p>
<p>原本都可以正常運作, 但某次發現如果在 device tree 中新增項目, 會無法開機</p>
<p>依照指令來看, 記憶體會是這樣:</p>


<img src="images/2017-05-17_155602.png">




<p>但是實際開機卻會卡住.</p>
<p>仔細分析後才發現, 原來 Kernel 實際執行的記憶體位址會是load address 0x80008000, 而不是 bootm 設定的 0x81000000. 而 load address 可透過 mkimage 或者 Kernel Makefile 來進行設定.</p>

<img src="images/e69caae591bde5908d1.png">





<p><span style="color:#0000ff;">如果 bootm 位址和 load address 不一樣, bootloader 會先把 Kernel move 到 load address</span>, 接著從 entry point 開始執行</p>


<img src="images/2017-05-17_162236.png">






<p>回過來, 那當初為什麼在 device tree 中新增項目, 會造成無法開機呢 ?</p>
<p>我們來算一下記憶體空間</p>
<blockquote><p>Load Address: 0x80008000</p>
<p>DTB Address: 0x80000000</p>
<p>0x80008000 – 0x80000000 = 0x8000 = <strong><span style="color:#0000ff;">32768</span></strong></p></blockquote>
<p>由上述可知, 只要 dtb 檔案大小 &gt; 32768, 就會覆蓋到 Linux Kernel. 造成檔案毀損而導致無法開機. 所以只要修改 Load address 即可解決.</p>
<hr>
<p><span style="color:#0000ff;"><strong>心得:</strong></span></p>
<p>移植 Linux Kernel 時, 除了要熟悉 SoC/ 週邊/ 硬體資訊外, 也要多閱讀 Linux Documentation 的資料.</p>
<p>魔鬼藏在細節, 只要有一個地方疏忽掉, 就會導致開不了機. 所以要移植 Linux Kernel 時, 最重要的事 – <strong><span style="color:#ff0000;">請睡飽 <img draggable="false" class="emoji" alt="😀" src="https://s0.wp.com/wp-content/mu-plugins/wpcom-smileys/twemoji/2/svg/1f600.svg" scale="0"></span></strong></p>
<p>&nbsp;</p>
<hr>
<p>ref:</p>
<p>[1]:&nbsp;https://www.kernel.org/doc/Documentation/arm/Booting</p>
<p>[2]:&nbsp;https://szlin.me/2016/06/11/linux-fast-boot-%E5%BF%AB%E9%80%9F%E9%96%8B%E6%A9%9F%E8%AA%BF%E6%A0%A1/</p>
<p>[3]:&nbsp;http://www.elinux.org/images/8/88/Dt_stat.patch</p>
<p>[4]:&nbsp;http://elinux.org/images/b/ba/Dt_stat_150811_1802.patch</p>
<p>[5]:&nbsp;http://elinux.org/Device_Tree_frowand</p>
<p>[6]:&nbsp;https://www.kernel.org/doc/Documentation/driver-model/platform.txt</p>
