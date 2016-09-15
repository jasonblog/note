# OpenGL，glx,glaux,glut，freeglut,glew，glfw，mesa


##一、OpenGL

OpenGL函數庫相關的API有核心庫(gl)，實用庫(glu)，輔助庫(aux)、實用工具庫(glut)，窗口庫(glx、agl、wgl)和擴展函數庫等。

gl是核心，glu是對gl的部分封裝。glx、agl、wgl 是針對不同窗口系統的函數。glut是為跨平臺的OpenGL程序的工具包，比aux功能強大（aux很大程度上已經被glut庫取代。）。擴展函數庫是硬件廠商為實現硬件更新利用OpenGL的擴展機制開發的函數。

OpenGL的競爭對手是Direct3D；OpenGL對應的開源實現是mesa 3D。


##二、gult：OpenGL工具庫 OpenGL Utility Toolkit

這部分函數以glut開頭，主要包括窗口操作函數，窗口初始化、窗口大小、窗口位置等函數；回調函數：響應刷新消息、鍵盤消息、鼠標消息、定時器函數等；創建複雜的三維物體；菜單函數；程序運行函數。

gult對應的開源實現是freegult。

##三、窗口庫GLX

對於X窗口系統，它所使用的的OpenGL擴展（GLX：OpenGL extension for X.）是作為OpenGL的一個附件提供的，所有的GLX函數都使用前綴glX。

apl、wgl分別用於apple、windows。

##四、glew

GLUT或者FREEGLUT主要是1.0的基本函數功能；GLEW是使用OPENGL2.0之後的一個工具函數。

不同的顯卡公司，也會發布一些只有自家顯卡才支 持的擴展函數，你要想用這數涵數，不得不去尋找最新的glext.h,有了GLEW擴展庫，你就再也不用為找不到函數的接口而煩惱，因為GLEW能自動識別你的平臺所支持的全部OpenGL高級擴展函數。也就是說，只要包含一個glew.h頭文件，你就能使用gl,glu,glext,wgl,glx的全部函數。

##五、glfw

GLFW無愧於其號稱的lightweight的OpenGL框架，的確是除了跨平臺必要做的事情都沒有做，所以一個頭文件，很少量的API，就完成了任務。GLFW的開發目的是用於替代glut的，從代碼和功能上來看，我想它已經完全的完成了任務。

一個輕量級的，開源的，跨平臺的library。支持OpenGL及OpenGL ES，用來管理窗口，讀取輸入，處理事件等。因為OpenGL沒有窗口管理的功能，所以很多熱心的人寫了工具來支持這些功能，比如早期的glut，現在的freeglut等。那麼GLFW有何優勢呢？glut太老了，最後一個版本還是90年代的。freeglut完全兼容glut，算是glut的代替品，功能齊全，但是bug太多。穩定性也不好（不是我說的啊），GLFW應運而生。

##六、層次關係

aux->glut->freeglut->glfw

glew

glu 

|
opengl1.0 glx/apl/wgl opengl2.0及以上

![](./images/20150117134719759)