#include<iostream>
#include<string>
using namespace std;

int main()
{
    size_t length;
    char buffer[8];
    string str("Test string......");
    cout << "str:" << str << endl;

    length = str.copy(buffer, 7, 5);
    buffer[length] = '\0';
    cout << "str.copy(buffer,7,5),buffer contains: " << buffer << endl;

    length = str.copy(buffer, str.size(), 5);
    buffer[length] = '\0';
    cout << "str.copy(buffer,str.size(),5),buffer contains:" << buffer << endl;

    length = str.copy(buffer, 7, 0);
    buffer[length] = '\0';
    cout << "str.copy(buffer,7,0),buffer contains:" << buffer << endl;

    length = str.copy(buffer, 7); //缺省参数pos，默认pos=0；
    buffer[length] = '\0';
    cout << "str.copy(buffer,7),buffer contains:" << buffer << endl;

    length = str.copy(buffer, string::npos, 5);
    buffer[length] = '\0';
    cout << "string::npos:" << (int)(string::npos) << endl;
    cout << "buffer[string::npos]:" << buffer[string::npos] << endl;
    cout << "buffer[length-1]:" << buffer[length - 1] << endl;
    cout << "str.copy(buffer,string::npos,5),buffer contains:" << buffer << endl;

    length = str.copy(buffer, string::npos);
    buffer[length] = '\0';
    cout << "str.copy(buffer,string::npos),buffer contains:" << buffer << endl;
    cout << "buffer[string::npos]:" << buffer[string::npos] << endl;
    cout << "buffer[length-1]:" << buffer[length - 1] << endl;
    return 0;
}
