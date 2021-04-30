import json
import os
from datetime import datetime
# 範例的字典檔案
example_dict = {
    "123": "tttt",
    "1123": ["123", "fffff"],
    "fffff": "awinni12344%%^^",
    "kkkkffff": "你好啊！！"
}


def main(save_json_path, save_json_name, dataDict):
    """
    :param save_json_path: 你要儲存json的位置
    :param save_json_name: 你要將字典存入到json的檔案名稱叫做什麼
    :param dataDict: dict資料
    """
    now_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    with open(save_json_path + save_json_name + ".json", 'w') as json_file:
        json.dump(dataDict, json_file)
    print("[%s] Json 檔案儲存至: %s" % (now_time, save_json_path + save_json_name + ".json"))


if __name__ == '__main__':
    main(save_json_path=os.getcwd() + '/',  save_json_name="test", dataDict=example_dict)
