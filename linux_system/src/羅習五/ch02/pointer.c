#include <stdio.h>
#include <stdlib.h>

int main()
{
	int *p;			/*指標未給初始值 */
	int ret;
	ret = scanf("%d", p);
	printf("ret = %d, %d", ret, *p);
}
