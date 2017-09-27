# Std Pair 用法


```cpp
#include<iostream>
using namespace std;

int main()
{
    pair<string, int> p("favorite h-game", 9800);
    cout << "Chang's " << p.first << " costs " << p.second << " yen." << endl;
    return 0;
}
```