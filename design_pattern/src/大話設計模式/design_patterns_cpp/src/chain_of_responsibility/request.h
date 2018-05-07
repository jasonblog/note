#ifndef design_patterns_chain_of_responsiblity_request_h
#define design_patterns_chain_of_responsiblity_request_h

#include <string>
using std::string;

class Request
{
private:
    int type_;
    string content_;
public:
    Request(): type_{0}, content_{} {}
    Request(int type, const string& content)
        : type_{type}, content_{content} {}
    int type() const
    {
        return type_;
    }
    void set_type(int type)
    {
        type_ = type;
    }
    string content() const
    {
        return content_;
    }
    void  set_content(const string& content)
    {
        content_ = content;
    }
};

#endif
