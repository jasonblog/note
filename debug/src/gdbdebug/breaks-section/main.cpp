#include <iostream>
#include <cstring>
#include <thread>
using namespace std;

typedef struct _TEST_S_
{
	int data;
	struct _TEST_S_* next;
}TEST_S;

void visit_list(TEST_S *list_head)
{
	TEST_S *temp = list_head->next;
	while(temp)
	{
		printf("the value =%d\n",temp->data);
		temp = temp->next;
	}
}
void switch_node(TEST_S* prev_node,TEST_S* node1,TEST_S* node2)
{
	prev_node->next = node2;
	node1->next = node2->next;
	node2->next = node1;
}
void sort_list(TEST_S *list_head)
{
	TEST_S *curr = list_head->next;
	TEST_S *prev = list_head;
	int swtiched = 0;
	while(curr)
	{
		TEST_S* next = curr->next;
		if(!next)
		{
			if(swtiched == 0)
			{
				break;
			}
			else
			{
				swtiched = 0;
				curr = list_head->next;
				prev = list_head;
				continue;
			}
		}
		if(curr->data > next->data)
		{
			switch_node(prev,curr,next);
			prev = next;
			swtiched = 1;
		}
		else
		{
			prev = curr;
			curr = curr->next;
		}
	}
}
void test_linklist()
{
	int i=1;
	TEST_S *temp = NULL;
	TEST_S *head = (TEST_S*)malloc(sizeof(TEST_S));
	head->data = 0;
	head->next = NULL;
	temp = head;
	for(i=10;i>0;i--)
	{
		TEST_S* element = (TEST_S*)malloc(sizeof(TEST_S));
		element->data = rand();
		element->next = NULL;
		temp->next = element;
		temp = element;
	}
	printf("排序前链表元素为:\n");
	visit_list(head);
	printf("排序后链表元素为:\n");
	sort_list(head);
	visit_list(head);
}
int main(int argc,char** argv)
{
	test_linklist();
	return 0;
}
