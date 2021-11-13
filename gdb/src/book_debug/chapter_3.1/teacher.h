#include <string>
class Teacher
{
public:
	Teacher(const char* Name,int ClassNumber);

	virtual ~Teacher(){}
	const char*Name();
	int classNumber();
private:
	std::string name;
	int class_number;
};
