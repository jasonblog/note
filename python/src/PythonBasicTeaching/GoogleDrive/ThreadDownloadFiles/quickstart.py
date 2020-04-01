from __future__ import print_function
import os
import io
import time
from googleapiclient.discovery import build
from googleapiclient.http import MediaFileUpload, MediaIoBaseDownload
from httplib2 import Http
from oauth2client import file, client, tools
import threading

# 權限必須
SCOPES = ['https://spreadsheets.google.com/feeds', 'https://www.googleapis.com/auth/drive']

getDownloadFileIdList = []
getDownloadFileNameList = []
getDownloadFileMimeTypeList = []

googleFormatList = []
changeGoogleFormat = []


def authorize_api():
    store = file.Storage('token.json')
    creds = store.get()
    if not creds or creds.invalid:
        flow = client.flow_from_clientsecrets('GoogleDriveAPI.json', SCOPES)
        creds = tools.run_flow(flow, store)
    return build('drive', 'v3', http=creds.authorize(Http()))


def delete_drive_service_file(service, file_id):
    service.files().delete(fileId=file_id).execute()


def getGoogleDocFormatDict():
    """
    如果有透過Google Drive建立的檔案，如試算表, 表格, 文件, 投影片 下載時會去做轉換
    可以參考下方官方連結：
    https://developers.google.com/drive/api/v3/ref-export-formats
    主要看你想要轉成什麼可以從這裡做修改
    記得一個參數都不能少喔！最後一個是要有一個.XXX
    """
    googleDocFormat = {
        "application/vnd.google-apps.spreadsheet": ["application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", ".xlsx"],
        "application/vnd.google-apps.presentation": ["application/vnd.openxmlformats-officedocument.presentationml.presentation", ".ppt"],
        # "application/vnd.google-apps.document": ["application/vnd.openxmlformats-officedocument.wordprocessingml.document", ".doc"],
        "application/vnd.google-apps.document": ["application/pdf", ".pdf"],
        "application/vnd.google-apps.drawing": ["image/png", ".png"],
        "application/vnd.google-apps.form": ["application/pdf", ".pdf"]
    }
    return googleDocFormat


def search_folder(service, drive_service_folder_name=None):
    """
    如果雲端資料夾名稱相同，則只會選擇一個資料夾，請勿取名相同名稱
    :param service: 認證用
    :param drive_service_folder_name: 取得指定資料夾的id，沒有的話回傳None，給錯也會回傳None
    """
    if drive_service_folder_name is not None:  # 如果有給參數的話就會跑這裡，否則就直接回傳None
        response = service.files().list(fields="nextPageToken, files(id, name)", spaces='drive',  # 搜尋雲端上的資料夾是否有符合
                                       q = "name = '" + drive_service_folder_name + "' and mimeType = 'application/vnd.google-apps.folder' and trashed = false").execute()
        for file in response.get('files', []):  # 搜尋雲端上符合的資料夾名稱，有就會回傳id，沒有的話依樣回傳None
            print('雲端資料夾: %s (%s)' % (file.get('name'), file.get('id')))
            return file.get('id')
    return None


