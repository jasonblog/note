#include <iostream>
#include <string>
#include "json/json.h"
using namespace std;
int main()
{
    Json::Value json_temp;
    json_temp["name"] = Json::Value("sharexu");
    json_temp["age"] = Json::Value(18);
    Json::Value root;
    root["key_string"] = Json::Value("value_string");
    root["key_number"] = Json::Value(12345);
    root["key_boolean"] = Json::Value(false);
    root["key_double"] = Json::Value(12.345);
    root["key_object"] = json_temp;
    root["key_array"].append("array_string");
    root["key_array"].append(1234);

    Json::FastWriter fast_writer;
    std::cout << fast_writer.write(root);

    Json::StyledWriter styled_writer;
    std::cout << styled_writer.write(root);

    string str_test = "{\"id\":1,\"name\":\"pacozhong\"}";
    Json::Reader reader;
    Json::Value value;

    if (!reader.parse(str_test, value)) {
        return 0;
    }

    string value_name = value["name"].asString();
    cout << value_name << endl;
    cout << value["name"];

    if (!value["id"].isInt()) {
        cout << "id is not int" << endl;
    } else {
        int value_id = value["id"].asInt();
        cout << value_id << endl;
    }

    return 0;
}
