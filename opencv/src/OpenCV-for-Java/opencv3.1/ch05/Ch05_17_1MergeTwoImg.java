package ch05;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Rect;
import org.opencv.imgcodecs.Imgcodecs;
//2張影像合併
public class Ch05_17_1MergeTwoImg
{
    public static void main(String[] args)
    {
        try {
            System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
            Mat source = Imgcodecs.imread("C://opencv3.1//samples//ncku.jpg",
                                          Imgcodecs.CV_LOAD_IMAGE_COLOR);
            Mat source1 = Imgcodecs.imread("C://opencv3.1//samples//jelly_studio_logo.jpg",
                                           Imgcodecs.CV_LOAD_IMAGE_COLOR);
            Mat destination = source.clone();

            //Imgproc.resize
            Rect roi = new Rect(50, 50, 90, 62);
            Mat destinationROI = source.submat(roi);
            source1.copyTo(destinationROI, source1);

            Imgcodecs.imwrite("C://opencv3.1//samples//merge2.jpg", source);
        } catch (Exception e) {
            System.out.println("error: " + e.getMessage());
        }
    }
}