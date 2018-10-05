package ch05;
import java.util.ArrayList;
import java.util.List;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;

public class Ch05_14_1Concat
{
    public static void main(String[] args)
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        Mat im = Imgcodecs.imread("C://opencv3.1//samples//ncku.jpg");
        List<Mat> matList = new ArrayList<Mat>();
        Mat hdst = new Mat();
        Mat vdst = new Mat();
        //3¦¸
        matList.add(im);
        matList.add(im);
        matList.add(im);
        Core.hconcat(matList, hdst);
        Core.vconcat(matList, vdst);

        Imgcodecs.imwrite("C://opencv3.1//samples//hconcat_ncku.jpg", hdst);
        Imgcodecs.imwrite("C://opencv3.1//samples//vconcat_ncku.jpg", vdst);
    }
}
