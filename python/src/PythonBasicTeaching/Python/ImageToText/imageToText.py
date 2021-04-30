from os.path import join
import cropPicture
import cv2
import pytesseract
import os


def getImagePath(FilesPath):
    Images_list_path = []
    for root, dirs, files in os.walk(FilesPath):
        for f in files:
            if f != '.DS_Store':
                fullpath = join(root, f)
                Images_list_path.append(fullpath)

    print("IMG Path: %s" % str(Images_list_path))
    return Images_list_path


class ImageToTextManager:

    def __init__(self, cropPictureFolderPath):
        self.imagePathList = []  # 存放 圖片絕對路徑的 list
        self.cv2ReadImageList = []  # 圖片轉成 cv2的 灰階色 list
        self.getImagesPath(cropPictureFolderPath)  # 取得 圖片的絕對位置，放到List內
        self.getCv2ReadImageToGray()  # 將每個圖片轉成 Cv2 加 灰階色
        self.getImageToTextList()  # Cv2灰階圖 轉成 文字

    def getImagesPath(self, imageFilesPath):
        for root, dirs, files in os.walk(imageFilesPath):
            for f in files:
                if f != '.DS_Store':
                    fullPath = join(root, f)
                    self.imagePathList.append(fullPath)
        print("IMG Path: %s" % str(self.imagePathList))

    def getCv2ReadImageToGray(self):
        for i in self.imagePathList:
            img = cv2.imread(i, 0)
            ret, thresh1 = cv2.threshold(img, 127, 255, cv2.THRESH_BINARY)
            cv2.imwrite(os.getcwd() + '/test_image_File/' + os.path.basename(i), thresh1)
            self.cv2ReadImageList.append(thresh1)

    def getImageToTextList(self):
        for i in range(len(self.cv2ReadImageList)):
            tessdata_dir_config = r'--psm 7 , --pdi 70, --oem 1'
            # tessdata_dir_config = r'--psm 11 , --pdi 70, sfz'  # 準確率有提高 搭配 sfz
            # /usr/local/Cellar/tesseract/4.1.1/share/tessdata/configs
            text1 = pytesseract.image_to_string(self.cv2ReadImageList[i], config=tessdata_dir_config)
            print("*" * 10)
            print(self.imagePathList[i] + '\n' + text1)


if __name__ == '__main__':
    imagePathList = getImagePath(FilesPath=os.getcwd() + '/ImageFiles_backup/')

    # 裁剪圖片功能
    cropPicture.cropPictureFunction(save_crop_picture_path=os.getcwd() + '/cropPictureFiles/',
                                    image_path_list=imagePathList, image_x=1440, image_y=2560,
                                    crop_upper_proportion=1.78, crop_lower_proportion=1.64)

    ImageToTextManager = ImageToTextManager(cropPictureFolderPath=os.getcwd() + '/cropPictureFiles/')

