#include<iostream>
#include<stdlib.h>
#include<vector>
#include<string>
using namespace std;
vector<string> StringSplit(const string& sData, const string& sDelim)
{
    vector<string>vItems;
    vItems.clear();
    string::size_type bpos = 0;
    string::size_type epos = 0;
    string::size_type nlen = sDelim.size();

    while ((epos = sData.find(sDelim, epos)) != string::npos) {
        vItems.push_back(sData.substr(bpos, epos - bpos));
        epos += nlen;
        bpos = epos;
    }

    vItems.push_back(sData.substr(bpos, sData.size() - bpos));
    return vItems;
}
int main()
{
    cout << "Content-type:text/html\r\n\r\n";
    cout << "<html>\n";
    cout << "<head>\n";
    cout << "<title>Hello World - First CGI Program</title>\n";
    cout << "</head>\n";
    cout << "<body>\n";
    cout << "get parameter:<br/>";
    char* value = getenv("QUERY_STRING");

    if (value != 0) {
        /*"a=1&b=2&c=3"*/
        vector<string>paras = StringSplit((const string)value, "&");
        vector<string>::iterator iter = paras.begin();

        for (; iter != paras.end(); iter++) {
            vector<string>singlepara = StringSplit(*iter, "=");
            cout << singlepara[0] << " " << singlepara[1] << "<br/>";
        }

        cout << "</body>\n";
        cout << "</html>\n";
        return 0;
    }
