//main.cpp
#include <iostream>
#include "student.h"
#include "teacher.h"
int main(int argc,char ** argv)
{
	Student stu("Mike",20);
	printf("Student's Name is %s,age is %d\n",stu.Name(),stu.Age());
	Teacher teacher("John",2);
	printf("Teacher's Name is %s,class number is %d\n",teacher.Name(),teacher.classNumber());
	return 0;
}
