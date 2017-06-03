#include<iostream>
#include<stdlib.h>
#include<stdio.h>
using namespace std;
int main()
{
    cout << "Content-type:text/html\r\n\r\n";
    cout << "<html>\n";
    cout << "<head>\n";
    cout << "<title>Testing Post</title>\n";
    cout << "</head>\n";
    cout << "<body>\n";
    char* lenstr = getenv("CONTENT_LENGTH");

    if (lenstr == NULL) {
        cout << "Error, CONTENT_LENGTH should be entered!" << "<br/>";
    } else {
        int len = atoi(lenstr);
        char poststr[20];
        fgets(poststr, len + 1, stdin);
        cout << "poststr:" << poststr << "<br/>";
        char m[10], n[10];

        if (sscanf(poststr, "m=%[^&]&n=%s", m, n) != 2) {
            cout << "Error: Parameters are not right!<br/>";
        } else {
            cout << "m * n = " << atoi(m)*atoi(n) << "<br/>";
        }
    }

    cout << "</body>\n";
    cout << "</html>\n";
    return 0;
}
