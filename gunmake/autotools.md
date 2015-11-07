# Autotools

<div id="posttext">
<p>
在<a href="http://tetralet.luna.com.tw/index.php?op=ViewArticle&amp;articleId=185&amp;blogId=1">上一個章節</a>中，我們展示了如何利用 <span style="color: #3366ff"><tt>Makefile</tt></span>
來做為編譯軟體時的輔助工具。但在實務上，因為每個人的環境多多少少有些不同<tt>，Makefile</tt>
往往得隨之修改才能順利進行編譯，這其實還是不夠人性化。同時，<tt>Makefile</tt>
對於大部份的人而言和外星文也差不多，要求人們自行修改 <tt>Makefile</tt>
以符合自己系統的需求未免太強人所難。為了解決這些問題，能依據各個系統之不同而自動建立 <tt>Makefile</tt>
檔的工具 － <tt><span style="color: #993300">autoconf</span></tt>
及 <tt><span style="color: #993300">automake</span></tt>
就順應而生了。
</p>
<p>
<tt>autoconf</tt> 及 <tt>automake</tt>
會依據程式開發者的設定來自動產生 <tt>configure</tt>、<tt>config.h.in</tt> 及 <tt>Mackfile.in</tt>
檔，而使用者則只要執行 <span style="color: #3366ff"><tt>configure</tt></span>
這個 Script 就能自動產生適合其系統的 <tt>Makefile</tt>
及 <tt>config.h</tt>
了；而有了適切的 <tt>Makefile</tt> 及 <tt>config.h</tt>，那麼接下來的編譯工作將會輕鬆許多。而以上就是著名的 <tt><span style="color: #993300">[./configure]</span></tt>、<tt><span style="color: #993300">[make]</span></tt>、<tt><span style="color: #993300">[make
install]</span></tt> 三部曲。雖然乍聽之下，就可以猜出來事情一定不會那麼簡單，但只要依以下步驟進行，其實也不會複雜到哪裡去。
</p>
<p>

</p>
<h2>範例程式</h2>
<p>
以下是我們在本文中所採用的範例程式，<tt><a href="http://lilyterm.luna.com.tw/">LilyTerm</a></tt>：
</p>
<blockquote>
	<img src="images/LilyTerm.png" alt="Gtk+2 範例程式" title="Gtk+2 範例程式" width="829" height="546">
</blockquote>
其檔案結構如下：
<blockquote>
	<table border="1" cellpadding="2" cellspacing="0" width="100%">
		<tbody>
			<tr>
				<td nowrap="nowrap" valign="top"><tt>.</tt><br>
				<tt>|-- AUTHORS</tt><br>
				<tt>|-- COPYING</tt><br>
				<tt>|-- ChangeLog</tt><br>
				<tt>|-- INSTALL</tt><br>
				<tt>|-- NEWS</tt><br>
				<tt>|-- README</tt><br>
				<tt>|-- TODO</tt><br>
				<tt>|-- data</tt><br>
				<tt>|&nbsp;&nbsp; |-- lilyterm.desktop</tt><br>
				<tt>|&nbsp;&nbsp; |-- lilyterm.png</tt><br>
				<tt>|&nbsp;&nbsp; |-- lilyterm.rc</tt><br>
				<tt>|&nbsp;&nbsp; `-- lilyterm.xpm</tt><br>
				<tt>|-- po</tt><br>
				<tt>|&nbsp;&nbsp; |-- lilyterm.pot</tt><br>
				<tt>|&nbsp;&nbsp; `-- zh_TW.po</tt><br>
				<tt>`-- src</tt><br>
				<tt>&nbsp;&nbsp;&nbsp; |-- Makefile</tt><br>
				<tt>&nbsp;&nbsp;&nbsp; |-- dialog.c</tt><br>
				<tt>&nbsp;&nbsp;&nbsp; |-- dialog.h</tt><br>
				<tt>&nbsp;&nbsp;&nbsp; |-- main.c</tt><br>
				<tt>&nbsp;&nbsp;&nbsp; |-- main.h</tt><br>
				<tt>&nbsp;&nbsp;&nbsp; |-- menu.c</tt><br>
				<tt>&nbsp;&nbsp;&nbsp; |-- menu.h</tt><br>
				<tt>&nbsp;&nbsp;&nbsp; |-- notebook.c</tt><br>
				<tt>&nbsp;&nbsp;&nbsp; |-- notebook.h</tt><br>
				<tt>&nbsp;&nbsp;&nbsp; |-- page.h</tt><br>
				<tt>&nbsp;&nbsp;&nbsp; |-- profile.c</tt><br>
				<tt>&nbsp;&nbsp;&nbsp; `-- profile.h</tt> </td>
			</tr>
		</tbody>
	</table>
</blockquote>
<p>
我們將會展示如何利用 <tt>autoconf</tt> 和 <tt>automake</tt>
所提供的工具來替 <tt>LilyTerm</tt> 產生適切的 <tt>configure</tt>、<tt>config.h.in</tt>
和 <tt>Makefile.in</tt>
檔。雖然根據本文所提供的資訊可能還不足以應付一些複雜的大型程式，但只要能掌握基本法則，剩下的就應該不難克服了。
</p>
<h2>需事先安裝套件</h2>
<p>
由於本文主角是 <tt>autoconf</tt> 和 <tt>automake，</tt>所以請事先安裝這兩個套件。例：&nbsp;
</p>
<blockquote>
	<table border="1" cellpadding="2" cellspacing="0" width="100%">
		<tbody>
			<tr>
				<td nowrap="nowrap" valign="top"><tt>aptitude install autoconf
				automake</tt> </td>
			</tr>
		</tbody>
	</table>
</blockquote>
<p>
而 <tt>aptitude</tt> 應該會在安裝這兩個套件的同時自動把一些相關的套件也安裝進系統裡。
</p>
<p>
但請注意到，<tt>autoconf </tt>和 <tt>automake</tt>
在可攜性及可用性上下足了苦心：<span style="color: #000066">使用者並不需要在系統裡安裝 <tt>autoconf</tt>
和
<tt>automake</tt>
及其相關套件就能利用 <tt>configure</tt>
來產生適切的 <tt>Makefile</tt> 檔，</span>因為 <tt>configure</tt> 使用的是標準的 shell
語法。而這也是 <tt>autoconf</tt> 和 <tt>automake</tt> 大受開發者歡迎的原因之一。
</p>
<h2>AutoTools 流程圖</h2>
<blockquote>
	<img src="/AutoTools.png" alt="AutoTools 流程圖" title="AutoTools 流程圖" width="694" height="700">
</blockquote>
由於 <tt>automake</tt> 及 <tt>autoconf</tt>
在使用時其步驟頗為繁雜，在閱讀本文的過程中若能同時參考以上流程圖將有助於了解 <tt>automake</tt> 及 <tt>autoconf</tt>
的運作流程。
<h2>建立 configure.ac
</h2>
<p>
<span style="color: #3366ff"><tt>configure</tt></span>
是用來檢查系統環境並決定一些編譯參數及最終檔案的擺放位置等等的一個 Shell Script 檔。如果您查看 <tt>configure</tt>
的內容，您會發現它真的不像是人類的語言，因為它是由 <tt>autoconf</tt> 參考 <span style="color: #993300"><tt>configure.ac</tt></span>
的內容搭配為數眾多的 <tt>M4</tt>
巨集自動產生的，而產生 <tt>configure</tt>
的基本步驟如下：
</p>
<h3>執行 <tt>autoscan</tt>&nbsp;
</h3>
<p>
請在程式的<span style="color: #006600">主目錄</span>執行 <span style="color: #3366ff"><tt>autoscan</tt></span>
來產生 <tt>configure.scan</tt>
檔案。這個檔案將會被我們用來做為 <tt>configure.ac</tt> 的藍本。請將 <tt>configure.scan</tt>
重新命名為 <tt>configure.ac</tt>
並進行下一步：
</p>
<blockquote>
	<table border="1" cellpadding="2" cellspacing="0" width="100%">
		<tbody>
			<tr>
				<td nowrap="nowrap" valign="top"><tt>autoscan</tt> <br>
				<tt> </tt><tt>mv configure.scan configure.ac</tt> </td>
			</tr>
		</tbody>
	</table>
