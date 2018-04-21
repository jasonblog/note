# Google Protocol Buffers 學習筆記



Protocol Buffers（PB）是一個用於序列化結構化數據的機制，是谷歌的一個開源項目，在github上有源代碼，也有發行版。PB跟XML相似，XML序列化的時候速度是可以接受的，但是XML解析數據的時候非常慢。然而Protocol Buffers則是非常輕量，速度也很快。


我們學習Protocol Buffers，直接通過實現一個Protocol Buffers小項目來實驗，得出其源代碼，再分析編碼技術。

安裝完Protocol Buffers之後（安裝過程略去，我們選擇的是Github上面的源代碼c++版，而不是發行版，有利於我們學習編碼技術），我們開始正式實驗。

 

##關於PB的編碼


由官方文檔，PB先用variant編碼整形數據，然後再進行操作

我們打開編譯好的源代碼我們找到了這樣一條在序列化的時候所調用的函數


```cpp
if (has_sid()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->sid(), output);
}
```

使用gdb調試，嘗試進入函數體內部，發現其調用：

src/google/protobuf/message.cc:174 的一個函數

bool Message::SerializeToOstream(ostream* output) const

最後找到了編碼的函數：

src/google/protobuf/coded_stream.cc


```cpp
inline uint8* CodedOutputStream::WriteVarint32FallbackToArrayInline(
    uint32 value, uint8* target)
{
    target[0] = static_cast<uint8>(value | 0x80);

    if (value >= (1 << 7)) {
        target[1] = static_cast<uint8>((value >>  7) | 0x80);

        if (value >= (1 << 14)) {
            target[2] = static_cast<uint8>((value >> 14) | 0x80);

            if (value >= (1 << 21)) {
                target[3] = static_cast<uint8>((value >> 21) | 0x80);

                if (value >= (1 << 28)) {
                    target[4] = static_cast<uint8>(value >> 28);
                    return target + 5;
                } else {
                    target[3] &= 0x7F;
                    return target + 4;
                }
            } else {
                target[2] &= 0x7F;
                return target + 3;
            }
        } else {
            target[1] &= 0x7F;
            return target + 2;
        }
    } else {
        target[0] &= 0x7F;
        return target + 1;
    }
}
```

在variant編碼中第一個字節的高位msb為1表示下一個字節還有有效數據，msb為0表示該字節中的後7為是最後一組有效數字。踢掉最高位後的有效位組成真正的數字。

 

如數據value = 123456 運行上述程序的過程：

12345D = 3039H

target[0] = u8(3039H | 80H)       B9

30B9H > 80H

target[1] = 30B9H >> 7        60H

61H < 8000H

target[1] &= 0x7F   60H




結果應該是：B961H = 1011 1001 0110 0000 B


低位是的元數據位是1，表示後面還有8位的數據，依次下去

按照規則解析過來，剛好就是12345D

int32的編碼

用上述方法使用gdb調試程序，得到函數入口

1、 計算長度1 + Int32Size(value);

```sh
(gdb) 

google::protobuf::io::CodedOutputStream::VarintSize32 (value=？)

at /usr/local/include/google/protobuf/io/coded_stream.h:1108
```

2、 調用這個函數，將值寫入新的空間之中去

```sh
(gdb) step

google::protobuf::internal::WireFormatLite::Int32Size (value=？)

at /usr/local/include/google/protobuf/wire_format_lite_inl.h:797
```
存儲的時候分為tag和value兩塊。tag部分由公式field_number << 3 | WITETYPE_VARIANT給出，

value部分則由用戶給的值的variant編碼構成


### String類型的編碼

同樣需要使用variant編碼,string的要求是UTF8的編碼的。

先計算計算長度公式 1 + variant(stringLength)+stringLength

編碼後的格式為tag+length(variant int)+value

tage為公式field_number << 3 | WITETYPE_VARIANT給出，length則是長度的variant編碼,value為用戶字符串內容

函數：


```sh
google::protobuf::internal::WireFormat::VerifyUTF8StringFallback (

data=0x613118 "ye jiaqi", size=8, 

op=google::protobuf::internal::WireFormat::SERIALIZE, 

field_name=0x407dc8 "Lab.Student.name")

at google/protobuf/wire_format.cc:1089

1089                                           const char* field_name) {
```


下面進行實操：

 

定義一個Protocol Buffers