def download_folder_all_files(key, value, download_file_path, google_doc_format_dict):
    """
    :param key: 每個檔案的id
    :param value: value[0]為檔案名稱, value[1]為檔案類型
    :param download_file_path: 你要下載到哪個位置
    :param google_doc_format_dict: 取得google drive上的檔案格式
    """
    drive_service = authorize_api()
    if str(value[1]) in google_doc_format_dict.keys():  # 判斷說 是否是Google上所建立的檔案，是的話用這個方法上傳
        if str(value[1]) != 'application/vnd.google-apps.form':  # 目前不支援表格，不曉得可以轉成什麼格式就先跳過不下載
            request = drive_service.files().export_media(fileId=key, mimeType=google_doc_format_dict[value[1]][0])  # fileId是檔案id, mimeTpye是下載轉成的檔案格式
        local_download_path = download_file_path + str(value[0]) + google_doc_format_dict[value[1]][1]  # 下載位置，最後一個是副檔名，因為google drive 好像沒提供，因此寫一個dict紀錄
    else:
        request = drive_service.files().get_media(fileId=key)  # 單純下載之前上傳的檔案，只要給一個fileId就可以
        local_download_path = download_file_path + str(value[0])  # 下載檔案到你的本地端的檔案會與雲端上名稱相同

    fh = io.FileIO(local_download_path, 'wb')  # 指定下載檔案位置寫入
    downloader = MediaIoBaseDownload(fh, request)  # 進行下載檔案
    print("下載檔案中....")
    done = False
    while done is False:
        status, done = downloader.next_chunk()
        print("Download %d%%." % int(status.progress() * 100))
    print("=====下載檔案 %s 完成=====" % str(value[0]))


def download_folder_files(service, file_id_list, file_name_list, file_type_list, download_file_path, google_doc_format_dict):

    """
    :param service: 認證用
    :param file_id_list: 取得檔案的id list
    :param file_name_list: 取得檔案的 name list
    :param file_type_list: 取得檔案的 type
    :param download_file_path: 你要下載到哪個位置
    :param google_doc_format_dict: 取得google drive上的檔案格式
    """
    download_file_path_list = []  # 紀錄下載多少個檔案跟檔案路徑


    # 我們將剛剛的三個list轉成 dict 來做操作下面的流程
    downloadFileDict = dict(zip(file_id_list, zip(file_name_list, file_type_list)))  # key 為 id, value 為 name, type

    if downloadFileDict == {}:
        print('你所提供的檔案名稱不存在或是輸入有錯，記得要加上副檔名')
    else:
        print('下載的檔案名稱以及Id: %s' % str(downloadFileDict))
        if file_id_list is not None:
            for key, value in downloadFileDict.items():  # key 就是檔案的id, file_name_list = value[0] 是檔案名稱, file_type_list = value[1] 是檔案Type
                if str(value[1]) in google_doc_format_dict.keys():  # 判斷說 是否是Google上所建立的檔案，是的話用這個方法上傳
                    if str(value[1]) != 'application/vnd.google-apps.form':  # 目前不支援表格，不曉得可以轉成什麼格式就先跳過不下載
                        request = service.files().export_media(fileId=key, mimeType=google_doc_format_dict[value[1]][0])  # fileId是檔案id, mimeTpye是下載轉成的檔案格式

                    local_download_path = download_file_path + str(value[0]) + google_doc_format_dict[value[1]][1]  # 下載位置，最後一個是副檔名，因為google drive 好像沒提供，因此寫一個dict紀錄
                else:
                    request = service.files().get_media(fileId=key)  # 單純下載之前上傳的檔案，只要給一個fileId就可以
                    local_download_path = download_file_path + str(value[0])  # 下載檔案到你的本地端的檔案會與雲端上名稱相同

                fh = io.FileIO(local_download_path, 'wb')  # 指定下載檔案位置寫入
                downloader = MediaIoBaseDownload(fh, request)  # 進行下載檔案
                print("下載檔案中....")
                done = False
                while done is False:
                    status, done = downloader.next_chunk()
                    print("Download %d%%." % int(status.progress() * 100))
                # print("下載檔案位置為: ", str(local_download_path))
                print("=====下載檔案 %s 完成=====" % str(value[0]))
                download_file_path_list.append(local_download_path)
        else:
            print("=====下載檔案失敗，未找到檔案=====")
        print('雲端檔案下載儲存在你本地端的位置:\n%s' % '\n'.join(download_file_path_list))
        print('檔案下載數量為: %s' % len(download_file_path_list))


