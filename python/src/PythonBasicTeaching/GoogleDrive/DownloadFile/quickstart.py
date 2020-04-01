from __future__ import print_function
import os
import io
import time
from googleapiclient.discovery import build
from googleapiclient.http import MediaFileUpload, MediaIoBaseDownload
from httplib2 import Http
from oauth2client import file, client, tools

# 權限必須
SCOPES = ['https://spreadsheets.google.com/feeds', 'https://www.googleapis.com/auth/drive']


def delete_drive_service_file(service, file_id):
    service.files().delete(fileId=file_id).execute()


def download_file(service, file_id, download_file_path, download_drive_service_name):
    """
    雲端將資料下載到本地端用

    :param service: 認證用
    :param file_id: 雲端檔案的id 可透過 update_file()取得
    :param download_file_path: 將雲端上的資料下載到本地端的位置
    :param download_file_name: 下載到本地端的名稱
    :return:
    """
    if file_id is not None:
        request = service.files().get_media(fileId=file_id)
        local_download_path = download_file_path + download_drive_service_name
        fh = io.FileIO(local_download_path, 'wb')
        downloader = MediaIoBaseDownload(fh, request)
        print("下載檔案中....")
        done = False
        while done is False:
            status, done = downloader.next_chunk()
            print("Download %d%%." % int(status.progress() * 100))
        print("下載完成")
        print("下載檔案位置為: ", str(download_file_path + download_drive_service_name))
        print("=====下載檔案完成=====")
    else:
        print("=====下載檔案失敗，未找到檔案=====")


def search_file(service, download_drive_service_name, is_delete_search_file=False):
    """
    本地端
    取得到雲端名稱，可透過下載時，取得file id 下載

    :param service: 認證用
    :param download_drive_service_name: 要上傳到雲端的名稱
    :param is_delete_search_file: 判斷是否需要刪除這個檔案名稱
    :return:
    """
    # Call the Drive v3 API
    results = service.files().list(fields="nextPageToken, files(id, name)", spaces='drive',
                                   q="name = '" + download_drive_service_name + "' and trashed = false",
                                   ).execute()
    items = results.get('files', [])
    if not items:
        print('雲端上，沒有發現你要找尋的 ' + download_drive_service_name + ' 檔案.')
        return None
    else:
        print('搜尋的檔案: ')
        for item in items:
            times = 1
            print(u'{0} ({1})'.format(item['name'], item['id']))
            if is_delete_search_file is True:
                print("刪除檔案為:" + u'{0} ({1})'.format(item['name'], item['id']))
                delete_drive_service_file(service, file_id=item['id'])

            if times == len(items):
                return item['id']
            else:
                times += 1


def main(is_download_file_function=False,
         download_drive_service_name=None,
         download_file_path=None):
    """
    :param is_download_file_function: 判斷是否執行下載的功能
    :param download_drive_service_name: 要上傳到雲端上的檔案名稱
    :param download_file_path: 要下載的位置以及名稱
    :return:
    """

    print("is_download_file_function: %s " % is_download_file_function)
    store = file.Storage('token.json')
    creds = store.get()
    if not creds or creds.invalid:
        flow = client.flow_from_clientsecrets('credentials.json', SCOPES)
        creds = tools.run_flow(flow, store)
    service = build('drive', 'v3', http=creds.authorize(Http()))
    print('*' * 10)

    # 本地端 執行部分
    if is_download_file_function is True:
        print("=====執行下載檔案=====")
        # 搜尋上傳的檔案名稱 本地端執行部分
        drive_service_file_id = search_file(service=service, download_drive_service_name=download_drive_service_name)
        download_file(service=service, file_id=drive_service_file_id, download_file_path=download_file_path, download_drive_service_name=download_drive_service_name)


if __name__ == '__main__':

    main(is_download_file_function=bool(True), download_drive_service_name='aaa.txt', download_file_path=os.getcwd() + '/')