```sh
package Lab;
message Student{
required string name = 1;
required int32 sid = 2;
required int32 age = 3;
enum genderType {
FEMALE = 0;
MALE = 1;
}

required genderType gender = 4;
optional string phone = 5;
optional string email = 6;
}
```

然後使用protoc將其編譯為c++類


 寫一個簡單的腳本測試這個類
 
 
 ```cpp
 #include <iostream>
#include <fstream>
#include <string>
#include "student.pb.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::fstream;
using std::ios;

void setStudent(Lab::Student* student)
{
    cout << "Enter the student's name" << endl;;
    string name;
    getline(cin, name);
    student->set_name(name);

    cout << "Enter the student's id" << endl;
    int id;
    cin >> id;
    student->set_sid(id);

    cout << "Enter the student's age" << endl;
    int age;
    cin >> age;
    student->set_age(age);

    cout << "Enter the student's gender 'male' or 'female'" << endl;
    string gender;
    cin.ignore(256, '\n');
    getline(cin, gender);

    if (gender == "male") {
        student->set_gender(Lab::Student::MALE);
    } else if (gender == "female") {
        student->set_gender(Lab::Student::FEMALE);
    } else {
        cerr << "no such type set default(male)" << endl;
        student->set_gender(Lab::Student::MALE);
    }

    cout << "Enter the student's phone" << endl;
    string phone;
    getline(cin, phone);
    student->set_phone(phone);

    cout << "Enter the student's email" << endl;
    string email;
    getline(cin, email);
    student->set_email(email);

}

int main(int argc, char* argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (argc != 2) {
        cerr << "Usage:  " << argv[0] << " ADDRESS_BOOK_FILE" << endl;
        return -1;
    }

    // scope output
    {
        Lab::Student student;
        setStudent(&student);
        fstream output(argv[1], ios::out | ios::trunc | ios::binary);

        if (!student.SerializeToOstream(&output)) {
            cerr << "Failed to write address book." << endl;
            return -1;
        } else {
            string output;
            student.SerializeToString(&output);
            //cout << output << endl;
        }
    }

    // scope input
    {
        Lab::Student student;
        fstream input(argv[1], ios::in | ios::binary);

        if (!input) {
            cout << argv[1] << ": File not found.  Creating a new file." << endl;
        } else if (!student.ParseFromIstream(&input)) {
            cerr << "Failed to parse address book." << endl;
            return -1;
        } else {
            cout.setf(ios::left);
            cout.fill(' ');
            cout << "student's id: \t\t" << student.sid() << endl;
            cout << "student's name: \t" << student.name() << endl;
            cout << "student's age: \t\t" << student.age() << endl;
            cout << "student's gender: \t" << student.gender() << endl;
            cout << "student's phone: \t" << student.phone() << endl;
            cout << "student's email: \t" << student.email() << endl;
        }
    }

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
```

<info.input

ye jiaqi

13331314

20

male

18819473230

使用如下命令來進行編譯，為了方便，定義一個makefile:


```sh
main:proto student.pb.h test.cpp
g++ test.cpp student.pb.cc student.pb.h -lprotobuf -o student -W -g
./student test.txt <info.input
proto:student.proto
protoc -I=./ --cpp_out=./ ./student.proto
clean:
rm -f *.out *.h.gch
rm -f student
rm -f *.pb.h
rm -f *.pb.cc
rm -f *.txt
```

運行結果如下

然後我們用vim 的二進制方式打開存儲的文件，分析其中部分的結構

開始的前兩位OA是protobuf存儲文件時候預定義的兩位，結束時候也是

 

字符串：“ye jiaqi” : 08則是 由tag = 1 的預定義的name 和protobuf枚舉類型中的2得到， 1 << 3 | 2 = 08 H, 後面的是utf8編碼的字符串內容，可以被vim識別

 

INT32 “13331314”: 按照我們之前的編碼方式

2 >> 3 | 0 = 10H

13331314（CD6B72H）使用variant方式編碼得到的是F2D6AD06H, 跟我們的編碼方式完全一致


##總結

protobuf是一個很好的輕量級的編碼方式，比XML要好。然而json也是一種輕量級的數據傳輸協議。查了一下資料，json在許多報文方面還是不太行，protobuf不為一種很好的選擇。可是protobuf文件的可讀性約等於0，這個缺點實在有些。