def search_file(service, download_drive_service_name):
    """
    本地端 取得到雲端名稱，可透過下載時，取得file id 下載
    :param service: 認證用
    :param download_drive_service_name: 你要下載的檔案名稱
    :return:
    """
    # Call the Drive v3 API
    results = service.files().list(fields="nextPageToken, files(id, name)", spaces='drive',  # 搜尋你的檔案是否在雲端上，不包含垃圾桶資料
                                   q="name = '" + download_drive_service_name + "' and trashed = false",
                                   ).execute()
    items = results.get('files', [])  # 將取得到的檔案都放置在items
    if not items:  # 沒有符合你的檔案會會傳 None
        print('雲端上，沒有發現你要找尋的 ' + download_drive_service_name + ' 檔案.')
        return None
    else:  # 有的話，會回傳第一個找到的檔案id
        print('搜尋的檔案: ')
        for item in items:
            times = 1
            if times == len(items):
                print("檔案為:" + u'{0} ({1})'.format(item['name'], item['id']))
                return item['id']
            else:
                times += 1


def search_folder_files(service, download_drive_service_name, folder_id):
    """
    :param service: 認證用
    :param download_drive_service_name: 你要下載的檔案如果給None 就是下載資料夾內的檔案，但是不包含裡面的資料夾
    :param folder_id: 取得到的資料夾 id
    """
    # 進行雲端搜尋 取得 parents, id, name, mimeType的參數值
    # 篩選方式 不要資料夾以及不要垃圾桶的資料
    results = service.files().list(fields="nextPageToken, files(parents, id, name, mimeType)", spaces='drive',
                                   q="\'" + folder_id + "' in parents and trashed = false and mimeType != 'application/vnd.google-apps.folder'",
                                   ).execute()
    items = results.get('files', [])  # 篩選完的資料放置到items
    for item in items:
        # 因為有些檔案沒有提供 parents參數會導致崩潰，因此用try的方式進行
        try:
            getItemStr = ''.join(item['parents'])  # 取得每個檔案的資料夾 id 轉換成 str 判斷用～
            if getItemStr == folder_id:  # 查看每個資料夾id 是否符合我們之前找的資料夾id 有符合就可以找尋這個資料夾底下的參數了
                if download_drive_service_name is None:  # 如果沒有指定的話(None)，會抓資料夾底下的所有檔案，但不會抓取下一層的資料夾底下的檔案
                    getDownloadFileIdList.append(item['id'])  # 我們需要幾個參數 id, name ,mimeType
                    getDownloadFileNameList.append(item['name'])
                    getDownloadFileMimeTypeList.append(item['mimeType'])
                else:  # 如果你有給資料夾底下指定的檔案名稱會跑這裡
                    if item['name'] == download_drive_service_name:
                        getDownloadFileIdList.append(item['id'])  # 我們需要幾個參數 id, name ,mimeType
                        getDownloadFileNameList.append(item['name'])
                        getDownloadFileMimeTypeList.append(item['mimeType'])
        except Exception as e:
            pass


def download_file(service, file_id, download_file_path, download_drive_service_name):
    """
    雲端將資料下載到本地端用

    :param service: 認證用
    :param file_id: 雲端檔案的id 可透過 update_file()取得
    :param download_file_path: 將雲端上的資料下載到本地端的位置
    :param download_drive_service_name:
    """
    if file_id is not None:  # 如果有取得到檔案id就會跑這裡
        request = service.files().get_media(fileId=file_id)  # 取得檔案id
        local_download_path = download_file_path + download_drive_service_name  # 你存放在本地端的位置以及名稱
        fh = io.FileIO(local_download_path, 'wb')  # 指定要寫入的檔案位置以及名稱
        downloader = MediaIoBaseDownload(fh, request)  # 使用 下載的功能 Google Drive內建的
        print("下載檔案中....")
        done = False
        while done is False:
            status, done = downloader.next_chunk()
            print("Download %d%%." % int(status.progress() * 100))
        print("下載完成")
        print("下載檔案位置為: ", str(download_file_path + download_drive_service_name))
        print("=====下載檔案完成=====")
    else:  # 沒找到 id 會跑這裏
        print("=====下載檔案失敗，未找到檔案=====")


