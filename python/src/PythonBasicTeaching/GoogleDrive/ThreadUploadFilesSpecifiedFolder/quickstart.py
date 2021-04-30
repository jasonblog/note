from __future__ import print_function
import os
import io
import time
from os.path import join
import threading
from googleapiclient.discovery import build
from googleapiclient.http import MediaFileUpload, MediaIoBaseDownload
from httplib2 import Http
from oauth2client import file, client, tools

# 權限必須
SCOPES = ['https://spreadsheets.google.com/feeds', 'https://www.googleapis.com/auth/drive']


def authorize_api():
    store = file.Storage('token.json')
    creds = store.get()
    if not creds or creds.invalid:
        flow = client.flow_from_clientsecrets('GoogleDriveAPI.json', SCOPES)
        creds = tools.run_flow(flow, store)
    return build('drive', 'v3', http=creds.authorize(Http()))


def delete_drive_service_search_file(file_id):
    drive_service = authorize_api()
    drive_service.files().delete(fileId=file_id).execute()


def delete_drive_service_file(service, file_id):
    service.files().delete(fileId=file_id).execute()


def update_all_file(update_drive_service_name, local_file_path, update_drive_service_folder_id):
    """

    將本地端的檔案傳到雲端上

    :param update_drive_service_folder_id: 判斷是否有 Folder id 沒有的話，會上到雲端的目錄
    :param update_drive_service_name: 存到 雲端上的名稱
    :param local_file_path: 本地端的位置
    :param local_file_name: 本地端的檔案名稱
    """

    service = authorize_api()
    print("正在上傳檔案...")
    if update_drive_service_folder_id is None:
        file_metadata = {'name': update_drive_service_name}
    else:
        print("雲端資料夾id: %s" % update_drive_service_folder_id)
        file_metadata = {'name': update_drive_service_name,
                         'parents': update_drive_service_folder_id}

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


def update_file(service, update_drive_service_name, local_file_path, update_drive_service_folder_id):
    """

    將本地端的檔案傳到雲端上

    :param update_drive_service_folder_id: 判斷是否有 Folder id 沒有的話，會上到雲端的目錄
    :param service: 認證用
    :param update_drive_service_name: 存到 雲端上的名稱
    :param local_file_path: 本地端的位置
    :param local_file_name: 本地端的檔案名稱
    """
    print("正在上傳檔案...")
    if update_drive_service_folder_id is None:
        file_metadata = {'name': update_drive_service_name}
    else:
        print("雲端資料夾id: %s" % update_drive_service_folder_id)
        file_metadata = {'name': update_drive_service_name,
                         'parents': update_drive_service_folder_id}

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


def search_folder(service, update_drive_folder_name=None):
    """
    如果雲端資料夾名稱相同，則只會選擇一個資料夾上傳，請勿取名相同名稱
    :param service: 認證用
    :param update_drive_folder_name: 取得指定資料夾的id，沒有的話回傳None，給錯也會回傳None
    :return:
    """
    get_folder_id_list = []
    print(len(get_folder_id_list))
    if update_drive_folder_name is not None:
        response = service.files().list(fields="nextPageToken, files(id, name)", spaces='drive',
                                       q = "name = '" + update_drive_folder_name + "' and mimeType = 'application/vnd.google-apps.folder' and trashed = false").execute()
        for file in response.get('files', []):
            # Process change
            print('Found folder: %s (%s)' % (file.get('name'), file.get('id')))
            get_folder_id_list.append(file.get('id'))
        if len(get_folder_id_list) == 0:
            print("你給的資料夾名稱沒有在你的雲端上！，因此檔案會上傳至雲端根目錄")
            return None
        else:
            return get_folder_id_list
    return None


def search_all_file(service, update_drive_service_name_list, folder_id):
    """
    本地端
    取得到雲端名稱，可透過下載時，取得file id 下載

    :param service: 認證用
    :param update_drive_service_name_list: 要上傳到雲端的名稱
    :return:
    """
    # Call the Drive v3 API
    try:
        results = service.files().list(fields="nextPageToken, files(parents, id, name)", spaces='drive',
                                       q="\'" + folder_id[0] + "' in parents and mimeType != 'application/vnd.google-apps.folder' and trashed = false",
                                       ).execute()
    except Exception as e:
        print("沒給雲端資料夾名稱以及沒有提供要上傳的檔案，因此將上傳的檔案位置通通上傳到根目錄:%s" % e)
        results = service.files().list(fields="nextPageToken, files(parents, id, name)", spaces='drive',
                                       q="mimeType != 'application/vnd.google-apps.folder' and trashed = false",
                                       ).execute()
    items = results.get('files', [])  # 篩選完的資料放置到items
    # print(items)
    get_drive_file_id_list = []
    for item in items:
        # print(u'{0} ({1})'.format(item['name'], item['id']))
        for updateServiceName in update_drive_service_name_list:
            if updateServiceName == item['name']:
                print(u'Found file: {0} ({1})'.format(item['name'], item['id']))
                get_drive_file_id_list.append(item['id'])
    return get_drive_file_id_list


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


