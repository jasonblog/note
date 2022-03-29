//student.h
#include <string>

class Student
{
public:
	Student(const char* Name,int Age);
	const char* Name();
	int Age();
private:
	std::string name;
	int age;
	
};