</blockquote>
<tt>autoscan</tt>
還會產生一個 <tt>autoscan.log</tt> 檔案以記錄 <tt>autoscan</tt>
的掃瞄過程。這個檔案可以放心得刪除。
<h3>編輯 <tt>configure.ac</tt></h3>
<p>
<span style="color: #993300"><tt>configure.ac</tt></span> 是一個使用<span style="color: #3366ff"> <tt>Shell</tt> 語法</span>及 <span style="color: #3366ff"><tt>M4</tt>
巨集指令</span>的 Script 檔案，在產生 <tt>configure</tt>
的過程中它可是扮演著舉足輕重的角色：唯有經過適當配置的 <tt>configure.ac</tt> 才能產生合適的 <tt>configure</tt>，而 <tt>configure</tt>
則是 <tt>Makefile</tt>
成敗的關鍵。所以如何設定 <tt>configure.ac</tt>
就成為本文最重要的主題了。
</p>
<p>
另外，舊版的 <tt>autoconf</tt> 在此是使用 <tt>configure.<span style="color: #993300">in</span></tt><span style="color: #9999ff">（而不是 <tt>configure.<span style="color: #993300">ac</span></tt>）</span>。但因為 <tt>*.in</tt>
表示將會被 <tt>configure</tt> 處理的檔案，所以建議改用 <tt>configure.ac</tt> 以避免混淆。
</p>
<blockquote>
	<h4>基本語法： </h4>
	<blockquote>
		<p>
		由於 <tt>autoconf</tt> 主要是使用 <tt>M4</tt> 巨集指令來產生 <tt>configure</tt>
		的，所以在 <tt>configure.ac</tt> 裡所有使用 <tt>M4</tt> 巨集的<span style="color: #993300">參 數</span>都必須依照 <tt>M4</tt> 的語法，以<span style="color: #3366ff"><tt> [ ] </tt></span>及<span style="color: #3366ff"><tt> , </tt></span>做 為<span style="color: #006600">區隔字元</span>。例： 
		</p>
		<blockquote>
			<table border="1" cellpadding="2" cellspacing="0" width="100%">
				<tbody>
					<tr>
						<td nowrap="nowrap" valign="top"><tt>pkg_modules="gtk+-2.0
						&gt;= 2.12.0 glib-2.0 gdk-pixbuf-2.0"</tt> <br>
						<tt>PKG_CHECK_MODULES(PACKAGE<span style="color: #3366ff">, [</span>$pkg_modules<span style="color: #3366ff">]</span>)</tt> </td>
					</tr>
				</tbody>
			</table>
		</blockquote>
		<p>
		但若無混淆之虞時，像是所傳遞的參數裡沒有 "<span style="color: #3366ff"><tt>,</tt></span>"，則 [ ] 可以省略不加。 
		</p>
		<p>
		而若傳遞的參數裡也有 [ ] 的話，為了避免混淆，請將整個參數以 [[ ]] 包起來。但若所傳遞的參數為<span style="color: #993300">說明文字</span>的話，則不可以使用 [[
		]]，否則可能會產生不可預期的結果。 
		</p>
		<p>
		但若有些參數為空字串時，我們可以直接以<span style="color: #3366ff"><tt>
		, </tt></span>區 隔： 
		</p>
		<blockquote>
			<table border="1" cellpadding="2" cellspacing="0" width="100%">
				<tbody>
					<tr>
						<td valign="top"><tt>AC_CHECK_HEADERS([main.h],,,)</tt> </td>
					</tr>
				</tbody>
			</table>
		</blockquote>
		<p>
		或乾脆省略： 
		</p>
		<blockquote>
			<table border="1" cellpadding="2" cellspacing="0" width="100%">
				<tbody>
					<tr>
						<td valign="top"><tt>AC_CHECK_HEADERS([main.h])</tt> </td>
					</tr>
				</tbody>
			</table>
		</blockquote>
		<p>
		另外還有一些小細節要注意的： 
		</p>
		<ul>
			<li>
			<p>
			<tt>M4</tt> 巨集指令和它的左捌號 <span style="color: #006600"><strong>(</strong></span>
			之間請不要加空白，以免產生無法預期的後果。 
			</p>
			</li>
			<li>
			<p>
			以<tt><span style="color: #3366ff"> # </span></tt>或
			<span style="color: #3366ff"><tt>dnl</tt></span>
			開頭的則為註解。<tt>#</tt> 為 Shell 語法，而 <tt>dnl</tt> 語法則用於 <tt>M4</tt> 巨集。 
			</p>
			</li>
			<li>
			<p>
			下文中會介紹一些常用的 <tt>M4</tt> 巨集。其中，<tt>AM_*</tt> 的是 <tt>automake</tt>
			相關巨集，而 <tt>AC_*</tt> 的則為 <tt>autoconf</tt> 相關巨集。 
			</p>
			</li>
		</ul>
		<p>
		請多多參考其它自由軟體專案所附帶的 <tt>configure.in</tt> 或 <tt>configure.ac</tt>，
		應可有助於了解該如何編寫更完善的 <tt>configure.ac</tt>。 
		</p>
	</blockquote>
	<h4>範本，以 <tt>LilyTerm</tt> 為例：</h4>
	<p>
	以下為 <tt>LilyTerm</tt> 的 <tt>configure.ac</tt> 修改之後的最終結果：<span style="color: #9999ff">（<span style="color: #993300">紅色</span>表示需手動編輯）</span> 
	</p>
	<p>
	<span style="color: #9999ff"> </span> 
	</p>
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>#&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
					-*- Autoconf -*-</tt> <br>
					<tt># Process this file with autoconf to produce a configure
					script.</tt> <br>
					<tt> </tt> <br>
					<tt> <span style="color: #9999ff"># autoconf 版本資訊</span></tt>
					<br>
					<tt>AC_PREREQ(2.61)</tt> <br>
					<tt> </tt><tt><span style="color: #9999ff"># 初始化
					autoconf</span></tt> <br>
					<tt>AC_INIT(<span style="color: #993300">LilyTerm,
					0.8.3, tetralet@gmail.com</span>)</tt> <br>
					<tt> </tt><tt><span style="color: #9999ff"># 指定和
					automake 搭配運作</span></tt> <br>
					<tt> <span style="color: #993300">AM_INIT_AUTOMAKE</span></tt>
					<br>
					<tt><span style="color: #993300"> </span></tt><tt><span style="color: #9999ff"># 檢查 src 目錄</span></tt> <br>
					<tt>AC_CONFIG_SRCDIR([<span style="color: #993300">src/main.c</span>])</tt>
					<br>
					<tt> </tt><tt><span style="color: #9999ff">#
					指定要產生 config.h</span></tt> <br>
					<tt>AC_CONFIG_HEADER<span style="color: #993300">S</span>([config.h])</tt>
					<br>
					<tt> </tt> <br>
					<tt> </tt> <br>
					<tt># Checks for programs.</tt> <br>
					<tt>AC_PROG_CC</tt> <br>
					<tt> <span style="color: #993300">AM_PROG_CC_C_O</span></tt>
					<br>
					<tt><span style="color: #993300"> </span></tt> <br>
					<tt># Checks for libraries.</tt> <br>
					<tt> </tt><tt><span style="color: #9999ff">#
					檢查系統是否有安裝相關的函式庫</span></tt> <br>
					<tt> <span style="color: #993300">AM_PATH_GTK_2_0(2.12.0,,AC_MSG_ERROR([You
					need Gtk+ &gt;= 2.12.0 to build $PACKAGE]))</span></tt> <br>
					<tt><span style="color: #993300">AM_PATH_GLIB_2_0(2.14.0,,AC_MSG_ERROR([You
					need Glib &gt;= 2.14.0 to build $PACKAGE]))</span></tt> <br>
					<tt><span style="color: #993300">PKG_CHECK_MODULES(vte,
					[vte &gt;= 0.16.12],, AC_MSG_ERROR([You need libvte &gt;= 0.16.12 to
					build $PACKAGE]))</span></tt> <br>
					<tt><span style="color: #993300">AC_SUBST(CFLAGS,
					[$vte_CFLAGS])</span></tt> <br>
					<tt><span style="color: #993300">AC_SUBST(LDFLAGS,
					[$vte_LIBS])</span></tt> <br>
					<tt><span style="color: #993300">AC_CHECK_LIB([c],[setlocale],,AC_MSG_ERROR([You
					need libc6 (setlocale) to build $PACKAGE]))</span></tt> <br>
					<tt><span style="color: #993300">AC_CHECK_LIB([c],[strlen],,AC_MSG_ERROR([You
					need libc6 (strlen) to build $PACKAGE]))</span></tt> <br>
					<tt><span style="color: #993300">AC_CHECK_LIB([c],[atof],,AC_MSG_ERROR([You
					need libc6(atof) to build $PACKAGE]))</span></tt> <br>
					<tt><span style="color: #993300"> </span> </tt> <br>
					<tt> </tt><tt><span style="color: #9999ff">#
					開啟/關閉某些功能</span></tt> <br>
					<tt> <span style="color: #993300">AC_ARG_ENABLE(hints,
					AC_HELP_STRING([--enable-hints], [turn on window hints]),
					AC_DEFINE(HINTS,, [turn on hints]))</span></tt> <br>
					<tt><span style="color: #993300">AC_ARG_ENABLE(debug,
					AC_HELP_STRING([--enable-debug], [turn on debug]), CFLAGS="$CFLAGS -g")</span></tt>
					<br>
					<tt><span style="color: #993300">PKG_CHECK_EXISTS(gtk+-2.0
					&gt;= 2.10.0, AC_DEFINE(ENABLE_TAB_REORDER,, [Define only if gtk+-2.0
					&gt;= 2.10.0.]))</span></tt> <br>
					<tt><span style="color: #993300"> </span> </tt> <br>
					<tt><span style="color: #9999ff"># L10N 相關</span></tt>
					<br>
					<span style="color: #993300"><tt>AC_PROG_INTLTOOL</tt></span>
					<br>
					<span style="color: #993300"><tt>ALL_LINGUAS="zh_TW"</tt></span>
					<br>
					<span style="color: #993300"><tt> </tt><tt>GETTEXT_PACKAGE="$PACKAGE"</tt></span>
					<br>
					<span style="color: #993300"><tt>AC_SUBST(GETTEXT_PACKAGE)</tt></span>
					<br>
					<span style="color: #993300"><tt>AM_GLIB_GNU_GETTEXT</tt></span>
					<br>
					<span style="color: #993300"><tt>AM_GLIB_DEFINE_LOCALEDIR(LOCALEDIR)</tt></span>
					<br>
					<tt><span style="color: #993300"> </span> </tt> <br>
					<tt># Checks for header files.</tt> <br>
					<tt>AC_HEADER_STDC</tt> <br>
					<tt>AC_CHECK_HEADERS([locale.h stdlib.h string.h])</tt> <br>
					<tt> </tt> <br>
					<tt># Checks for typedefs, structures, and compiler
					characteristics.</tt> <br>
					<tt>AC_C_CONST</tt> <br>
					<tt>AC_TYPE_PID_T</tt> <br>
					<tt> </tt> <br>
					<tt># Checks for library functions.</tt> <br>
					<tt>AC_CHECK_FUNCS([setlocale])</tt> <br>
					<tt> </tt> <br>
					<tt> </tt><tt><span style="color: #9999ff">#
					指定要產生的檔案</span></tt> <br>
					<tt>AC_CONFIG_FILES([</tt> <br>
					<tt><span style="color: #993300">Makefile</span></tt>
					<br>
					<tt><span style="color: #993300">src/Makefile</span></tt>
					<br>
					<tt><span style="color: #993300">po/Makefile.in</span></tt>
					<br>
					<tt><span style="color: #993300">data/Makefile</span></tt>
					<br>
					<tt>])</tt> <br>
					<tt>AC_OUTPUT</tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	<blockquote>
	</blockquote>
	<p>
	其中的重要語法及巨集指令將會在下文中解釋。 
	</p>
	<h4>版本資訊：</h4>
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td valign="top"><tt>AC_PREREQ(<span style="color: #006600">2.61</span>)</tt></td>
				</tr>
			</tbody>
		</table>
		<p>
		表示這個檔案是由 <tt>autoconf 2.6.1</tt> 所產生的。 
		</p>
	</blockquote>
	<h4>初始化資訊：<span style="color: #9999ff">（必要資訊，且必須是第一個呼叫的 <tt>M4</tt>
	巨集）</span></h4>
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>AC_INIT(<span style="color: #006600">FULL-PACKAGE-NAME</span>, <span style="color: #006600">VERSION</span>, <span style="color: #006600">[BUG-REPORT-ADDRESS]</span></tt><tt>, <span style="color: #006600">[PACKAGE_TARNAME]</span></tt><tt>)</tt>
					<tt> </tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	<blockquote>
		請將其中的替換成以下資訊：
		<ul>
			<li>
			<p>
			<span style="color: #336666"><tt>FULL-PACKAGE-NAME</tt></span>：套件的完整名稱。如果在此有包含空白或其它可能不合法字元，請記得加上第 4 個參數。 
			</p>
			</li>
			<li>
			<p>
			<span style="color: #336666"><tt>VERSION</tt></span>：目前的版本號碼。 
			</p>
			</li>
			<li>
			<p>
			<span style="color: #336666"><tt>BUG-REPORT-ADDRESS</tt></span>：錯誤回報的電子郵件位址。可忽略不填。 
			</p>
			</li>
			<li><span style="color: #336666"><tt>PACKAGE_TARNAME</tt></span>：傳給 <tt>AM_INIT_AUTOMAKE</tt> 作為 tarball 的名稱。可忽略不填。</li>
		</ul>
		若同時在 <tt>configure.ac</tt> 中也設定了 <span style="color: #3366ff"><tt>AC_CONFIG_HEADERS</tt></span><span style="color: #9999ff">（請參考下一節《<a href="http://tetralet.luna.com.tw/index.php?op=ViewArticle&amp;articleId=200&amp;blogId=1#AC_CONFIG_HEADERS">定義產生 <tt>configure</tt> 時所需的 <tt>config.h</tt> 檔案</a></span><span style="color: #9999ff">》中的說明</span><span style="color: #9999ff">）</span>， 以上這些參數會在 <tt>configure</tt>
		的過程中寫入 <span style="color: #3366ff"><tt>config.h</tt></span>
		裡。因此，我們可以利用以下語法<span style="color: #9999ff">（像是寫在 <tt>main.h</tt>
		裡）</span>讓 <tt>config.h</tt> 的內容也能於程式內部使用：
		<blockquote>
			<table border="1" cellpadding="2" cellspacing="0" width="100%">
				<tbody>
					<tr>
						<td nowrap="nowrap" valign="top"><tt>#ifdef HAVE_CONFIG_H</tt>
						<br>
						<tt>#&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; include "<span style="color: #993300">config.h</span>"</tt> <br>
						<tt>#endif</tt> </td>
					</tr>
				</tbody>
			</table>
		</blockquote>
		不過，在同目錄下不能有 <tt>config.h</tt> 檔案，否則在 <tt>make</tt> 之時將會參考到同目錄下的 <tt>config.h</tt><span style="color: #9999ff">（而不是主目錄下的 <span style="color: #006600"><tt>config.h</tt></span>）</span>因而導致預期之外的結果。 
	</blockquote>
	<h4>執行建立 <tt>Mackfile.in</tt> 時所需巨集 </h4>
	<blockquote>
		<p>
		一般而言，<tt><span style="color: #993300">Makefile.in</span>
		</tt>可經由手動編 輯，或是由 <span style="color: #3366ff"><tt>automake</tt></span>
		參考 <span style="color: #3366ff"><tt>configure.ac</tt></span>
		及 <span style="color: #3366ff"><tt>Makefile.am</tt></span>
		的內容來自動產生。而使用 <tt>automake</tt> 來產生 <tt>Makefile.in</tt>
		當然是比較理想的作法。但若想要讓 <tt>configure.ac</tt> 能和 <tt>automake</tt> 搭配使用，您必須在
		<tt>configure.ac</tt> 裡加入如下一行，要求在產生 <tt>Makefile</tt>
		之前能先執行一些必要的巨集： 
		</p>
	</blockquote>
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td valign="top"><tt>AM_INIT_AUTOMAKE(<span style="color: #006600">[OPTIONS]</span>)</tt></td>
				</tr>
			</tbody>
		</table>
		<p>
		其中的 <tt><span style="color: #993300">[OPTIONS]</span></tt><span style="color: #9999ff">（以空白分隔）</span>為要傳給 <tt>automake</tt>
		的參數。這些參數會放在 <tt>AUTOMAKE_OPTIONS</tt> 裡傳給所有的 <tt>Makefile.am</tt> 檔。 
		</p>
		<p>
		<tt><span style="color: #993300"> </span></tt> 
		</p>
		<blockquote>
			<table border="0" cellpadding="2" cellspacing="2" width="100%">
				<tbody>
					<tr>
						<td valign="top">註： </td>
						<td valign="top"><tt>AM_INIT_AUTOMAKE(PACKAGE, VERSION,
						[NO-DEFINE])</tt> 是較舊的寫法，因為在 <tt>AC_INIT</tt> 裡已定義這些資訊了。 <tt> </tt> </td>
					</tr>
				</tbody>
			</table>
		</blockquote>
	</blockquote>
	<h4>檢查原始碼目錄：</h4>
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td valign="top"><tt>AC_CONFIG_SRCDIR(<span style="color: #006600">[src/main.c]</span>)</tt> </td>
				</tr>
			</tbody>
		</table>
		<p>
		安全檢驗：<tt>autoconf</tt> 會檢查此檔案來確認該目錄是否存在。在此可任意使用該目錄中的某個檔案作為代表。 
		</p>
	</blockquote>
	<blockquote>
	</blockquote>
	<h4><a name="AC_CONFIG_HEADERS" title="AC_CONFIG_HEADERS"></a>定義產生 <tt>configure</tt>
	時所需的 <tt>config.h</tt> 檔案：</h4>
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td valign="top"><tt>AC_CONFIG_HEADERS([<span style="color: #006600">config.h</span>])</tt></td>
				</tr>
			</tbody>
		</table>
		<p>
		它會收集在原始程式碼裡使用 <tt>#define</tt> 所定義的前置處理常數，並寫入指定的檔案中，<span style="color: #9999ff">（在此為 </span><span style="color: #9999ff"><span style="color: #3366ff"><tt>config.h</tt></span>）</span>，以便於讓
		<tt>autoheader</tt> 處理。一般而言應該不要去更改這個設定值。 
		</p>
		<p>
		另外，提供類似功能的 <tt>AC_CONFIG_HEADER</tt> 以及 <tt>AM_CONFIG_HEADER</tt>
		現已廢棄不再使用了。 
		</p>
	</blockquote>
	<h4>檢查程式： </h4>
	<p>
	若您的程式需要一些特定程式，如：<tt>perl</tt>，才能正確編譯，那麼可以利用以下指令檢查在 $<tt>PATH</tt>
	中是否有這麼一個指令<span style="color: #9999ff">（在本例中則不需要）</span>： 
	</p>
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>AC_CHECK_PROG(HAVE_PERL,
					<span style="color: #006600">perl</span>,, </tt><tt>C_MSG_ERROR([You
					need perl to build $PACKAGE]))</tt></td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	另外，<tt>autoconf</tt> 還內建了一些巨集，像是 <span style="color: #3366ff"><tt> AC_PROG_INSTALL</tt></span><span style="color: #9999ff">（指定用來安裝檔案的 <tt>install</tt></span>
	指令）或是 <span style="color: #3366ff"><tt>AC_PROG_RANLIB</tt></span><span style="color: #9999ff">（指定用來建立 static lib (*.a) 的 <tt>ranlib</tt>
	指令）</span>，若您的程式需要這些指令才能正確編譯，請務必將其加到 <tt>configure.ac</tt>
	裡，否則編譯很可能會失敗。
	<h4>檢查相關函式庫：</h4>
	<p>
	對於 <tt>configure.ac</tt>
	而言，它有一個很重要的使命，便是檢查系統裡是否有安裝了編譯該軟體時所需的函式庫。其檢查的語法如下： 
	</p>
	<span style="color: #9999ff"><tt><span style="color: #3366ff">AC_CHECK_LIB</span></tt></span>， 檢查函式庫所提供的功能：
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>AC_CHECK_LIB(LIBRARY,
					FUNCTION, [ACTION-IF-FOUND],</tt> <br>
					<tt>&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp;
					[ACTION-IF-NOT-FOUND], [OTHER-LIBRARIES])</tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	<span style="color: #9999ff"><tt><span style="color: #3366ff">PKG_CHECK_MODULES</span></tt></span>
	及 <span style="color: #9999ff"><tt><span style="color: #3366ff">P</span></tt></span><span style="color: #3366ff"><tt>KG_CHECK_EXISTS</tt></span>，檢查套件名稱及版本：
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>PKG_CHECK_MODULES(VARIABLE-PREFIX,
					MODULES,</tt> <br>
					<tt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
					[ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])</tt> </td>
				</tr>
			</tbody>
		</table>
		<p>
		&nbsp;
		</p>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>PKG_CHECK_EXISTS(MODULES,</tt>
					<br>
					<tt>&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;
					&nbsp;&nbsp; [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])</tt></td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	<p>
	但很多常見的函式庫裡會有一些事先寫好的<tt> M4</tt> 巨集指令，我們可以直接採用這些指令而不必再去傷腦筋這些指令該怎麼寫了。 
	</p>
	以 <tt>LilyTerm</tt> 為例，我們可以查看它的 <tt>*.h</tt> 檔案，然後將它所需引入的函式庫整理如下：
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>#include
					&lt;gdk/gdkkeysyms.h&gt;</tt> <br>
					<tt>#include &lt;glib/gi18n.h&gt;</tt> <br>
					<tt>#include &lt;gtk/gtk.h&gt;</tt> <br>
					<tt>#include &lt;locale.h&gt;</tt> <br>
					<tt>#include &lt;stdlib.h&gt;</tt> <br>
					<tt>#include &lt;string.h&gt;</tt> <br>
					<tt>#include &lt;vte/vte.h&gt;</tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	也因此，我們可以發現我們需要安裝的套件如下：<span style="color: #9999ff">（可利用 <tt>dpkg
	-S <span style="color: #336666">glib/gi18n.h</span></tt>
	之類的指令來尋找）</span>
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td valign="top"><tt>libgtk2.0-dev</tt> <br>
					<tt>libglib2.0-dev</tt> <br>
					<tt>libc6-dev</tt> <br>
					<tt>libvte-dev</tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	我們可以利用 <tt>dpkg</tt> 指令試著找找看該套件是否有提供 <tt> M4</tt> 巨集指令可以直接取用：
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>dpkg -L <span style="color: #006600">libgtk2.0-dev</span> | grep m4</tt> <br>
					<tt>/usr/share/aclocal/gtk-2.0.m4</tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	我們查看 <tt> /usr/share/aclocal/gtk-2.0.m4</tt> 的內容，發現它有如下的註解：
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt><span style="color: #006600">dnl</span>
					AM_PATH_GTK_2_0([MINIMUM-VERSION,</tt> <br>
					<tt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
					[ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])</tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	因此我們可以在 <tt>configure.ac</tt> 中這麼寫：
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>AM_PATH_GTK_2_0(2.12.0,,</tt>
					<br>
					<tt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
					<span style="color: #006600">AC_MSG_ERROR</span>(</tt><tt>[You
					need </tt><tt>Gtk+ &gt;= 2.12.0 </tt><tt> to build $PACKAGE</tt><tt>]))</tt>
					</td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	而 <tt> libglib2.0-dev</tt> 也有提供類似的 <tt> M4 </tt>巨集指令，於是依樣畫葫蘆如下：
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>AM_PATH_GLIB_2_0(2.14.0,,</tt>
					<br>
					<tt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
					AC_MSG_ERROR(</tt><tt>[You need </tt><tt>Glib &gt;= 2.14.0</tt><tt> </tt><tt>
					to build $PACKAGE</tt><tt>]</tt><tt>))</tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	但<tt> libvte-dev</tt> 就沒有提供任何的 <tt> M4</tt> 巨集指令 可供直接取用，所以我們只好採用標準的 <span style="color: #3366ff"><tt>PKG_CHECK_MODULES</tt></span>
	語法：
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>PKG_CHECK_MODULES(vte,
					[vte &gt;= 0.16.12],,</tt> <br>
					<tt>&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;
					&nbsp; AC_MSG_ERROR([You need libvte &gt;= 0.16.12 to build $PACKAGE]))</tt>
					<tt> </tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	<p>
	在上例中的 <tt>PKG_CHECK_MODULES</tt> 將會根據所傳入的 <span style="color: #006600"><tt>vte</tt></span> 模組而回傳 <tt><span style="color: #006600"><em>vte</em></span>_CFLAGS</tt> 及 <tt><span style="color: #006600"><em>vte</em></span>_LIBS</tt>
	這兩個參數。為了讓程式能順利編譯，在此我們還得將 <tt>CFLAGS</tt> 及 <tt>LDFLAGS</tt> 設定為這兩個參數：
	</p>
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>AC_SUBST(CFLAGS,
					[$vte_CFLAGS])</tt> <br>
					<tt>AC_SUBST(LDFLAGS, [$vte_LIBS])</tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	<p>
	在此透過了 <tt><span style="color: #3366ff">AC_SUBST</span>
	</tt>函數讓之後的程式能 存取到 <tt>$CFLAGS</tt> 及 <tt>$</tt><tt>LDFLAGS</tt>，否則在之後的 <tt>make</tt> 過程中將會因找不到 libvte 而出現錯誤訊息並停止編譯。 
	</p>
	<p>
	另一個類似的常用的函數為 <span style="color: #3366ff"><tt>PKG_CHECK_EXISTS</tt></span>，它提供了和 <tt>PKG_CHECK_MODULES</tt> 類似的功能，但不會回傳任何參數或顯示錯誤訊息。例如，<tt>LilyTerm</tt>
	裡有部份的功能僅適用於 gtk+ 2.10 版以上，我們可以利用這個函數來進行檢查： 
	</p>
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>PKG_CHECK_EXISTS(gtk+-2.0
					&gt;= 2.10.0,</tt> <br>
					<tt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
					<span style="color: #006600">AC_DEFINE</span>(ENABLE_TAB_REORDER,,
					[Define only if gtk+-2.0 &gt;= 2.10.0.]))</tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	<p>
	其中，<span style="color: #3366ff"><tt>AC_DEFINE</tt></span>
	巨集是要求把 <tt>ENABLE_TAB_REORDER</tt> 參數也寫入 <tt>config.h</tt>
	中，讓原始程式也可以存取到這個參數。請參考下一節《<a href="http://tetralet.luna.com.tw/index.php?op=ViewArticle&amp;articleId=200&amp;blogId=1#Enable_Disable">開啟/關閉程式功能</a>》中的說明。 
	</p>
	<p>
	有關於 <tt>PKG_CHECK_MODULES</tt> 及 <tt>PKG_CHECK_EXISTS </tt>的使用方法請 <span style="color: #006600"><tt>man pkg-config</tt></span>
	以取得更詳盡的資訊。 
	</p>
	而 <tt>libc6-dev</tt> 也沒有提供及任何的 <tt> M4 巨集指令</tt> 可供直接取用，在此我們採用了 <span style="color: #3366ff"><tt>AC_CHECK_LIB</tt></span>
	語法以作為示範：
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>AC_CHECK_LIB([c],[setlocale],,AC_MSG_ERROR([You
					need libc6 (setlocale) to build $PACKAGE]))</tt> <br>
					<tt>AC_CHECK_LIB([c],[strlen],,AC_MSG_ERROR([You need libc6
					(strlen) to build $PACKAGE]))</tt> <br>
					<tt>AC_CHECK_LIB([c],[atof],,AC_MSG_ERROR([You need
					libc6(atof) to build $PACKAGE]))</tt> <tt> </tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	<p>
	不過似乎很少人在檢查系統裡是否有安裝 <tt>libc</tt> 的？ 
	</p>
	<h4><a name="Enable_Disable" title="Enable_Disable"></a>開啟/關閉程式功能：</h4>
	<p>
	我們可以在程式碼裡使用了像是 <span style="color: #3366ff"><tt>#if</tt></span>、<span style="color: #3366ff"><tt>#elif</tt></span>、<span style="color: #3366ff"><tt>#ifdef</tt></span> 及 <span style="color: #3366ff"><tt>#ifndef</tt></span>
	等等的條件編譯語法來開啟或關閉程式的某些功能，而這些選項可以寫入 <tt>configure.ac</tt> 裡，讓使用者在執行 <tt>configure
	</tt>時可決定是否要開啟這些功能。 
	</p>
	<p>
	<tt><span style="color: #993300"> </span></tt>首先，請使用 <span style="color: #3366ff"><tt>ifnames</tt></span>
	來檢查檔案裡使用了哪些條件編譯語法，例： 
	</p>
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt><span style="color: #336666"># ifnames src/*.c</span></tt> <br>
					<tt><span style="color: #336666"> </span><span style="color: #993300">HINTS</span> src/config.c
					src/notebook.c</tt> <br>
					<tt> <span style="color: #993300">ENABLE_TAB_REORDER</span>
					src/notebook.</tt> <tt> </tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	然後，使用 <span style="color: #3366ff"><tt>AC_ARG_ENABLE</tt></span>
	來設定開啟或關閉該功能，其語法如下：
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>AC_ARG_ENABLE(FEATURE,
					HELP-STRING,</tt> <br>
					<tt>&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;
					[ACTION-IF-GIVEN], [ACTION-IF-NOT-GIVEN])</tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	<p>
	其中的 <tt>[ACTION-IF-GIVEN]</tt> 及 <tt>[ACTION-IF-NOT-GIVEN]</tt>
	可用 <tt>Shell</tt> 語法或 <tt>M4</tt> 巨集指令。 
	</p>
	以 <tt>LilyTerm</tt> 為例，它可以在編譯時加上 <tt><span style="color: #993300">HINTS</span></tt> 這個參數來啟用 <tt>hints</tt>
	功能。那麼我們可以這樣寫：
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>AC_ARG_ENABLE(hints,</tt>
					<br>
					<tt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
					AC_HELP_STRING([--enable-hints], [Turn on window hints]),</tt> <br>
					<tt>&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;
					AC_DEFINE(HINTS,, [Turn on window hints]))</tt> <tt> </tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	或者，我們可以在編譯時加上 <tt>-g</tt> 參數，以便能用 <tt>gdb</tt> 來進行除錯：
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>AC_ARG_ENABLE(debug,</tt>
					<br>
					<tt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
					AC_HELP_STRING([--enable-debug], [Enable debugging]),</tt> <br>
					<tt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
					<span style="color: #006600">CFLAGS</span>="$CFLAGS
					-g")</tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	而 <tt>AC_ARG_ENABLE</tt> 在處理時，可將結果作為參數放在 <tt>enableval</tt> 以及 <tt>enable_<em>feature</em>
	</tt>裡傳遞給 <tt>[ACTION-IF-GIVEN]</tt> 處理。我們可以利用這個參數進行更複雜的處理，例：
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>AC_ARG_ENABLE(hints,</tt>
					<br>
					<tt>&nbsp;&nbsp;&nbsp;&nbsp; &nbsp; &nbsp; &nbsp;&nbsp;
					&nbsp; <span style="color: #006600">AC_HELP_STRING</span>([</tt><tt>--enable-hints</tt><tt>],
					[</tt><tt>turn on window hints</tt><tt>]))</tt> <br>
					<tt>if test "$enable_hints" != "no"; then</tt> <br>
					<tt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; </tt><tt>AM_PATH_GTK_2_0(2.12.0,,</tt>
					<br>
					<tt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
					AC_MSG_ERROR(You need Gtk+ &gt;= 2.12.0 if you enabled hints.))</tt> <br>
					<tt>fi</tt> <tt> </tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	<h4>L10N 相關函式：</h4>
	<p>
	由於 <tt>LilyTerm</tt> 支援 L10N，所以我們得要求 <tt>configure.ac</tt>
	能同時帶入 L10N 相關函式： 
	</p>
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td valign="top"><tt>AC_PROG_INTLTOOL</tt> <br>
					<tt> </tt> <br>
					<tt>ALL_LINGUAS="<span style="color: #006600">zh_TW</span>"</tt>
					<br>
					<tt> </tt><tt>GETTEXT_PACKAGE="$PACKAGE"</tt> <br>
					<tt>AC_SUBST(GETTEXT_PACKAGE)</tt> <br>
					<tt>AM_GLIB_GNU_GETTEXT</tt> <br>
					<tt>AM_GLIB_DEFINE_LOCALEDIR(LOCALEDIR)</tt> <tt> </tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	其中的 <span style="color: #3366ff"><tt>ALL_LINGUAS</tt></span>
	項目請填入您 <tt>po/</tt> 目錄裡的各語系 <tt>po</tt> 檔列表。
	<p>
	另外，因為在此使用了 <span style="color: #3366ff"><tt>intltool</tt></span>
	來處理 <tt>po/</tt> 目錄，所以在編譯的過程中您還需在系統裡安裝 <span style="color: #993300"><tt>libxml-parser-perl</tt></span>
	套件，否則 <tt>configure</tt> 的過程中將會發生錯誤無法繼續進行。 
	</p>
	<h4>輸出檔案：<span style="color: #9999ff">（必要資訊，且必須是最後一個呼叫的 <tt>M4</tt>
	巨集）</span> </h4>
</blockquote>
<blockquote>
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>AC_CONFIG_FILES([</tt> <br>
					<tt><span style="color: #006600">Makefile</span></tt>
					<br>
					<tt><span style="color: #006600">src/Makefile</span></tt>
					<br>
					<tt><span style="color: #006600">po/Makefile.in</span></tt>
					<br>
					<tt><span style="color: #006600">data/Makefile</span></tt>
					<br>
					<tt>])</tt> <br>
					<tt>AC_OUTPUT</tt> <tt><span style="color: #993300">
					</span></tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
	<p>
	<tt>configure.ac</tt> 的最主要目的便是產生 <tt>Makefile.in</tt>，而在此的<tt> <span style="color: #3366ff">AC_CONFIG_FILES</span></tt> 及 <span style="color: #3366ff"><tt>AC_OUTPUT</tt></span> 則指定了要產生哪些 <tt>Makefile.in</tt>。然後 <tt>automake</tt> 將會根據這個檔案列表，將列表裡的 <em><tt>File</tt></em> 由 <tt><em>File</em><span style="color: #993300">.am</span></tt> 轉換為 <tt><em>File</em><span style="color: #993300">.in</span></tt>。 
	</p>
	<p>
	比如說在上例裡，<tt>automake</tt> 會試著尋找 <tt> Makefile.am</tt>、<tt>src/Makefile.am</tt>
	以及 <tt>data/Makefile.am</tt> 並將其轉換為 <tt> Makefile.in</tt>、<tt>src/Makefile.in</tt>
	以及 <tt>data/Makefile.in</tt>。 而<tt> <span style="color: #3366ff">po/Makefile.in</span></tt> 則會另外特別處理。
	</p>
	<p>
	一般而言，我們會在每個目錄裡都產生一個 <tt>Makefile</tt> 以便於日後維護及除錯。 
	</p>
	當 <tt>autoconf</tt> 執行到 <tt>AC_OUTPUT</tt> 時，它會建立一個 <span style="color: #993300"><tt>config.status</tt></span> 的 Shell
	檔案，並執行它，由它產生各目錄的 <tt>*.in</tt> 檔。<span></span> 
</blockquote>
<h2>建立 Makefile.am</h2>
<p>
就如前言所述，編寫 <tt>Makefile</tt> 實在不是件容易的工作，而 <span style="color: #3366ff"><tt>automake</tt></span>
則可以依據 <span style="color: #3366ff"><tt>Makefile.am</tt></span>
裡的資訊產生 <tt>Makefile.in</tt> 檔，然後 <span style="color: #3366ff"><tt>configure</tt></span>
會根據 <tt>Makefile.in</tt>
的內容來產生適切的 <tt>Makefile</tt>。聽起來頗讓人心動，而實際上操作起來其實也不怎麼困難。 
</p>
<h3>原始 Makefile 內容：</h3>
<blockquote>
	<table border="1" cellpadding="2" cellspacing="0" width="100%">
		<tbody>
			<tr>
				<td nowrap="nowrap" valign="top"><tt>bindir = /usr/bin</tt> <br>
				<tt>etcdir = /etc/xdg</tt> <br>
				<tt>datadir = /usr/share</tt> <br>
				<tt>CC = gcc</tt> <br>
				<tt>CFLAGS = $$CFLAGS -Wall -g</tt> <br>
				<tt> </tt> <br>
				<tt>INCLUDES = -DHINTS \</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				-D</tt><tt>ENABLE_TAB_REORDER \</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				-DPACKAGE_NAME="LilyTerm" \</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				-DPACKAGE="lilyterm" \</tt> <br>
				<tt>&nbsp; &nbsp;&nbsp; &nbsp;&nbsp; &nbsp;&nbsp;
				-DVERSION="0.8.3" \</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; &nbsp; &nbsp;&nbsp; &nbsp;
				-DPACKAGE_BUGREPORT="tetrlet@gmail.com" \</tt> <br>
				<tt>&nbsp; &nbsp;&nbsp; &nbsp;&nbsp; &nbsp;&nbsp;
				-DLOCALEDIR="$(datadir)/locale" \</tt> <br>
				<tt>&nbsp; &nbsp;&nbsp; &nbsp;&nbsp; &nbsp;&nbsp;
				-DICONDIR="$(datadir)/pixmaps"</tt> <br>
				<tt> </tt> <br>
				<tt>.PHONY: all</tt> <br>
				<tt>all: debug</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; strip lilyterm</tt> <br>
				<tt> </tt> <br>
				<tt>menu.o: page.h dialog.h menu.h menu.c</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; $(CC) $(CFLAGS) $(INCLUDES) -c menu.c
				`pkg-config --cflags gtk+-2.0`</tt> <br>
				<tt> </tt> <br>
				<tt>profile.o: main.h profile.h profile.c</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; $(CC) $(CFLAGS) $(INCLUDES) -c profile.c
				`pkg-config --cflags gtk+-2.0`</tt> <br>
				<tt> </tt> <br>
				<tt>dialog.o: page.h dialog.h dialog.c</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; $(CC) $(CFLAGS) $(INCLUDES) -c dialog.c
				`pkg-config --cflags gtk+-2.0`</tt> <br>
				<tt> </tt> <br>
				<tt>notebook.o: main.h page.h profile.h dialog.h notebook.h
				notebook.c</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; $(CC) $(CFLAGS) $(INCLUDES) -c
				notebook.c `pkg-config --cflags gtk+-2.0`</tt> <br>
				<tt> </tt> <br>
				<tt>main.o: main.h main.c</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; $(CC) $(CFLAGS) $(INCLUDES) -c main.c
				`pkg-config --cflags gtk+-2.0`</tt> <br>
				<tt> </tt> <br>
				<tt>.PHONY: clean</tt> <br>
				<tt>clean:</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; -rm lilyterm</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; -rm *.o</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; -rm ../po/*.mo</tt> <br>
				<tt> </tt> <br>
				<tt>.PHONY: install</tt> <br>
				<tt>install:</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; install lilyterm $(bindir)</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; mkdir -p $(etcdir)</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; install ../data/$(PACKAGE).rc $(etcdir)</tt>
				<br>
				<tt>&nbsp;&nbsp;&nbsp; mkdir -p $(datadir)/pixmaps</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; install ../data/lilyterm.png
				$(datadir)/pixmaps</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; mkdir -p
				$(datadir)/doc/$(PACKAGE)/examples</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; install ../data/$(PACKAGE).rc
				$(datadir)/doc/$(PACKAGE)/examples</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; mkdir -p
				$(datadir)/locale/zh_TW/LC_MESSAGES/</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; install ../po/zh_TW.mo
				$(datadir)/locale/zh_TW/LC_MESSAGES/lilyterm.mo</tt> <br>
				<tt> </tt> <br>
				<tt>.PHONY: uninstall</tt> <br>
				<tt>uninstall:</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; -rm -f $(bindir)/lilyterm</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; -rm -f $(etcdir)/$(PACKAGE).rc</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; -rmdir $(etcdir)</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; -rm -f $(datadir)/pixmaps/lilyterm.png</tt>
				<br>
				<tt>&nbsp;&nbsp;&nbsp; -rmdir $(datadir)/pixmaps</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; -rm -f
				$(datadir)/doc/$(PACKAGE)/examples/$(PACKAGE).rc</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; -rmdir -p
				$(datadir)/doc/$(PACKAGE)/examples</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; -rm -f
				$(datadir)/locale/zh_TW/LC_MESSAGES/lilyterm.mo</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; -rmdir -p
				$(datadir)/locale/zh_TW/LC_MESSAGES</tt> <br>
				<tt> </tt> <br>
				<tt>.PHONY: po</tt> <br>
				<tt>po:</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; xgettext --from-code=UTF-8 -k_ -o
				../po/lilyterm.pot *.c</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; msgmerge ../po/zh_TW.po
				../po/lilyterm.pot -o ../po/zh_TW.po</tt> <br>
				<tt> </tt> <br>
				<tt>.PHONY: debug</tt> <br>
				<tt>debug: menu.o profile.o dialog.o notebook.o main.o</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; $(CC) $(CFLAGS) -o lilyterm menu.o
				profile.o dialog.o notebook.o main.o `pkg-config --cflags --libs
				gtk+-2.0 vte`</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp; msgfmt -o ../po/zh_TW.mo ../po/zh_TW.po</tt>
				</td>
			</tr>
		</tbody>
	</table>
	<p>
	以上是一個原本附在 <tt>LilyTerm</tt> 裡一個十分簡略的 <tt>Makefile</tt>
	檔案。我們將會示範如何編輯 <tt>Makefile.am</tt> 以達到相同的功能。 
	</p>
</blockquote>
<h3>編輯各目錄的 Makefile.am</h3>
<h4>主目錄：&nbsp;
</h4>
<blockquote>
	<p>
	在主目錄下的 <tt>Makefile.am</tt> 只要指明要產生的 <tt>Makefile</tt>
	的目錄即可，所以其內容僅僅如下： 
	</p>
	<table border="1" cellpadding="2" cellspacing="0" width="100%">
		<tbody>
			<tr>
				<td valign="top"><tt>SUBDIRS = src po data</tt> </td>
			</tr>
		</tbody>
	</table>
	<p>
	<span style="color: #3366ff"><tt>SUBDIRS</tt></span>
	指明了要處理的子目錄及順序。 
	</p>
</blockquote>
<h4><tt>src</tt> 目錄：</h4>
<blockquote>
	<table border="1" cellpadding="2" cellspacing="0" width="100%">
		<tbody>
			<tr>
				<td nowrap="nowrap" valign="top"><tt>INCLUDES =
				-DICONDIR="$(datadir)/pixmaps"</tt> <br>
				<tt> </tt> <br>
				<tt>bin_PROGRAMS = <span style="color: #006600"><em>lilyterm</em></span></tt>
				<br>
				<tt> <span style="color: #006600"><em>lilyterm</em></span>_CFLAGS&nbsp;
				= $(GTK_CFLAGS)</tt> <br>
				<tt> <span style="color: #006600"><em>lilyterm</em></span>_LDADD&nbsp;&nbsp;
				= $(GTK_LIBS) $(INTLLIBS)</tt> <br>
				<tt> <span style="color: #006600"><em>lilyterm</em></span>_SOURCES
				= page.h \</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				menu.h menu.c \</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				profile.h profile.c \</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				dialog.h dialog.c \</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				notebook.h notebook.c \</tt> <br>
				<tt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				main.h main.c</tt> </td>
			</tr>
		</tbody>
	</table>
</blockquote>
<blockquote>
	<p>
	<tt>bin_PROGRAMS </tt>指的是要產生的執行檔名。在此為 <span style="color: #006600"><em><tt>lilyterm</tt></em></span>。 
	</p>
	<p>
	<span style="color: #006600"><em><tt>lilyterm</tt></em></span><tt>_CFLAGS
	</tt>指定了在產生 <span style="color: #006600"><em><tt>lilyterm</tt></em></span>
	這個執行檔時的 <tt>CFLAGS</tt> 參數。 
	</p>
	<p>
	<span style="color: #006600"><em><tt>lilyterm</tt></em></span><tt>_LDADD</tt>
	指定了在產生 <span style="color: #006600"><em><tt>lilyterm</tt></em></span>
	這個執行檔時要連結的 函式庫。而其中的 <span style="color: #3366ff"><tt>$(INTLLIBS)</tt></span>
	則是用於 L10N。 
	</p>
	<p>
	<tt><span style="color: #006600"><em>lilyterm</em></span>_SOURCES
	</tt>是 指在產生 <span style="color: #006600"><em><tt>lilyterm</tt></em></span>
	這個執行檔時所需要的檔案。 
	</p>
</blockquote>
<h4><tt>po</tt> 目錄：&nbsp;
</h4>
<blockquote>
	<p>
	<tt>po</tt> 目錄裡放的是各語系的翻譯檔。我們可以使用 <span style="color: #3366ff"><tt>intltoolize</tt></span> 指令讓它能和 <tt>automake</tt>
	搭配使用，而我們只要在事先準備好 <span style="color: #3366ff"><tt>POTFILES.in</tt></span>
	這個檔案即可。<tt>POTFILES.in</tt> 裡放的是有翻譯字句的檔案列表，可用以下指令產生： 
	</p>
	<p>
	<tt> </tt> 
	</p>
	<blockquote>
		<table border="1" cellpadding="2" cellspacing="0" width="100%">
			<tbody>
				<tr>
					<td nowrap="nowrap" valign="top"><tt>cd po</tt> <br>
					<tt>touch POTFILES.in</tt> <br>
					<tt>intltool-update -m</tt> <br>
					<tt>mv missing POTFILES.in</tt> </td>
				</tr>
			</tbody>
		</table>
	</blockquote>
</blockquote>
<h4>data 目錄：</h4>
<blockquote>
	<p>
	<tt>LilyTerm</tt> 的 <tt>data</tt> 目錄裡放的是一些不用進行編譯的檔案。所以它的 <tt>Makefile.am</tt>
	所使用的語法也稍稍有些不同： 
	</p>
</blockquote>
<blockquote>
	<table border="1" cellpadding="2" cellspacing="0" width="100%">
		<tbody>
			<tr>
				<td nowrap="nowrap" valign="top"><tt><span style="color: #006600"><em>etc</em></span>dir =
				$(sysconfdir)/xdg</tt> <br>
				<tt> <span style="color: #006600"><em>etc</em></span>_DATA
				= $(PACKAGE).rc</tt> <br>
				<tt> </tt> <br>
				<tt> <span style="color: #006600"><em>desktop</em></span>dir
				= $(datadir)/applications</tt> <br>
				<tt> <span style="color: #006600"><em>desktop</em></span>_DATA
				= $(PACKAGE).desktop</tt> <br>
				<tt> </tt> <br>
				<tt> <span style="color: #006600"><em>pixmaps</em></span>dir
				= $(datadir)/pixmaps</tt> <br>
				<tt> <span style="color: #006600"><em>pixmaps</em></span>_DATA
				= $(PACKAGE).png $(PACKAGE).xpm</tt> <br>
				<tt> </tt> <br>
				<tt> <em><span style="color: #006600">examples</span></em>dir
				= $(datadir)/doc/$(PACKAGE`)/examples</tt> <br>
				<tt> <span style="color: #006600"><em>examples</em></span>_DATA
				= $(PACKAGE).rc</tt> <br>
				<tt> </tt> <br>
				<tt> <span style="color: #3366ff">EXTRA_DIST</span>
				= $(etc_DATA) $(desktop_DATA) $(pixmaps_DATA) $(examples_DATA)</tt><br>
				<tt> </tt> </td>
			</tr>
		</tbody>
	</table>
	<p>
	<tt><span style="color: #006600"><em>desktop</em></span>dir</tt>
	指的是 <tt><span style="color: #006600"><em>desktop</em></span></tt>
	這個項目的安裝目錄。 
	</p>
	<p>
	<tt><span style="color: #006600"><em>desktop</em></span>_DATA</tt>
	指的是 <tt><span style="color: #006600"><em>desktop</em></span></tt>
	這個項目的檔案列表。 
	</p>
	<p>
	<span style="color: #3366ff"><tt>EXTRA_DIST</tt></span>
	是指這些檔案<span style="color: #993300">不用編譯</span>，直接將其安裝到指定目錄即可。 
	</p>
</blockquote>
<h2>執行 autotools</h2>
<p>
當我們準備好了以上檔案之後，接下來的工作就輕鬆了。我們只要執行幾個指令就能產生適切的 <span style="color: #3366ff"><tt>configure</tt></span>
檔案了。 
</p>
<h3>執行 <tt>aclocal</tt></h3>
<p>
如果在 <tt>configure.ac</tt> 裡有使用到一些巨集指令，如：<tt>AM_PATH_GTK_2_0</tt>，則必須執行此指令來產生 <span style="color: #3366ff"><tt>aclocal.m4</tt></span>
及 <span style="color: #3366ff"><tt>acsite.m4</tt></span> 檔。
</p>
<p>
在執行 <tt>aclocal</tt> 的過程中會產生 <tt>autom4te.cache</tt>
這個暫存目錄。我們事後可以放心得將其刪除。
</p>
<h3>執行 <tt>autoheader</tt></h3>
<p>
如果在程式碼中有用到 <tt>#define</tt> 語法的話，則必須額外下達這個指令。它會根據 <tt>configure.ac</tt>
的內容並搭配 <tt>aclocal.m4</tt>、<tt>acsite.m4</tt> 而產生 <span style="color: #3366ff"><tt>config.h.in</tt></span>
檔案。 
</p>
<h3>執行 <tt>intltoolize</tt><tt> --automake --copy --force</tt></h3>
<p>
如果該程式有支援多國語系的話，我們必須下達這個指令，讓 <tt>automake</tt> 能夠處理 <tt>po</tt>
目錄：&nbsp;
</p>
<blockquote>
	<table border="1" cellpadding="2" cellspacing="0" width="100%">
		<tbody>
			<tr>
				<td nowrap="nowrap" valign="top"><tt>intltoolize --automake
				--copy --force</tt> </td>
			</tr>
		</tbody>
	</table>
</blockquote>
在執行 <tt>intltoolize</tt>
的過程中會產生 <tt>intltool-extract.in</tt>、<tt>intltool-merge.in</tt>、<tt>intltool-update.in</tt>
以及 <tt>po/Makefile.in.in</tt> 檔。一般而言，在事後大多不必刪除這幾個檔案。
<h3>執行<tt> automake --add-missing --copy</tt></h3>
<blockquote>
	<table border="1" cellpadding="2" cellspacing="0" width="100%">
		<tbody>
			<tr>
				<td nowrap="nowrap" valign="top"><tt>automake --add-missing
				--copy</tt> </td>
			</tr>
		</tbody>
	</table>
</blockquote>
<p>
以上指令會根據 <span style="color: #3366ff"><tt>configure.ac</tt></span>
的內容以及各目錄中的 <span style="color: #3366ff"><tt>Makefile.am</tt></span>
檔案，自動在各目錄中產生 <span style="color: #3366ff"><tt>Makefile.in</tt></span>。另外，根據 <tt>configure.ac</tt>
以及各目錄中的 <tt>Makefile.am</tt> 的內容，它可能會同時產生 <tt>compile</tt>、<tt>config.guess</tt>、<tt>config.sub</tt>、<tt>install-sh</tt>、<tt>missing</tt>、<tt>depcomp</tt>
等檔案。 
</p>
<p>
除此之外，<tt>automake</tt> 還會檢查一些像是 <tt>COPYING</tt> 等等的必要檔案以符合 <tt>gnu</tt>
的規範。如果您不想這麼麻煩，在執行 <tt>automake</tt> 指令時請加上 <span style="color: #993300"><tt>--foreign</tt></span>
參數。
</p>
<p>
而如果在主目錄下沒有 <span style="color: #3366ff"><tt>INSTALL</tt></span>
這個檔案，在執行
<tt>automake</tt>
的過程中會替您補上一個制式的 <tt>INSTALL</tt> 檔，所以您就沒必要再費心去自行撰寫這個檔案了。
</p>
<h3>最後的步驟：執行 <tt>autoconf</tt></h3>
<p>
最後一個要執行的指令為 <tt>autoconf</tt>，它會根據 <span style="color: #3366ff"><tt>configure.ac</tt></span>
的內容並搭配 <span style="color: #3366ff"><tt>aclocal.m4</tt></span>、<span style="color: #3366ff"><tt>acsite.m4</tt></span>
而產生我們期盼已久的 <span style="color: #993300"><tt>configure</tt></span>
檔案。
</p>
至此應該算是大功告成，我們千呼萬喚的 <tt>configure</tt> 檔案終於現身了！完成之後，我們就可以將 <tt>LilyTerm</tt>
這個專案以 <tt>tarball</tt> 的形式發佈，而大部份的使用者及套件打包人員應該就能輕鬆得編譯、安裝及打包這個套件了。
<h2><tt>make</tt> 的使用方式</h2>
<p>
以下列出的是採用 <tt>autoconf</tt>
和 <tt>automake</tt> 時，<span style="color: #3366ff"><tt>make</tt></span>
指令常見的用法： 
</p>
<blockquote>
	<table border="0" cellpadding="2" cellspacing="2">
		<tbody>
			<tr>
				<td nowrap="nowrap" valign="top"><tt>make：</tt></td>
				<td valign="top">進行編譯。</td>
			</tr>
			<tr>
				<td nowrap="nowrap" valign="top"><tt>make clean：</tt></td>
				<td valign="top">清除在 <tt>make</tt> 的過程中所產生的執行檔及 <tt>*.o</tt>
				目的檔。</td>
			</tr>
			<tr>
				<td nowrap="nowrap" valign="top"><tt>make distclean：</tt></td>
				<td valign="top">執行 <tt>make clean</tt>，並同時清除 <tt>configure</tt>
				所產生的 <tt>Makefile</tt> 檔。</td>
			</tr>
			<tr>
				<td nowrap="nowrap" valign="top"><tt>make install：</tt></td>
				<td valign="top">將檔案安裝至系統裡。</td>
			</tr>
			<tr>
				<td nowrap="nowrap" valign="top"><tt>make dist：</tt></td>
				<td valign="top">將原始碼壓縮成 <tt>*.tar.gz</tt> 檔。 </td>
			</tr>
			<tr>
				<td nowrap="nowrap" valign="top"><tt>make distcheck：</tt></td>
				<td valign="top">將原始碼壓縮成 <tt>*.tar.gz</tt> 檔，並進行測試。</td>
			</tr>
		</tbody>
	</table>
</blockquote>
<h2>日後維護</h2>
<p>
而日後如果程式的某些部份有所更新，請依以上步驟稍加修改，或是直接執行 <span style="color: #993300"><tt>autoreconf</tt></span>
指令，就能產生新的 <tt>configure</tt> 及 <tt>Makefile.in</tt>。由此可以看到，使用 <tt>autoconf</tt>
和 <tt>automake</tt> 這些工具將會讓日後的維護工作輕鬆許多。 
</p>
<h2>參考資訊</h2>
<blockquote>
	<p>
	<a href="http://www.gnu.org/software/automake/manual/index.html">automake
	manual</a> 
	</p>
	<p>
	<a href="http://www.gnu.org/software/autoconf/manual/index.html">autoconf
	manual</a> 
	</p>
</blockquote>
</div>