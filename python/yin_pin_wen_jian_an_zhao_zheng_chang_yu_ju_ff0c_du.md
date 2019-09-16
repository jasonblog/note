# 音頻文件按照正常語句，斷句拆分的處理方法



https://github.com/jiaaro/pydub


- Installation

```sh
pip install git+https://github.com/jiaaro/pydub.git@master
```


```py
from pydub import AudioSegment
from pydub.silence import split_on_silence
import os

# 初始化
audiopath = "C:/Users/L/Desktop/test/test.mp3"
audiotype = 'mp3' #如果wav、mp4其他格式參看pydub.AudioSegment的API
# 讀入音頻
print('讀入音頻')
sound = AudioSegment.from_file(audiopath, format=audiotype)
sound = sound[:3*60*1000] #如果文件較大，先取前3分鐘測試，根據測試結果，調整參數
# 分割 
print('開始分割')
chunks = split_on_silence(sound,min_silence_len=300,silence_thresh=-70)#min_silence_len: 拆分語句時，靜默滿0.3秒則拆分。silence_thresh：小於-70dBFS以下的為靜默。
# 創建保存目錄
filepath = os.path.split(audiopath)[0]
chunks_path = filepath+'/chunks/'
if not os.path.exists(chunks_path):os.mkdir(chunks_path)
# 保存所有分段
print('開始保存')
for i in range(len(chunks)):
    new = chunks[i]
    save_name = chunks_path+'%04d.%s'%(i,audiotype)
    new.export(save_name, format=audiotype)
    print('%04d'%i,len(new))
print('保存完畢')
```