def get_update_files_path_list(update_files_path):
    """
    將上傳檔案的資料夾內的路徑以及名稱，全部放到list內
    :param update_files_path: 要上傳檔案的資料夾
    """
    UploadFilesPathList = []
    UploadFilesNameList = []
    for root, dirs, files in os.walk(update_files_path):
        for f in files:
            fullPath = join(root, f)
            UploadFilesPathList.append(fullPath)
            UploadFilesNameList.append(f)
    print("取得要上傳的所有檔案路徑: \n%s" % '\n'.join(UploadFilesPathList))
    return UploadFilesNameList, UploadFilesPathList


def main(is_update_file_function=False, update_drive_service_folder_name=None,
         update_drive_service_name=None, update_file_path=None, isThread=False):
    """
    :param isThread: 是否使用多進程
    :param update_drive_service_folder_name: 給要上傳檔案到雲端的資料夾名稱，預設則是上傳至雲端目錄
    :param is_update_file_function: 判斷是否執行上傳的功能
    :param update_drive_service_name: 要上傳到雲端上的檔案名稱
    :param update_file_path: 要上傳檔案的位置以及名稱
    """

    print("is_update_file_function: %s" % is_update_file_function)
    print("update_drive_service_folder_name: %s" % update_drive_service_folder_name)

    drive_service = authorize_api()
    print('*' * 10)

    if is_update_file_function is True:
        if update_drive_service_name is None:  # 上傳資料夾內的所有檔案會跑這裡
            print("上傳資料夾內，所有檔案")
            UploadFilesName, UploadFilesPath = get_update_files_path_list(update_files_path = update_file_path)
            print("=====執行上傳檔案=====")
            get_folder_id = search_folder(service=drive_service, update_drive_folder_name=update_drive_service_folder_name)
            get_drive_service_name_list = search_all_file(service=drive_service, update_drive_service_name_list=UploadFilesName, folder_id=get_folder_id)

            if isThread is True:
                # 刪除雲端上有符合要上傳檔案名稱 Thread
                thread_delete_drive_name_list = []
                for i in get_drive_service_name_list:
                    print('file id:%s ' % i)
                    t = threading.Thread(target=delete_drive_service_search_file, args=(i,))
                    thread_delete_drive_name_list.append(t)
                for t in thread_delete_drive_name_list:
                    t.start()
                for t in thread_delete_drive_name_list:
                    t.join()

                # 檔案上傳到雲端上 Thread
                thread_update_drive_path_list = []
                for i in range(len(UploadFilesPath)):
                    t = threading.Thread(target=update_all_file, args=(UploadFilesName[i], UploadFilesPath[i], get_folder_id))
                    thread_update_drive_path_list.append(t)
                for t in thread_update_drive_path_list:
                    t.start()
                for t in thread_update_drive_path_list:
                    t.join()
            else:
                #  搜尋要上傳的檔案名稱是否有在雲端上並且刪除
                for UploadFileName in UploadFilesName:
                    search_file(service = drive_service, update_drive_service_name = UploadFileName, is_delete_search_file = True)

                # 檔案上傳到雲端上
                for i in range(len(UploadFilesPath)):
                    update_file(service=drive_service, update_drive_service_name=UploadFilesName[i],
                                local_file_path=UploadFilesPath[i], update_drive_service_folder_id=get_folder_id)
            print("=====上傳檔案完成=====")

        else:  # 單純上傳一個檔案會跑這裡
            print(update_file_path + update_drive_service_name)
            print("=====執行上傳檔案=====")
            # 清空 雲端垃圾桶檔案
            # trashed_file(service=service, is_delete_trashed_file=True)
            get_folder_id = search_folder(service = drive_service, update_drive_folder_name = update_drive_service_folder_name)
            # 搜尋要上傳的檔案名稱是否有在雲端上並且刪除
            search_file(service=drive_service, update_drive_service_name=update_drive_service_name, is_delete_search_file=True)
            # 檔案上傳到雲端上
            update_file(service=drive_service, update_drive_service_name=update_drive_service_name,
                        local_file_path=update_file_path + update_drive_service_name, update_drive_service_folder_id=get_folder_id)
            print("=====上傳檔案完成=====")


if __name__ == '__main__':
    # 花費時間 75.77654194831848秒 未使用 Thread 1.先刪除雲端上與要上傳相同名稱的id -> 2.再上傳檔案上去
    # 花費時間 28.011533975601196秒 使用 Thread 1.先刪除雲端上與要上傳相同名稱的id -> 2.再上傳檔案上去
    start = time.time()
    main(is_update_file_function=bool(True), update_drive_service_folder_name='TestAPI',
         update_drive_service_name=None, update_file_path=os.getcwd() + '/UploadFiles/', isThread=True)
    end = time.time()
    print("用時{}秒".format((end-start)))

