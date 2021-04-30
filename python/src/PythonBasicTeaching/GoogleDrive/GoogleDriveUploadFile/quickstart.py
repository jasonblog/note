from __future__ import print_function
import os
import io
import time

from googleapiclient.http import MediaFileUpload, MediaIoBaseDownload
from apiclient import discovery
import httplib2
from oauth2client import file, client, tools

try:
    import argparse
    flags = argparse.ArgumentParser(parents=[tools.argparser]).parse_args()
except ImportError:
    flags = None


# If modifying these scopes, delete your previously saved credentials
# at ~/.credentials/drive-python-quickstart.json
SCOPES = 'https://www.googleapis.com/auth/drive'
CLIENT_SECRET_FILE = 'ncku_client_secret.json'
APPLICATION_NAME = 'Drive API Python Quickstart'


def get_credentials():
    """Gets valid user credentials from storage.

    If nothing has been stored, or if the stored credentials are invalid,
    the OAuth2 flow is completed to obtain the new credentials.

    Returns:
        Credentials, the obtained credential.
    """
    home_dir = os.path.expanduser('~')
    credential_dir = os.path.join(home_dir, '.credentials')
    if not os.path.exists(credential_dir):
        os.makedirs(credential_dir)
    credential_path = os.path.join(credential_dir,
                                   'drive-python-quickstart.json')

    store = file.Storage(credential_path)
    credentials = store.get()
    if not credentials or credentials.invalid:
        flow = client.flow_from_clientsecrets(CLIENT_SECRET_FILE, SCOPES)
        flow.user_agent = APPLICATION_NAME
        if flags:
            credentials = tools.run_flow(flow, store, flags)
        else: # Needed only for compatibility with Python 2.6
            credentials = tools.run(flow, store)
        print('Storing credentials to ' + credential_path)
    return credentials


# 權限必須
SCOPES = ['https://spreadsheets.google.com/feeds', 'https://www.googleapis.com/auth/drive']

def delete_drive_service_file(service, file_id):
    service.files().delete(fileId=file_id).execute()

def update_file(service, update_drive_service_name, local_file_path):
    """

    將本地端的檔案傳到雲端上

    :param service: 認證用
    :param update_drive_service_name: 存到 雲端上的名稱
    :param local_file_path: 本地端的位置
    :param local_file_name: 本地端的檔案名稱
    """

    print("正在上傳檔案...")
    file_metadata = {'name': update_drive_service_name}
    media = MediaFileUpload(local_file_path, )
    file_metadata_size = media.size()
    start = time.time()
    file_id = service.files().create(body=file_metadata, media_body=media, fields='id').execute()
    end = time.time()
    print("上傳檔案成功！")
    print('雲端檔案名稱為: ' + str(file_metadata['name']))
    print('雲端檔案ID為: ' + str(file_id['id']))
    print('檔案大小為: ' + str(file_metadata_size) + ' byte')
    print("上傳時間為: " + str(end-start))

    return file_metadata['name'], file_id['id']



def search_file(service, update_drive_service_name, is_delete_search_file=False):
    """
    本地端
    取得到雲端名稱，可透過下載時，取得file id 下載

    :param service: 認證用
    :param update_drive_service_name: 要上傳到雲端的名稱
    :param is_delete_search_file: 判斷是否需要刪除這個檔案名稱
    :return:
    """
    # Call the Drive v3 API
    results = service.files().list(fields="nextPageToken, files(id, name)", spaces='drive',
                                   q="name = '" + update_drive_service_name + "' and trashed = false",
                                   ).execute()
    items = results.get('files', [])
    if not items:
        print('沒有發現你要找尋的 ' + update_drive_service_name + ' 檔案.')
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


def trashed_file(service, is_delete_trashed_file=False):
    """

    抓取到雲端上垃圾桶內的全部檔案，進行刪除

    :param service: 認證用
    :param is_delete_trashed_file: 是否要刪除垃圾桶資料
    :return:
    """
    results = service.files().list(fields="nextPageToken, files(id, name)", spaces='drive', q="trashed = true",
                                   ).execute()
    items = results.get('files', [])
    if not items:
        print('垃圾桶無任何資料.')
    else:
        print('垃圾桶檔案: ')
        for item in items:
            print(u'{0} ({1})'.format(item['name'], item['id']))
            if is_delete_trashed_file is True:
                print("刪除檔案為:" + u'{0} ({1})'.format(item['name'], item['id']))
                delete_drive_service_file(service, file_id=item['id'])


def main(is_update_file_function=False, update_drive_service_name=None, update_file_path=None):
    """
    :param is_update_file_function: 判斷是否執行上傳的功能
    :param update_drive_service_name: 要上傳到雲端上的檔案名稱
    :param update_file_path: 要上傳檔案的位置以及名稱
    """

    print("is_update_file_function")
    print(type(is_update_file_function))
    print(is_update_file_function)

    #store = file.Storage('token.json')
    #creds = store.get()
    #if not creds or creds.invalid:
        #flow = client.flow_from_clientsecrets('credentials.json', SCOPES)
        #creds = tools.run_flow(flow, store)
    #service = build('drive', 'v3', http=creds.authorize(Http()))
    #service = build('drive', 'v3', http=)

    credentials = get_credentials()
    http = credentials.authorize(httplib2.Http())
    service = discovery.build('drive', 'v3', http=http)
    print('*' * 10)

    if is_update_file_function is True:
        print(update_file_path + update_drive_service_name)
        print("=====執行上傳檔案=====")
        # 清空 雲端垃圾桶檔案
        # trashed_file(service=service, is_delete_trashed_file=True)

        # 搜尋要上傳的檔案名稱是否有在雲端上並且刪除
        search_file(service=service, update_drive_service_name=update_drive_service_name,
                    is_delete_search_file=True)
        # 檔案上傳到雲端上
        update_file(service=service, update_drive_service_name=update_drive_service_name,
                    local_file_path=os.getcwd() + '/' + update_drive_service_name)
        print("=====上傳檔案完成=====")


if __name__ == '__main__':
    main(is_update_file_function=bool(True), update_drive_service_name='aaa.txt', update_file_path=os.getcwd() + '/')
