from os.path import join

from PIL import Image
import threading
import os
import sys
import time
sys.path.append(os.getcwd())


def getImagePath(FilesPath):
    """
    :param FilesPath: 給一個圖片資料夾的位置，將所有的檔案絕對路徑加入到Images_list_path中
    imagePathList = getImagePath(FilesPath=os.getcwd() + '/ImageFiles/')
    """
    Images_list_path = []
    for root, dirs, files in os.walk(FilesPath):
        for f in files:
            if f != '.DS_Store':  # 隱藏檔案不要加進來
                fullPath = join(root, f)
                Images_list_path.append(fullPath)
    return Images_list_path


class cropPictureFunction:
    def __init__(self, save_crop_picture_path, image_path_list, image_x, image_y,
                 crop_upper_proportion=1.80, crop_lower_proportion=1.62, isThread=False):

        self.save_crop_picture_path = save_crop_picture_path  # 儲存切格好的位置 str
        self.image_path_list = image_path_list  # 取得手機擷取圖片的全部絕對路徑位置 list
        self.image_x, self.image_y = image_x, image_y  # 取得 image 的x,y軸 int
        self.crop_upper_proportion = crop_upper_proportion  # 切格比例的上方 float
        self.crop_lower_proportion = crop_lower_proportion  # 切格比例的下方 float

        print('Image X軸：%s, Y軸: %s' % (self.image_x, self.image_y))
        print('切割圖片比例上方： %s, 下方: %s' % (self.crop_upper_proportion, self.crop_lower_proportion))
        print('切割圖片儲存位置:\n%s' % '\n'.join(self.image_path_list))
        print('切割圖片儲存位置: %s' % save_crop_picture_path)
        print("*" * 10)
        cropPictureFunction.cropPicture(self, isThread)

    def cropPicture(self, isThread):
        if isThread is False:
            for image_path in self.image_path_list:
                img = Image.open(image_path)
                cropped = img.crop((0, self.image_y / self.crop_upper_proportion,
                                    self.image_x, self.image_y / self.crop_lower_proportion))  # (left, upper, right, lower) 左、上、右、下
                cropped.save(self.save_crop_picture_path + os.path.basename(image_path))  # 儲存在你的指定位置，檔案名稱相同
                print('cropPicturePath: %s' % self.save_crop_picture_path + os.path.basename(image_path))
        else:
            thread_crop_picture_list = []
            for i in self.image_path_list:
                t = threading.Thread(target = self.useThreadCropPicture, args = (i,))
                thread_crop_picture_list.append(t)
            for t in thread_crop_picture_list:
                t.start()
            for t in thread_crop_picture_list:
                t.join()

    def useThreadCropPicture(self, image_path):
        img = Image.open(image_path)
        cropped = img.crop((0, self.image_y / self.crop_upper_proportion,self.image_x, self.image_y / self.crop_lower_proportion))  # (left, upper, right, lower) 左、上、右、下
        cropped.save(self.save_crop_picture_path + os.path.basename(image_path))  # 儲存在你的指定位置，檔案名稱相同
        print('cropPicturePath: %s' % self.save_crop_picture_path + os.path.basename(image_path))


if __name__ == '__main__':
    start = time.time()

    imagePathList = getImagePath(FilesPath=os.getcwd() + '/ImageFiles/')
    # 裁剪圖片功能
    cropPictureFunction(save_crop_picture_path=os.getcwd() + '/cropPictureFiles/',
                        image_path_list=imagePathList, image_x=2000, image_y=510,
                        crop_upper_proportion=1.6, crop_lower_proportion=1.05, isThread=False)
    end = time.time()
    print("用時{}秒".format((end-start)))


