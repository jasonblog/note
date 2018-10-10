package ch05;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Size;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;

public class Ch05_15_1Mosaic
{
    public static void main(String args[])
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        Mat im = Highgui.imread("C://opencv3.1//samples//lena.jpg");
        Imgproc.resize(im, im, new Size(), 0.1, 0.1, Imgproc.INTER_NEAREST);
        Imgproc.resize(im, im, new Size(), 10.0, 10.0, Imgproc.INTER_NEAREST);
        Highgui.imwrite("C://opencv3.1//samples//Mosaic-lena.jpg", im);
    }
}