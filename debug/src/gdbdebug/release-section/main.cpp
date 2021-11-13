#include <iostream>
#include <fstream>
#include <string>
using namespace std;
/*
二叉树节点信息
*/
template<class T> class BiNode
{
public:
	T data;
	BiNode <T>* lch;
	BiNode <T>* rch;
	BiNode() :lch(NULL), rch(NULL) { };
};

template<class T> class BST
{
public:
	BiNode<T>* root;
public:
	BST() { root = NULL; }
	void CreateBST(T a[], int n);
	void InsertBST(BiNode<T>*& R, BiNode<T>* s); //插入
	BiNode<T>* SearchBST(BiNode<T>* R, T key);//查找

	bool DeleteBST(BiNode<T>*& R, T key);  //删除
	void Delete(BiNode<T>*& R);

	void Inorder(BiNode<T>* R);
	void ShowEven(BiNode<T>* R);
	void SearchMatch(BiNode<T>* R,T key1,T Key2);
	void Release(BiNode<T>* R);
	void InsertNode(int number);
	~BST() { Release(root); }
};
/*
初始化一个二叉树，从一个数组中来创建一个二叉树
*/
template<class T>
void BST<T>::CreateBST(T a[], int n)
{
	for (int i = 0; i < n; i++)
	{
		BiNode<T>* s = new  BiNode<T>;
		s->data = a[i];
		InsertBST(root, s);
	}
}
/*
插入一个节点到二叉树中
这里只是构建一个二叉树节点，然后调用真正的插入到二叉树的操作
InsertBST
*/
template<class T>
void BST<T>::InsertNode(int number)
{
		BiNode<T>* s = new  BiNode<T>;
		s->data = number;
		InsertBST(root,s);
}
/*
将节点插入到二叉树中
*/
template<class T>
void BST<T>::InsertBST(BiNode<T>*& R, BiNode<T>* s)
{
	if (R == NULL)
		R = s;
	else
	{
		BiNode<T>** p = &R;
		while (*p != NULL)
		{
			//如果要插入的值比左子树小，则插入到左子树
			if (s->data <= (*p)->data)
				p = &(*p)->lch;
			else				  //否则插入到右子树
				p = &(*p)->rch;
		}
		*p = s;
	}
}
/*
在二叉树中继续关键字查找
*/
template<class T>
BiNode<T>* BST<T>::SearchBST(BiNode<T>* R, T key) //非递归
{
	while (R != NULL)
	{
		if (key == R->data)
			return R;
		else if (key < R->data)//如果关键字小于有子树，则向左子树查找
			R = R->lch;
		else
			R = R->rch;	//否则向右子树查找
	}
	return NULL;
}
/*
从二叉树中删除一个节点
*/
template <class T>
bool  BST<T>::DeleteBST(BiNode<T>*& R, T key)
{
	if (R == NULL) 	      return false;
	else
	{
		/*
		采用递归方式查找，删除节点
		*/
		if (key == R->data)     Delete(R);
		else if (key < R->data)   DeleteBST(R->lch, key); //如果关键字比右子树值小，则删除左子树对应的节点
		else                    DeleteBST(R->rch, key);	//否则，删除右子树匹配的节点
		return true;
	}
}
/*
真正地从二叉树中删除节点
*/
template <class T>
void  BST<T>::Delete(BiNode<T>*& R)
{
	BiNode<T>* q, * s;
	if (R->lch == NULL)
	{
		q = R; R = R->rch; delete q;
	}
	else if (R->rch == NULL)
	{
		q = R; R = R->lch; delete q;
	}
	else { //s是R的前驱， q是s的双亲
		q = R;
		s = R->lch;
		while (s->rch != NULL)
		{
			q = s; s = s->rch;
		}
		R->data = s->data;
		if (q != R)
			q->rch = s->lch;
		else
			R->lch = s->lch;    //q=R表示s为R的左孩子
		delete s;
	}
}

