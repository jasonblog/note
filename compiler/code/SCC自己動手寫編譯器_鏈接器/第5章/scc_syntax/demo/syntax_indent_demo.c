/*********************************************************** 
 * syntax_indent_demo.c
 **********************************************************/
struct point{int x;  int y;};
void main()
{int arr[10]; int i;	struct point pt;
pt.x =1024;pt.y=768;
for(i = 0; i < 10; i = i + 1)	{arr[i]=i;			
if(i == 6){continue;		}
else{printf("arr[%d]=%d\n",i,arr[i]);}}
printf("pt.x = %d, pt.y = %d\n",pt.x,pt.y);}

