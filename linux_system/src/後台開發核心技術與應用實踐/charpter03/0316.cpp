#include <iostream>
#include <vector>
using namespace std;
int main()
{
    vector<int> iVec;
    cout << "容器大小:" << iVec.size() << "容量:" << iVec.capacity() << endl;

    /*1个元素,容器容量为1*/
    iVec.push_back(1);
    cout << "容器大小:" << iVec.size() << "容量:" << iVec.capacity() << endl;

    /*2个元素,容器容量为2*/
    iVec.push_back(2);
    cout << "容器大小:" << iVec.size() << "容量:" << iVec.capacity() << endl;

    /*3个元素,容器容量为4*/
    iVec.push_back(3);
    cout << "容器大小:" << iVec.size() << "容量:" << iVec.capacity() << endl;

    /*4个元素,容器容量为4*/
    iVec.push_back(4);
    cout << "容器大小:" << iVec.size() << "容量:" << iVec.capacity() << endl;

    /*5个元素,容器容量为8*/
    iVec.push_back(5);
    cout << "容器大小:" << iVec.size() << "容量:" << iVec.capacity() << endl;

    /*6个元素,容器容量为8*/
    iVec.push_back(6);
    cout << "容器大小:" << iVec.size() << "容量:" << iVec.capacity() << endl;

    /*7个元素,容器容量为8*/
    iVec.push_back(7);
    cout << "容器大小:" << iVec.size() << "容量:" << iVec.capacity() << endl;

    /*8个元素， 容器容量为8*/
    iVec.push_back(8);
    cout << "容器大小:" << iVec.size() << "容量:" << iVec.capacity() << endl;

    /*9个元素， 容器容量为16*/
    iVec.push_back(9);
    cout << "容器大小:" << iVec.size() << "容量:" << iVec.capacity() << endl;

    /* vs2005/8 容量增长不是翻倍的，如
    9个元素   容量9
    10个元素 容量13 */

    /* 测试effective stl中的特殊的交换 swap() */
    cout << "容器大小:" << iVec.size() << "容量:" << iVec.capacity() << endl;
    vector<int>(iVec).swap(iVec);

    cout << "临时的vector<int>对象的大小为:" << (vector<int>(iVec)).size() << endl;
    cout << "临时的vector<int>对象的容量为: " << (vector<int>
            (iVec)).capacity() << endl;
    cout << "交换后,当前vector的大小为:" << iVec.size() << endl;
    cout << "交换后,当前vector的容量为:" << iVec.capacity() << endl;

    return 0;
}
