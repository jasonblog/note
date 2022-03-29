#pragma once
#include <string.h>
#include <assert.h>
#pragma warning (disable:4996)

char* strcpy(char* strDest, const char* strSrc)
{
	char* address = strDest;
	assert((strDest != NULL) && (strSrc != NULL));
	while ((*strDest++ = *strSrc++) != '\0')
		;
	return address;
}

class student
{
public:
	student(const char* name, int score)
	{
		_ASSERT(name != NULL);
		int len = strlen(name);
		_name = new char[len + 1];
		strcpy(_name, name);
		_score = score;
	}
	~student()
	{
		delete[]_name;
	}
	const char* name()
	{
		return _name;
	}
	int score()
	{
		return _score;
	}
private:
	char* _name;
	int _score;
};


