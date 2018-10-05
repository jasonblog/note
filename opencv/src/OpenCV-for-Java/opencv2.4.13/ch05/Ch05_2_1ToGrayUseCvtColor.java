package ch05;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;

public class Ch05_2_1ToGrayUseCvtColor
{
    public static void main(String[] args)
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);

        try {

            Mat src = Highgui.imread("C://opencv3.1//samples//lena.jpg");
            Mat dst = new Mat(src.rows(), src.cols(), src.type());
            Imgproc.cvtColor(src, dst, Imgproc.COLOR_RGB2GRAY);
            Highgui.imwrite("C://opencv3.1//samples//lena-gray.jpg", dst);
        } catch (Exception e) {
            System.out.println("error: " + e.getMessage());
        }
    }
}