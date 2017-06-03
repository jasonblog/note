#include<iostream>
#include<string>
using namespace std;
int main()
{
    string str1 = "Spend all your time waiting.";
    string str2 = "For that second chance.";
    string str3(str1, 6); //"all your time waiting."
    string str4(str1, 6, 3); //"all"
    char ch_music[] = {"Sarah McLachlan"};
    string str5 = ch_music;
    string str6(ch_music);
    string str7(ch_music, 5); //"Sarah"
    string str8(4, 'a'); //aaaa
    string str9(ch_music + 6, ch_music + 14); //" McLachlan"
    cout << "str1:" << str1 << endl;
    cout << "str2:" << str2 << endl;
    cout << "str3:" << str3 << endl;
    cout << "str4:" << str4 << endl;
    cout << "str5:" << str5 << endl;
    cout << "str6:" << str6 << endl;
    cout << "str7:" << str7 << endl;
    cout << "str8:" << str8 << endl;
    cout << "str9:" << str9 << endl;
    return 0;
}
