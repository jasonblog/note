import os
import json


class JsonToDict_Function:

    def __init__(self):
        pass

    def PATH(self, p):
        return os.path.abspath(os.path.join(os.path.dirname(__file__), p))

    def json_parser(self, raw_data):
        # json -> dict of python
        json_data = json.loads(raw_data)
        return json_data

    def getJsonToDictData(self):

        # 取得 json
        read_package = open(str(JsonToDict_Function.PATH(self, './Test.json')), "r")
        raw_data = read_package.read()
        read_package.close()

        # 將 json to dict
        package_dict = self.json_parser(raw_data)
        print("內容: %s" % package_dict)
        print("Type: %s" % type(package_dict))

        return package_dict


if __name__ == "__main__":
    jsonToDict = JsonToDict_Function()
    jsonToDict.getJsonToDictData()
