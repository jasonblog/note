package ch09;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.photo.Photo;
public class Ch09_2_1Decolor
{
    public static void main(String args[])
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        Mat im = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg");
        Mat color_boost = new Mat();
        Mat grayscale = new Mat();
        Photo.decolor(im, grayscale, color_boost);
        Imgcodecs.imwrite("C://opencv3.1//samples//decolor_boost.jpg", color_boost);
        Imgcodecs.imwrite("C://opencv3.1//samples//decolor_grayscale.jpg", grayscale);
    }
}