def main(is_download_file_function=False, download_drive_service_name=None,
         download_file_path=None, drive_service_folder_name=None, isThread=False):
    """
    :param is_download_file_function: 是否要開啟下載功能 預設為 False
    :param download_drive_service_name: 你要下載的檔案名稱，如果是選擇資料夾內檔案下載請給None,如果不是請給檔案名稱包含副檔名
                                        假設是Google上建立的檔案就不需要給副檔名
    :param download_file_path: 你要下載到本地端的位置
    :param drive_service_folder_name: 指定雲端上的資料夾名稱，如果沒有請給None，他會搜尋找個雲端上的檔案
                                    如果是在資料夾結構為test/test123/download/的話，你想指定 test123 資料夾
                                    參數可以這樣給 drive_service_folder_name='test123'
    """

    print("is_download_file_function: %s " % is_download_file_function)  # 確認是否使用 download 功能
    print("drive_service_folder_name: %s " % drive_service_folder_name)  # 選擇的資料夾名稱
    print("download_drive_service_name: %s" % download_drive_service_name)  # 選擇的下載名稱

    # 認證用
    drive_service = authorize_api()

    print('*' * 10)

    # 本地端 執行部分
    if is_download_file_function is True:
        print("=====執行下載檔案=====")

        get_folder_id = search_folder(service=drive_service, drive_service_folder_name = drive_service_folder_name)  # 取得資料夾的id，如果沒有會回傳None

        if get_folder_id is None:  # 判斷說 如果沒有資料夾時，會跑這塊搜尋找個雲端上的檔案 PS:這塊部分我沒有針對Google Drive上所建立的檔案去做下載動作，如果指定Google Drive的檔案會有問題
            drive_service_file_id = search_file(service=drive_service, download_drive_service_name=download_drive_service_name)  # 取得要下載
            download_file(service=drive_service, file_id=drive_service_file_id, download_file_path=download_file_path,
                          download_drive_service_name=download_drive_service_name)  # 下載你指定的檔案名稱到本地端
        else:  # 如果有指定資料夾會跑這塊，進行搜尋指定資料夾底下的檔案
            search_folder_files(service=drive_service, download_drive_service_name=download_drive_service_name,
                                folder_id=get_folder_id)  # 透過取得的資料夾id 找尋裡面的所有檔案id
            if isThread is True:  # 使用多進程
                downloadFileDict = dict(zip(getDownloadFileIdList, zip(getDownloadFileNameList, getDownloadFileMimeTypeList)))
                thread_download_drive_path_list = []

                for key, value in downloadFileDict.items():
                    t = threading.Thread(target=download_folder_all_files, args=(key, value, download_file_path, getGoogleDocFormatDict()))
                    thread_download_drive_path_list.append(t)
                for t in thread_download_drive_path_list:
                    t.start()
                for t in thread_download_drive_path_list:
                    t.join()
            else:
                download_folder_files(service=drive_service, file_id_list=getDownloadFileIdList, download_file_path=download_file_path,  # 執行下載的動作
                                      file_name_list=getDownloadFileNameList, file_type_list=getDownloadFileMimeTypeList, google_doc_format_dict=getGoogleDocFormatDict())


if __name__ == '__main__':
    # 花費時間 34.43345308303833秒 未使用 Thread 下載指定雲端資料夾內的所有檔案，14張圖片
    # 花費時間 14.741281747817993秒 使用 Thread 下載指定雲端資料夾內的所有檔案，14張圖片

    start = time.time()

    main(is_download_file_function=bool(True), drive_service_folder_name='美國簽證', download_drive_service_name=None,
         download_file_path=os.getcwd() + '/TestFolder/', isThread=True)

    end = time.time()
    print("用時{}秒".format((end-start)))
