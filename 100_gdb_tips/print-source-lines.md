# 打印源代碼行

## 例子

	$ gdb -q `which gdb`
	(gdb) l
	15	
	16	   You should have received a copy of the GNU General Public License
	17	   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */
	18	
	19	#include "defs.h"
	20	#include "main.h"
	21	#include <string.h>
	22	#include "interps.h"
	23	
	24	int

## 技巧

如上所示，在gdb中可以使用`list`（簡寫為l）命令來顯示源代碼以及行號。`list`命令可以指定行號，函數：

	(gdb) l 24
	(gdb) l main

還可以指定向前或向後打印：

	(gdb) l -
	(gdb) l +

還可以指定範圍：

	(gdb) l 1,10

詳情參見[gdb手冊](https://sourceware.org/gdb/onlinedocs/gdb/List.html#List)

## 貢獻者

xmj

