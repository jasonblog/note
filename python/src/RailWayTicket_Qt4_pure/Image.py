# encoding: utf-8
import cv2
from matplotlib import gridspec
from matplotlib.font_manager import FontProperties
import collections
import sys
import requests
import numpy as np
import operator
import matplotlib
matplotlib.use('Qt4Agg')
import matplotlib.pyplot as plt
import math

reload(sys)
sys.setdefaultencoding("utf-8")


class Image:

    '''
    如果type=url
    getimg參數要傳入requests回傳的content
    如果type=local
    getimg參數要傳入圖片本地路徑
    '''
    def __init__(self, getimg, type='url'):
        #  設置 matplotlib 中文字體
        self.font = FontProperties(fname=r"/usr/share/fonts/truetype/arphic/uming.ttc", size=14)
        #  儲存檔名
        # self.imageName = ImgName
        # #  儲存路徑
        # self.Path = Path
        #  用來儲放分割後的圖片邊緣坐標(x,y,w,h)
        self.arr = []
        #  將每個階段做的圖存起來 用來debug
        self.dicImg = collections.OrderedDict()
        #  將圖片做灰階
        # self.im = cv2.imread(Path + "\\" + ImgName)
        if type =='url':
            image = np.asarray(bytearray(getimg), dtype="uint8")
            self.im = cv2.imdecode(image, cv2.IMREAD_COLOR)
        elif type=='local':
            self.im = cv2.imread(getimg)
        # self.dicImg.update({"原始驗證碼": self.im.copy()})


    #  閾值化
    def threshold(self):
        # 115 是 threshold，越高濾掉越多
        # 255 是當你將 method 設為 THRESH_BINARY_INV 後，高於 threshold 要設定的顏色
        # 反轉黑白 以利輪廓識別
        gray_image = cv2.cvtColor(self.im, cv2.COLOR_BGR2GRAY)
        retval, self.im = cv2.threshold(gray_image, 200, 255, cv2.THRESH_BINARY)
        # 存檔
        #cv2.imwrite("D:\\CaptchaRaw\\" + self.imageName + 'Threshold.png', self.im)
        self.dicImg.update({"閾值化": self.im.copy()})

    #  去噪
    def removeNoise(self):
        for i in xrange(len(self.im)):
            for j in xrange(len(self.im[i])):
                if self.CheckPixelIsBlack(self.im[i][j]):
                    count = 0
                    for k in range(-2, 3):
                        for l in range(-2, 3):
                            try:
                                if self.CheckPixelIsBlack(self.im[i + k][j + l]):
                                    count += 1
                            except IndexError:
                                pass
                    # 這裡 threshold 設 4，當週遭小於 4 個點的話視為雜點
                    if count <= 2:
                        self.im[i][j] = 0

        self.im = cv2.dilate(self.im, (2, 2), iterations=1)
        self.dicImg.update({"去噪": self.im.copy()})

    #  色調分離
    def posterization(self,levels=3):
        n = levels  # Number of levels of quantization

        indices = np.arange(0, 256)  # List of all colors

        divider = np.linspace(0, 255, n + 1)[1]  # we get a divider

        quantiz = np.int0(np.linspace(0, 255, n))  # we get quantization colors

        color_levels = np.clip(np.int0(indices / divider), 0, n - 1)  # color levels 0,1,2..

        palette = quantiz[color_levels]  # Creating the palette

        im2 = palette[self.im]  # Applying palette on image

        self.im = cv2.convertScaleAbs(im2)  # Converting image back to uint8
        #  存檔
        #cv2.imwrite("D:\\CaptchaRaw\\" + self.imageName + '.png', self.im)
        self.dicImg.update({"posterization": self.im.copy()})

    #  干擾線檢測  (只檢查寬度為2pxiel的直線&橫線)
    def removeBlackLines(self):
        chop = 4  #  線段長度大於chop 才判斷為干擾線
        lineColor = 255  #  將線段設定為黑或白色 255:白 0:黑
        (height, width,_) = self.im.shape
        #  loop 每一個pixel
        for i in xrange(height):
            for j in xrange(width):
                #  如果是黑色點 開始計算線段長度
                if self.CheckPixelIsBlack(self.im[i][j]):
                    countWidth = 0
                    countWidth2= 0
                    #  移除橫線 在每個像素找尋橫向的像素 如果<threshold 就count+1
                    for c in range(j, width):
                        try:
                            if self.CheckPixelIsBlack(self.im[i][c]) and self.CheckPixelIsWhite(self.im[i+1][c]):
                                countWidth += 1
                            else:
                                break
                        # 檢查下方有沒有直線
                            if self.CheckPixelIsBlack(self.im[i-1][c]) and self.CheckPixelIsWhite(self.im[i-2][c]):
                                countWidth2+=1
                            else:
                                break
                        except:
                            pass

                    #  如果大於指定長度 代表是線段
                    if countWidth >= chop and countWidth >= chop:
                        for c in range(countWidth):
                            try:
                                #  如果此點的上下兩個點是白的 代表不在數字裡 可以移除
                                if self.CheckPixelIsWhite(self.im[i+1, j+c]) and self.CheckPixelIsWhite(self.im[i-2, j+c]):
                                    self.im[i, j + c] =lineColor
                                    self.im[i-1, j + c] = lineColor

                            except IndexError:
                                self.im[i, j + c] = lineColor
                                self.im[i - 1, j + c] = lineColor

                    j += countWidth
        #  loop 每一個pixel
        for j in xrange(width):
            for i in xrange(height):
                #  如果是黑色點 開始計算線段長度
                if self.CheckPixelIsBlack(self.im[i][j]):
                    countHeight = 0
                    countHeight2 = 0
                    #  移除直線
                    for c in range(i, height):
                        try:
                            if self.CheckPixelIsBlack(self.im[c][j]) and self.CheckPixelIsWhite(self.im[c][j-1]):
                                countHeight += 1
                            else:
                                break
                        # 檢查右方有沒有直線
                            if self.CheckPixelIsBlack(self.im[c][j+1]) and self.CheckPixelIsWhite(self.im[c][j+2]):
                                countHeight2 += 1
                            else:
                                break
                        except:
                            pass
                    if countHeight >= chop and countHeight2 >= chop:
                        for c in range(countHeight):
                            try:
                                if self.CheckPixelIsWhite(self.im[i + c, j + 2]) and self.CheckPixelIsWhite(self.im[i + c, j - 1]):
                                    self.im[i + c, j] =lineColor
                                    self.im[i + c, j+1] =lineColor
                            except IndexError:
                                    self.im[i + c, j] = lineColor
                                    self.im[i + c, j+1] = lineColor

                    i += countHeight
        # 存檔
        # cv2.imwrite("D:\\CaptchaRaw\\" + self.imageName + '.png', self.im)
        self.dicImg.update({"干擾線檢測": self.im.copy()})

    #  傳入RGB的pixel 判斷是否是黑點  (色調分離後 干擾線的RGB會變(127,127,127))
    def CheckPixelIsBlack(self, pixel, min= 0,max= 127):
        return self.CheckPixelColor(pixel,min,max)
    #  傳入RGB的pixel 判斷是否是白點
    def CheckPixelIsWhite(self, pixel, min= 160,max= 255):
        return self.CheckPixelColor(pixel, min, max)

    def CheckPixelColor(self,pixel, min ,max ):
        if  min<= pixel[0] <= max and min<= pixel[1] <= max and min <= pixel[2] <= max:
            return True
        else:
            return False

    def medianBlur(self):
        self.im = cv2.medianBlur(self.im, 3)
        self.dicImg.update({"中值模糊": self.im})

    # 閉運算
    def mop_close(self):
        # slef.im[x,y] x是高 y是寬
        def morphological(operator=operator.gt):
            height, width, _ = self.im.shape
            # 創建一個空白圖片
            out_im = np.zeros((height,width,3), np.uint8)
            out_im.fill(255) # 填滿白色
            for y in range(height):
                for x in range(width):
                    try:
                        # 如果顏色是在雜點的range 就進行膨脹/腐蝕判斷
                        if self.im[y,x][0] in [127,255] and self.im[y,x][1] in [127,255] and self.im[y,x][2] in [127,255]:
                            nlst = neighbours(self.im,operator, y, x)
                            # 依據operator 將out_im[y,x]設為相鄰像素中最大或最小的像素
                            out_im[y, x] = nlst
                        # 如果顏色是驗證碼本體的範圍內 就進行雜點判斷
                        else:
                            if  not CheckIsNoise(self.im, y, x):
                                out_im[y,x] = self.im[y,x]
                    except Exception as e:
                        print('morphological:' + str(e))
            return out_im

        def neighbours(pix,operator,y, x):
            # nlst = []
            pixel = None
            # 搜尋im[y,x]周邊的像素 依據operator 找出最大或最小的像素
            for yy in range(y-1,y+1):
                for xx in range(x-1,x+1):
                    try:
                        if pixel is not None:
                            if operator(pix[yy,xx].sum(),pixel.sum()):
                                pixel = pix[yy,xx]
                        else:
                            pixel = pix[yy, xx]
                    except Exception as e:
                        print('neighbours:'+ str(e))
            return pixel
        # 判斷是不是雜點
        def CheckIsNoise(pix,y, x):
            count = 0
            try:
                for yy in range(y-2,y+3):
                    for xx in range(x-2,x+3):
                        # 如果不是255,255,255 代表不是白色點 count+1
                        if not self.CheckPixelColor(pix[yy,xx],255,255):
                            count += 1
            except:
                pass
            # 如果周圍的非白點小於3 代表是雜點
            if count <=3:
                return True
            else:
                return False


        def erosion(im):
            return morphological(operator.lt)

        def dilation(im):
            return morphological(operator.gt)

        self.im = dilation(self.im)
        self.im = erosion(self.im)

                # # 定義結構元素
        # kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (2, 2))
        #
        # # 閉運算
        # self.im = cv2.morphologyEx(self.im, cv2.MORPH_CLOSE, kernel)
        #
        # # 顯示腐蝕後的圖像
        self.dicImg.update({"morphological": self.im.copy()})

        # retval, t = cv2.threshold(self.im, 125, 1, cv2.THRESH_BINARY)
        # h_sum = t.sum(axis=0)
        # v_sum = t.sum(axis=1)
        # print('h_sum = '+str(type(h_sum))+ str(h_sum)+'\n')
        # print('h_sum>5= '+str((h_sum > 5))+'\n')
        # print('nonezero= '+str((h_sum > 5).nonzero()))
        # x1, x2 = (v_sum > 1).nonzero()[0][0], (v_sum > 1).nonzero()[0][-1]
        # y1, y2 = (h_sum > 5).nonzero()[0][0], (h_sum > 1).nonzero()[0][-1]
        # self.im = self.im[x1:x2, y1:y2]
        # self.dicImg.update({"移除空白部份": self.im.copy()})
    # 把多餘的白色圖片切掉
    def cutBlankImage(self):
        # 創建一個空白圖片(img.shape[0]為height,img.shape[1]為width)
        paintx = np.zeros(self.im.shape, np.uint8)
        # 將新圖像數組中的所有通道元素的值都設置為0
        cv2.cv.Zero(cv2.cv.fromarray(paintx))

        # 創建width長度都為0的數組 此陣列會存放每一column的黑點的個數
        num_of_valid_pix = [0] * self.im.shape[1]
        # 對每一行計算投影值
        for x in range(self.im.shape[1]):
            for y in range(self.im.shape[0]):
                t = cv2.cv.Get2D(cv2.cv.fromarray(self.im), y, x)
                if t[0] == 255: #  如果是黑色點就count+1
                    num_of_valid_pix[x] += 1

        # 繪製垂直投影圖
        for x in range(self.im.shape[1]):
            for y in range(num_of_valid_pix[x]):
                # 把為255的像素變成黑

                cv2.cv.Set2D(cv2.cv.fromarray(paintx), y, x, (255, 255, 255))
        self.dicImg.update({"投影": paintx})


        # ==============分割圖片=======================
        # letter_col_id = []  # @letter_col_id儲存每個數字所在的欄位的index
        # i = 0
        # # loop num_of_valid_pix陣列 如果黑色點數量>0 就將index加到letter_id 代表文字所在的index
        # while i in range(len(num_of_valid_pix)):
        #     letter_id = []  # @letter_id 儲存每個數字的欄位數
        #     # letter feature: there must be blank cols that contains no valid pixels in the column
        #     while num_of_valid_pix[i] != 0:
        #         letter_id.append(i)
        #         i += 1
        #     if letter_id:
        #         letter_col_id.append(letter_id)
        #     i += 1
        # # 確認每個字的寬度
        # numofLetters = len(letter_col_id)
        # # this part is dealing with the saparated
        # height = self.im.shape[0]
        # imgarr = []
        # print(numofLetters)
        # print('=============')
        # for j in range(numofLetters):
        #     colsForLetter = len(letter_col_id[j])
        #     print(len(letter_col_id[j]))
        #     if colsForLetter in range(5, 14):
        #         newimg = np.zeros((height,len(letter_col_id[j])), np.uint8)
        #         for y in range(height):
        #             # rowbuffer = []
        #             i = 0
        #             for x in letter_col_id[j]:
        #                 newimg.itemset((y, i), 0)
        #                 i += 1
        #                 imgarr.append(newimg)


        # cv2.line(self.im, (20, 0), (20, 60), (255, 255, 255))
        # self.im = self.im[:,:30] 切割圖片 讓圖片width減短

        # =====================================================

        # self.dicImg.update({u"切割": imgarr})


    #  切割圖片
    def splitImg(self):
        # 找出各輪廓的距離
        def find_if_close(cnt1, cnt2, distance):
            row1, row2 = cnt1.shape[0], cnt2.shape[0]
            for i in xrange(row1):
                for j in xrange(row2):
                    dist = np.linalg.norm(cnt1[i] - cnt2[j])
                    if abs(dist) < distance:
                        return True
                    elif i == row1 - 1 and j == row2 - 1:
                        return False
        # 傳入輪廓陣列 回傳各陣列的距離分級
        def getStatus(contours,distance):
            LENGTH = len(contours)
            status = np.zeros((LENGTH, 1))  # 用來儲存每個輪廓的等級 等級一樣的會合併為同一個輪廓

            for i, cnt1 in enumerate(contours):
                x = i
                if i != LENGTH - 1:
                    for j, cnt2 in enumerate(contours[i + 1:]):
                        x = x + 1
                        dist = find_if_close(cnt1, cnt2, distance)
                        if dist == True:
                            val = min(status[i], status[x])
                            status[x] = status[i] = val
                        else:
                            if status[x] == status[i]:
                                status[x] = i + 1
            return status
        # 合併各輪廓
        def MergeEachCnts(contours, distance,unified = [] ,excuteTimes = 0):
            '''
            :param contours: 要判斷距離的輪廓
            :param unified:  已經判斷完 合併後的輪廓
            :return:
            '''
            # print('============\n執行次數:'+ str(excuteTimes))
            unsucess = [] # 面積過大的輪廓放進來重新判斷
            # 取得各輪廓距離的分類
            status = getStatus(contours ,distance)
            # print('status:\n')
            # print(status)
            # print('areas and width:\n')
            maximum = int(status.max()) + 1
            for i in xrange(maximum):
                pos = np.where(status == i)[0]
                if pos.size != 0:
                    cont = np.vstack(contours[i] for i in pos)  # 把輪廓陣列裡的輪廓合併 pos的index對應到輪廓陣列的index
                    # hull = cv2.convexHull(cont) # 將合併後的輪廓轉凸包
                    # 如果面積大於200 就是錯誤合併兩個數字了
                    area = cv2.contourArea(cont)
                    (x, y, w, h) = cv2.boundingRect(cont)
                    # print(area)
                    # print(x, y, w, h)
                    # 當面積大於200或寬度大於20或高度大於20且distance大於0 才會加到錯誤判斷輪廓的陣列
                    if (area > 200 or w > 20 or h > 20) and distance > 0:
                        for i in pos:
                            unsucess.append(contours[i])
                    # 如果distance已經小於0 就把未經合併的原始輪廓加到unified
                    elif area > 200 and distance <= 0:
                        for i in pos:
                            unified.append(contours[i])
                    # 如果面積<26且寬高皆小於9 判斷為雜點
                    elif (area < 26 and w < 9 and h < 9):
                        pass
                    else:
                        unified.append(cont)
            if len(unsucess) > 0:
                return MergeEachCnts(unsucess,distance-3,unified, excuteTimes+1)
            else:
                return unified



        # 將圖片二值化 以便做邊緣檢測
        colorIm = self.im
        self.im = cv2.cvtColor(self.im , cv2.COLOR_BGR2GRAY)
        self.retval, self.im = cv2.threshold(self.im, 200, 255, cv2.THRESH_BINARY_INV)
        # 找出輪廓
        _, contours, hierarchy = cv2.findContours(self.im.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

        # 取出輪廓的範圍、區域大小 且過濾面積太小的輪廓
        contours = [c for c  in contours if 4 < cv2.contourArea(c) < 1000]
        # 將鄰近的輪廓合併
        unified = MergeEachCnts(contours, 7)

        a = colorIm.copy()
        cv2.drawContours(a, contours, -1, (255, 0, 0), 1)
        self.dicImg.update({"找出輪廓(合併前)": a})
        cv2.drawContours(colorIm, unified, -1, (255, 0, 0), 1)
        self.dicImg.update({"找出輪廓(合併後)": colorIm.copy()})

        # 依照X軸排序輪廓
        unified = sorted([(c ,cv2.boundingRect(c)[0],cv2.contourArea(c)) for c in unified], key=lambda x: x[1])
        # 再將太小的輪廓移除
        unified = [c for c,v,a in unified if 15 < a < 200]
        for index, c in enumerate(unified):
            (x, y, w, h) = cv2.boundingRect(c)
            # print('目前輪廓rect :'+str(x)+' '+ str(y)+' '+ str(w)+' '+ str(h))
            try:
                # 只將寬高大於 7 視為數字留存
                if w > 7 and h > 7:
                    add = True
                    for i,img in enumerate(self.arr):
                        # 計算此輪廓與其他輪廓重心的距離 如果小於10 代表這兩輪廓是重疊的 例如6 0 9 會造成此誤判
                        dist = math.sqrt((x - img[0]) ** 2 + (y - img[1]) ** 2)
                        # print('距離:'+str(dist))
                        if dist <= 10:
                            add = False
                            break
                    if add:
                        self.arr.append((x, y, w, h))

            except IndexError:
                pass
        Imgarr = [self.im[y: y + h, x: x + w] for x, y, w, h in self.arr]
        self.dicImg.update({"圖片切割": Imgarr})
        return Imgarr


    #  圖片轉正
    def positiveImg(self):

        imgarr = []
        for index, (x, y, w, h) in enumerate(self.arr):
            roi = self.im[y: y + h, x: x + w]
            thresh = roi.copy()

            angle = 0
            smallest = 999
            row, col = thresh.shape

            for ang in range(-60, 61):
                M = cv2.getRotationMatrix2D((col / 2, row / 2), ang, 1)
                t = cv2.warpAffine(thresh.copy(), M, (col, row))

                r, c = t.shape
                right = 0
                left = 999

                for i in xrange(r):
                    for j in xrange(c):
                        if t[i][j] == 255 and left > j:
                            left = j
                        if t[i][j] == 255 and right < j:
                            right = j

                if abs(right - left) <= smallest:
                    smallest = abs(right - left)
                    angle = ang

            M = cv2.getRotationMatrix2D((col / 2, row / 2), angle, 1)
            thresh = cv2.warpAffine(thresh, M, (col, row))
            # resize 成相同大小以利後續辨識
            thresh = cv2.resize(thresh, (50, 50))
            # Convert to RGB for QImage.
            thresh = cv2.cvtColor(thresh, cv2.COLOR_GRAY2RGB)
            imgarr.append(thresh)
        self.dicImg.update({"轉正": imgarr})
        return imgarr

    #  將圖片顯示出來
    def showImg(self, img=None):

        if img is None:
            img = self.im

        cv2.imshow(self.imageName, img)
        cv2.namedWindow(self.imageName, cv2.WINDOW_NORMAL)
        #  調整視窗 讓標題列顯示出來
        cv2.resizeWindow(self.imageName, 250, 60)
        cv2.waitKey()


    #  將多個圖片顯示在一個figure
    def showImgEveryStep(self,):
        diclength = len(self.dicImg)
        if diclength > 0:
            fig = plt.figure(figsize=(10, 10))
            gs = gridspec.GridSpec(diclength+1, 6)

            # 依序列出dict物件裡的圖片
            for index, key in enumerate(self.dicImg):
                #  如果不是list物件 就是圖片 可以呼叫imshow
                if not isinstance(self.dicImg[key], list):
                    ax = fig.add_subplot(gs[index, :6])
                    ax.imshow(self.dicImg[key], interpolation='nearest')
                    ax.set_title(key, fontproperties=self.font)
                else:
                    try:
                        for i, img in enumerate(self.dicImg[key]):
                            ax = fig.add_subplot(gs[index, i])
                            ax.imshow(img, interpolation='nearest')
                    except IndexError:
                        pass

            plt.tight_layout()
            plt.show()
        else:
            print '圖片數字陣列為空'
    #  存檔
    def SaveImg(self):

        cv2.imwrite("D:\\CaptchaRaw\\" + self.imageName + '.png', self.im)

    # 開始執行 回傳處理完後的圖片陣列
    def StartProcess(self,path = None,fileName = None):
        #  取得驗證碼資料夾裡 隨機一個驗證碼的路徑
        self.posterization() #色調分離
        self.mop_close() #閉運算
        self.removeBlackLines() #直線檢測
        self.splitImg()
        return self.positiveImg()

# 目前步驟 1.色調分離 濾掉背景色 2.移除黑線
if __name__ == '__main__':
    for i in range(10):
        #  取得驗證碼資料夾裡 隨機一個驗證碼的路徑
        req = requests.get('http://railway.hinet.net/ImageOut.jsp')
        x = Image(req.content)
        # path=r'D:\FailCaptcha\26.png'
        # x = Image(path,'local')

        x.posterization() #色調分離
        x.mop_close() #閉運算
        # x.SaveImg()
        x.removeBlackLines() #直線檢測
        # x.removeNoise()
        # x.threshold()
        x.splitImg()
        x.positiveImg()
        x.showImgEveryStep()
