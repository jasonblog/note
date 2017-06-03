#include<iostream>
#include<fstream>
#include "Mymessage.pb.h"
using namespace std;
void ListMsg(const Im::Content& msg)
{
    cout << msg.id() << endl;
    cout << msg.str() << endl;
}
int main(int argc, char* argv[])
{
    Im::Content msg1;
    fstream input("./log", ios::in | ios::binary);

    if (!msg1.ParseFromIstream(&input)) {
        cerr << "Failed to parse address book." << endl;
        return -1;
    }

    ListMsg(msg1);
    return 0;
}
