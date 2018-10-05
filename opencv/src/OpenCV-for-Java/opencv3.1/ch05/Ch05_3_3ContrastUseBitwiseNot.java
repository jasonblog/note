package ch05;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;

public class Ch05_3_3ContrastUseBitwiseNot
{
    public static void main(String[] args)
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        Mat img = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg");
        Core.bitwise_not(img, img);
        Imgcodecs.imwrite("C://opencv3.1//samples//lena-contrast3.jpg", img);
    }
}
