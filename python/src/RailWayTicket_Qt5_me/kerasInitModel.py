# encoding: utf-8
from keras.models import Sequential
from keras.layers import Dense, Dropout, Activation, Flatten
from keras.layers import Convolution2D, MaxPooling2D
'''
初始化keras model

'''
def LoadModel():
    nb_classes = 10  # 要分類的群組數 0~9共十個分類
    kernel_size = (3, 3)  # 模型濾波器大小
    input_shape = (50, 50, 3)  # 輸入圖片大小
    batch_size = 128  # 每一批有幾個圖
    nb_epoch = 12  # 訓練次數
    model = Sequential()
    # 第一層卷積，filter大小3*3，數量32個，原始圖像大小50*50 輸出=48*48
    model.add(Convolution2D(32, kernel_size[0], kernel_size[1],
                            border_mode='valid',
                            input_shape=input_shape))
    model.add(Activation('relu'))
    # 第二層卷積，filter大小3*3，數量32個，輸入圖像大小（50-3+1）*（50-3-1） = 48*48 輸出=46*46
    model.add(Convolution2D(32, kernel_size[0], kernel_size[1]))
    model.add(Activation('relu'))
    # maxpooling，大小(2,2),輸入大小是46*46,stride默認是None，輸出大小是23*23
    model.add(MaxPooling2D(pool_size=(2, 2)))
    model.add(Dropout(0.25))
    # 第三層卷積，filter大小3*3，數量64個，輸入圖像大小23*23 輸出=21*21
    model.add(Convolution2D(64, kernel_size[0], kernel_size[1]))
    model.add(Activation('relu'))
    # 第四層卷積，filter大小3*3，數量64個，輸入圖像大小21*21，輸出是18*18
    model.add(Convolution2D(64, 4, 4))
    model.add(Activation('relu'))
    # maxpooling，大小(2,2),輸入大小是18*18,stride默認是None，輸出大小是9*9
    model.add(MaxPooling2D(pool_size=(3, 3)))
    model.add(Dropout(0.25))

    model.add(Flatten())
    model.add(Dense(512))
    model.add(Activation('relu'))
    model.add(Dropout(0.5))
    model.add(Dense(nb_classes))
    model.add(Activation('softmax'))

    model.compile(loss='categorical_crossentropy',
                  optimizer='adadelta',
                  metrics=['accuracy'])

    return model