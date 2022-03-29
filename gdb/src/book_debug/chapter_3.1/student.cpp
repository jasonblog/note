//student.cpp
#include "student.h"
using namespace std;

Student::Student(const char* Name,int Age)
{
	name = Name;
	age = Age;
}
const char* Student::Name()
{
	return name.c_str();
}
int Student::Age()
{
	return age;
}
