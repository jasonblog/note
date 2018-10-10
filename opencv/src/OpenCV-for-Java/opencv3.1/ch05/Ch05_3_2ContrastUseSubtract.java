package ch05;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.imgcodecs.Imgcodecs;

public class Ch05_3_2ContrastUseSubtract
{
    public static void main(String[] args)
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);

        try {

            Mat src = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg");
            Mat destination = new Mat(src.rows(), src.cols(), src.type(), new Scalar(255,
                                      255, 255));
            Mat dst = new Mat(src.rows(), src.cols(), src.type());
            Core.subtract(destination, src, dst);
            Imgcodecs.imwrite("C://opencv3.1//samples//lena-contrast2.jpg", dst);
        } catch (Exception e) {
            System.out.println("error: " + e.getMessage());
        }
    }
}