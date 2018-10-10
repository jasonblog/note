package ch08;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;

public class Ch08_6_1GrabCut
{
    public static void main(String[] args)
    {
        try {
            System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
            Mat source = Highgui.imread("C://opencv3.1//samples//lena.jpg",
                                        Highgui.CV_LOAD_IMAGE_COLOR);
            bgSubtracting(source);
        } catch (Exception e) {
            System.out.println("error: " + e.getMessage());
        }
    }

    private static void bgSubtracting(Mat img)
    {
        Mat firstMask = new Mat();
        Mat bgModel = new Mat();//grabCut內部運算使用
        Mat fgModel = new Mat();//grabCut內部運算使用
        Mat mask;
        Mat source = new Mat(1, 1, CvType.CV_8U, new Scalar(3.0));
        //定義矩形
        Rect rect = new Rect(0, 0, 150, 200);

        ///執行grabCut處理(分割)行
        Imgproc.grabCut(img, firstMask, rect, bgModel, fgModel, 1, 0);


        //得到前景
        Core.compare(firstMask, source, firstMask, Core.CMP_EQ);
        //生成輸出圖像
        Mat foreground = new Mat(img.size(), CvType.CV_8UC3, new Scalar(255,
                                 255, 255));
        //複製前景數據,符合挖出的區域
        img.copyTo(foreground, firstMask);

        mask = new Mat(foreground.size(), CvType.CV_8UC1, new Scalar(255, 255, 255));

        Imgproc.cvtColor(foreground, mask, Imgproc.COLOR_BGR2GRAY);
        Imgproc.threshold(mask, mask, 254, 255, Imgproc.THRESH_BINARY_INV);
        //挖出後的替代color
        Mat vals = new Mat(1, 1, CvType.CV_8UC3, new Scalar(255.0));

        img.setTo(vals, mask);
        Highgui.imwrite("C://opencv3.1//samples//grabcut-lena.jpg", img);
    }
}