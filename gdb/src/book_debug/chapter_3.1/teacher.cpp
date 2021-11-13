#include "teacher.h"
using namespace std;
Teacher::Teacher(const char* Name,int ClassNumber)
{
	name = Name;
	class_number = ClassNumber;
}
const char* Teacher::Name()
{
	return name.c_str();
}
int Teacher::classNumber()
{
	return class_number;
}
