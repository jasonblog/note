#ifndef design_patterns_builder_product_h
#define design_patterns_builder_product_h

#include <string>
#include <vector>
using std::string;
using std::vector;

class People
{
public:
    People(): parts_{}, name_{"x-man"} {}
    string name() const
    {
        return name_;
    }
    void set_name(const string& name)
    {
        name_ = name;
    }
    void AddPart(const string& part)
    {
        parts_.push_back(part);
    }
    void Show()
    {
        printf("%s has\n", name_.c_str());

        for (auto part : parts_) {
            printf("\t%s\n", part.c_str());
        }
    }

private:
    vector<string> parts_;
    string name_;
};

#endif
