package ch09;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.photo.Photo;
public class Ch09_3_2FastNlMeansDenoisingColored
{

    public static void main(String args[])
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        Mat im = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg");
        Mat dst = new Mat();
        Photo.fastNlMeansDenoisingColored(im, dst);
        Imgcodecs.imwrite("C://opencv3.1//samples//fastNlMeansDenoisingColored_lena.jpg",
                          dst);
    }
}
