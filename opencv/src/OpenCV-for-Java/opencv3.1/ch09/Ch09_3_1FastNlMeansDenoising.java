package ch09;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.photo.Photo;
public class Ch09_3_1FastNlMeansDenoising
{

    public static void main(String args[])
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        Mat im = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg");
        Mat img = Imgcodecs.imread("C://opencv3.1//samples//lena-gray.jpg",
                                   Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
        Photo.fastNlMeansDenoising(im, im);
        Imgcodecs.imwrite("C://opencv3.1//samples//fastNlMeansDenoising_lena.jpg", im);
        Imgcodecs.imwrite("C://opencv3.1//samples//fastNlMeansDenoising_lena-gray.jpg",
                          img);
    }
}