/*
中序变量二叉树
即先输出左节点，再输出当前节点，最后输出右节点，
最后的输出效果就是从小到大
采用递归方式
*/
template<class T>
void BST<T>::Inorder(BiNode<T>* R)
{
	if (R != NULL)
	{
		Inorder(R->lch);
		cout << R->data << "  ";
		Inorder(R->rch);
	}
}
/*
显示所有节点值为偶数的节点
*/
template<class T>
void BST<T>::ShowEven(BiNode<T>* R)
{
	if (R != NULL)
	{
		/*
		采用递归的方式查找所有节点值为偶数的节点
		*/
		ShowEven(R->lch);
		if(R->data % 2 == 0)	   //如果值为偶数，就输出
			cout << R->data << "  ";
		ShowEven(R->rch);
	}
}
/*
搜索节点值在区间[key1,key2]的所有满足条件的节点 
*/
template<class T>
void BST<T>::SearchMatch(BiNode<T>* R,T key1,T key2)
{
	if (R != NULL)
	{
		SearchMatch(R->lch,key1,key2);
		/*
		如果匹配条件，则输出
		递归方式
		*/
		if(R->data >= key1 && R->data <= key2)
			cout << R->data << "  ";
		SearchMatch(R->rch,key1,key2);
	}
}

/*
释放所有节点数据
在析构函数里面自动调用
*/
template<class T>
void BST<T>::Release(BiNode<T>* R)
{
	if (R != NULL)
	{
		Release(R->lch);
		Release(R->rch);
		delete R;
	}
}
/*
插入一个节点到二叉树中
*/
void Insert(BST<int> &btree)
{
	cout << "请输入要插入的数:" << endl;
	int number;
	cin >>number;
	btree.InsertNode(number);
}
/*
在二叉树中查找指定值的节点
*/
void Find(BST<int> &btree)
{
	cout << "请输入要查找的数:" << endl;
	int number;
	cin >> number;
	BiNode<int>* p = btree.SearchBST(btree.root,number);
	if(p)
	{
		cout << "找到节点，值为:" << number << endl;
	}
	else
	{
		cout << "没有值为 " << number << " 的节点" << endl;
	}
}
/*
删除指定值的节点
*/
void Delete(BST<int> &btree)
{
	cout << "请输入要删除的数:" << endl;
	int number;
	cin >> number;
	btree.DeleteBST(btree.root,number);
}
/*
查找偶数的节点
*/
void FindEven(BST<int> &btree)
{
	btree.ShowEven(btree.root);
	cout << endl;
}
/*
搜索区间匹配节点
*/
void SearchMatch(BST<int> &btree)
{
	int minval;
	int maxval;
	cout << "请输入最小值:\n";
	cin >> minval;
	cout << "请输入最大值:\n";
	cin >> maxval;
	btree.SearchMatch(btree.root,minval,maxval);
	cout << endl;

}
void Menu(BST<int>& btree)
{
	system("clear");
	cout << "当前二叉树为:" << endl;
	btree.Inorder(btree.root);
	cout << endl;
	cout << "请选择要执行的操作\n1:插入 2:查找 3:删除 4:显示所有值为偶数节点 5:区间搜索 6:退出" << endl;
}
/*
显示二叉树所有节点信息
*/
void ShowTree(BST<int>& btree)
{
	cout << "当前二叉树为:" << endl;
	btree.Inorder(btree.root);
	cout << endl;
}
int main()
{
	int a[] = { 63, 90, 70, 55, 67, 42, 98 };
	BST<int> btree;
	btree.CreateBST(a, sizeof(a)/sizeof(int));
	while(true)
	{
		Menu(btree);
		switch(getchar())
		{
		case '1':
			Insert(btree);
			ShowTree(btree);
			break;
		case '2':
			Find(btree);
			break;
		case '3':
			Delete(btree);
			ShowTree(btree);
			break;
		case '4':
			FindEven(btree);
			ShowTree(btree);
			break;
		case '5':
			SearchMatch(btree);
			break;
		case '6':
			return 0;
		case '\n':
			continue;
		default:
			break;
		}
		getchar();
		getchar();
	}
	return 0;
	
}

