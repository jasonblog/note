# encoding: utf-8
from __future__ import print_function
import numpy as np
np.random.seed(1337)  # for reproducibility
from keras.utils import np_utils
from sklearn.preprocessing import LabelEncoder
import glob
import cv2
import re
import kerasInitModel

nb_classes = 10 # 要分類的群組數 0~9共十個分類
kernel_size = (3,3) # 模型濾波器大小
input_shape = (50,50,3) # 輸入圖片大小
batch_size = 128 # 每一批有幾個圖
nb_epoch = 12 # 訓練次數

# 取得圖片和標籤
def GetData(path):
    # 取得所有圖片path
    allfile = glob.glob(path + '\*.png')
    length = len(allfile) # 圖片數量
    data = np.empty((length, 50,50,3), dtype="float32") # 用來存放圖片
    labels = [] #用來存放解答

    # 取得訓練圖片和訓練標籤(answer)
    for index, img_path in enumerate(allfile):
        image = cv2.imread(img_path)
        m = re.search(r'(\d)_\d*.png', img_path)
        label = m.group(1) # 從檔名取出圖片的解答
        arr = np.asarray(image, dtype="float32") / 255.0 #將黑白圖片轉成1,0陣列 原本是0,255
        data[index, :, :, :] = arr
        labels.append(label)
    # 將標籤轉成integers
    le = LabelEncoder()
    labels = le.fit_transform(labels)
    # 將標籤轉成獨熱編碼 one hot encoding
    labels = np_utils.to_categorical(labels, nb_classes)
    return data,labels

# 取得訓練資料
trainData,trainLabels = GetData('D:\CaptchaSingle')
# 取得測試資料
testData,testLabels = GetData('D:\CaptchaTest')

# 載入keras model
model =kerasInitModel.LoadModel()

model.fit(trainData, trainLabels, batch_size=batch_size, nb_epoch=nb_epoch,
          verbose=1
          , validation_data=(testData, testLabels)
          )
model.save_weights('model/model.h5')
score = model.evaluate(testData, testLabels, verbose=0)
print('Test score:', score[0])
print('Test accuracy:', score[1])

