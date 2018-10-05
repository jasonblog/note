package ch05;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Rect;
import org.opencv.highgui.Highgui;
//2張影像合併
public class Ch05_17_1MergeTwoImg
{
    public static void main(String[] args)
    {
        try {
            System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
            Mat source = Highgui.imread("C://opencv3.1//samples//ncku.jpg",
                                        Highgui.CV_LOAD_IMAGE_COLOR);
            Mat source1 = Highgui.imread("C://opencv3.1//samples//jelly_studio_logo.jpg",
                                         Highgui.CV_LOAD_IMAGE_COLOR);
            Mat destination = source.clone();

            //Imgproc.resize
            Rect roi = new Rect(50, 50, 90, 62);
            Mat destinationROI = source.submat(roi);
            source1.copyTo(destinationROI, source1);

            Highgui.imwrite("C://opencv3.1//samples//merge2.jpg", source);
        } catch (Exception e) {
            System.out.println("error: " + e.getMessage());
        }
    }
}