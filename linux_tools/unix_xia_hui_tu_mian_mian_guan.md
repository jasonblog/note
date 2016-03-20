# UNIX 下绘图面面观


在UNIX下有很多非常优秀的绘图工具，从手绘到语绘的，应有尽有。

最为强大的当属 MetaPost 和 asymptote，据说二者择一即可满足一生绘图的需求。asymptote 是后来者，C 语言风格，支持 3d 效果，也有一群粉丝。我看二者的语法都令人头痛，要精通不易啊。

这种情况下，偷懒是必须的。出于和其他工具软件接口的考虑，我选择 MetaPost 作为我的顶级绘图工具。手绘的我选择 xfigure，语绘的我喜欢 GnuPlot、graphviz、TikZ&PGF 包和 MetaPost，它们侧重点不同，下面逐一介绍。PSTricks 我没用过，据说也很方便，现有的工具足够强大了，所以就不再选 PSTricks 了。

用 GnuPlot 和 XFigure 输出 MetaPost

天底下绘图的工具很多，我用过 Visio、Dia、XFig、GnuPlot 等等，当我用到 MetaPost，我想它应该是终极画图工具了。MetaPost 集严谨与美感于一身，是TeX 用户最最合适的选择。但，MetaPost 语言实在令人抓狂，常用的还好说，稍微复杂一点的就难说了。好在我们有 GnuPlot 和 XFigure，前者是函数绘图工具，后者是手绘工具。它们都能输出 MetaPost 格式的文件 foo.mp，可以直接用 mpost foo.mp 得到 eps 格式的结果。

```sh
set terminal png           # 输出格式
set output "sin.png"       # 输出文件
set samples 40
set isosamples 40
set hidden3d               # 不透明
set title "2-d surface"    # 标题
set xrange [-3:3]
set yrange [-3:3]
set contour                # 等高线
splot sin(x*y)             # 画二维曲面
```

我们利用GnuPlot画函数图像，输出为MetaPost格式的文件，如果需要，可以在结果中做适当修改，然后再利用mpost得到我们想要的东西。

```sh
set terminal mp prologues 2
set output "surface.mp"
set samples 40
set isosamples 40
set hidden3d
set title "2-d surface"
set xrange [-3:3]
set yrange [-3:3]
set contour
splot sin(x*y)
```

可以对自动生成的 surface.mp 进行修改，譬如插入一个公式等等。另外，GnuPlot 还是一些数学软件的图形输出端，如 octave 等。手绘工具 Xfigure 可以导出 MetaPost 格式的文件，也能进行相似的操作。如果不是为了加入数学公式，或者做一些 GnuPlot 不能做的小修改，是没有必要动用 MetaPost 的，直接把 png 格式的图像 \includegraphics 到 LaTeX 中生成 pdf 也不错啊，不一定要搞个 eps 的出来。

##用 graphviz 输出流程图等

graphviz 是基于 dot 语言的绘图工具，对于有向图、无向图、关系图、目录图、流程图等，我认为用 graphviz 比较快捷。好处是：只要描述好对象之间的关系，图的生成是自动的，比 mindmap 还要好用（mindmap 有一个根节点，graphviz 对根节点个数没有限制）。缺点是：图的生成是自动的，有时得到的不是自己想要的。

另外一个不大不小的缺点是：graphviz 只支持中文 utf-8 编码，所以我在 emacs 下写完代码后要将之存为 utf-8 格式才能生成正常显示中文的 png 文件。例如，

```sh
digraph test {
	node[shape=box,fontname="/usr/local/share/fonts/TrueType/simsun.ttf",fontsize=12];
	node0 [shape=house, label="特征函数",fillcolor=red];
	node1 [shape=house, label="Chebyshev不等式",fillcolor=red];
	node2 [shape=house, label="Kolmogorov不等式",fillcolor=red];
	node3 [shape=ellipse, label="Kolmogorov强大数定律"];
	node1 -> Markov弱大数定律 -> Chebyshev弱大数定律 -> Poisson弱大数定律;
	Chebyshev不等式 -> Chebyshev弱大数定律 -> Bernoulli弱大数定律;
	Kolmogorov弱大数定律 -> Markov弱大数定律;
	node0 -> Khintchine弱大数定律 -> Bernoulli弱大数定律;
	node2  -> node3;
}
```

dot语言在emacs下可以实现语法高亮，详情见：/usr/local/share/emacs/site-lisp/graphviz-dot-mode.el。 graphviz自带的手册是：/usr/local/share/doc/graphviz/html/index.htmlC-x RET f utf-8 RET	将当前缓冲区的编码改为 utf-8

```sh
C-x RET c utf-8 RET C-x C-s filename RET	在保存时转换编码格式
C-x RET r utf-8 RET	重新以utf-8编码加载文件
```

##用 LaTeX 的宏包 TikZ&PGF

TikZ&PGF 是一个 LaTeX 宏包，功能直追 MetaPost，可以在 TeX 文件中直接描述所绘图形，pdflatex 后得到含所需图形的 pdf 文件。TikZ&PGF 包在描述路径方面与 MetaPost 的语法是一致的。teTeX 下这个宏包是缺省安装的，现在的版本是2.0，手册（/usr/local/share/doc/latex-pgf/pgfmanual.pdf）有560 页之多，令人畏惧。这个宏包几乎无所不能，例如

```sh
\begin{tikzpicture}
  \node {root}
    child {node {left}}
    child {node {right}
      child {node {child}}
      child {node {child}}
    };
\end{tikzpicture}
```

```sh
\begin{tikzpicture}[domain=0:4]
  \draw[very thin,color=gray] (-0.1,-1.1) grid (3.9,3.9);
  \draw[->] (-0.2,0) -- (4.2,0) node[right] {$x$};
  \draw[->] (0,-1.2) -- (0,4.2) node[above] {$f(x)$};
  \draw[color=red]    plot (\x,\x)             node[right] {$f(x) =x$};
  \draw[color=blue]   plot (\x,{sin(\x r)})    node[right] {$f(x) = \sin x$};
  \draw[color=orange] plot (\x,{0.05*exp(\x)}) node[right] {$f(x) = \frac{1}{20} \mathrm e^x$};
\end{tikzpicture}
```

看手册pp282页，TikZ&PGF 可以画 mindmap，生成的 mindmap 是我见到的最漂亮的。TikZ&PGF 的语法比 MetaPost 要简单，让人感叹 TeX 之伟大。和 MetaPost 相比，TikZ&PGF 是内置的，\usepackage{tikz} 引入宏包后，直接在 TeX 代码中绘图即可。MetaPost 则是一个外置的独立的画图工具，它生成 eps 格式的文件，通过 \includegraphics 插入。可能在编译时耗上有点差别，二者的功能应该在仲伯之间。

##强大的 MetaPost

王垠对 MetaPost 及其中文化做了概要介绍，不再敷述。感兴趣的在网上 google 能找出一片 introductions，也可以看类似一日游的 tutorial。上手的捷径就是搞几个模板，在此基础上修修改改，借此熟悉 MetaPost 语言。