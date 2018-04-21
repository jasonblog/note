# Google Protocol Buffers 学习笔记



Protocol Buffers（PB）是一个用于序列化结构化数据的机制，是谷歌的一个开源项目，在github上有源代码，也有发行版。PB跟XML相似，XML序列化的时候速度是可以接受的，但是XML解析数据的时候非常慢。然而Protocol Buffers则是非常轻量，速度也很快。


我们学习Protocol Buffers，直接通过实现一个Protocol Buffers小项目来实验，得出其源代码，再分析编码技术。

安装完Protocol Buffers之后（安装过程略去，我们选择的是Github上面的源代码c++版，而不是发行版，有利于我们学习编码技术），我们开始正式实验。

 

##关于PB的编码


由官方文档，PB先用variant编码整形数据，然后再进行操作

我们打开编译好的源代码我们找到了这样一条在序列化的时候所调用的函数


```cpp
if (has_sid()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->sid(), output);
}
```

使用gdb调试，尝试进入函数体内部，发现其调用：

src/google/protobuf/message.cc:174 的一个函数

bool Message::SerializeToOstream(ostream* output) const

最后找到了编码的函数：

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

在variant编码中第一个字节的高位msb为1表示下一个字节还有有效数据，msb为0表示该字节中的后7为是最后一组有效数字。踢掉最高位后的有效位组成真正的数字。

 

如数据value = 123456 运行上述程序的过程：

12345D = 3039H

target[0] = u8(3039H | 80H)       B9

30B9H > 80H

target[1] = 30B9H >> 7        60H

61H < 8000H

target[1] &= 0x7F   60H




结果应该是：B961H = 1011 1001 0110 0000 B


低位是的元数据位是1，表示后面还有8位的数据，依次下去

按照规则解析过来，刚好就是12345D

int32的编码

用上述方法使用gdb调试程序，得到函数入口

1、 计算长度1 + Int32Size(value);

```sh
(gdb) 

google::protobuf::io::CodedOutputStream::VarintSize32 (value=？)

at /usr/local/include/google/protobuf/io/coded_stream.h:1108
```

2、 调用这个函数，将值写入新的空间之中去

```sh
(gdb) step

google::protobuf::internal::WireFormatLite::Int32Size (value=？)

at /usr/local/include/google/protobuf/wire_format_lite_inl.h:797
```
存储的时候分为tag和value两块。tag部分由公式field_number << 3 | WITETYPE_VARIANT给出，

value部分则由用户给的值的variant编码构成


### String类型的编码

同样需要使用variant编码,string的要求是UTF8的编码的。

先计算计算长度公式 1 + variant(stringLength)+stringLength

编码后的格式为tag+length(variant int)+value

tage为公式field_number << 3 | WITETYPE_VARIANT给出，length则是长度的variant编码,value为用户字符串内容

函数：


```sh
google::protobuf::internal::WireFormat::VerifyUTF8StringFallback (

data=0x613118 "ye jiaqi", size=8, 

op=google::protobuf::internal::WireFormat::SERIALIZE, 

field_name=0x407dc8 "Lab.Student.name")

at google/protobuf/wire_format.cc:1089

1089                                           const char* field_name) {
```


下面进行实操：

 

定义一个Protocol Buffers


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

然后使用protoc将其编译为c++类


 写一个简单的脚本测试这个类
 
 
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

使用如下命令来进行编译，为了方便，定义一个makefile:


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

运行结果如下

然后我们用vim 的二进制方式打开存储的文件，分析其中部分的结构

开始的前两位OA是protobuf存储文件时候预定义的两位，结束时候也是

 

字符串：“ye jiaqi” : 08则是 由tag = 1 的预定义的name 和protobuf枚举类型中的2得到， 1 << 3 | 2 = 08 H, 后面的是utf8编码的字符串内容，可以被vim识别

 

INT32 “13331314”: 按照我们之前的编码方式

2 >> 3 | 0 = 10H

13331314（CD6B72H）使用variant方式编码得到的是F2D6AD06H, 跟我们的编码方式完全一致


##总结

protobuf是一个很好的轻量级的编码方式，比XML要好。然而json也是一种轻量级的数据传输协议。查了一下资料，json在许多报文方面还是不太行，protobuf不为一种很好的选择。可是protobuf文件的可读性约等于0，这个缺点实在有些。


