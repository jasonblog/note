package ch07;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

public class Ch07_11_2FloodFill
{

    public static void main(String[] args)
    {
        try {
            System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
            Mat source = Imgcodecs.imread("C://opencv3.1//samples//DSC_0068.jpg");

            Point seedPoint = new Point(30, 30);
            Mat mask = new Mat();
            Rect rect = new Rect();
            Imgproc.floodFill(source, mask, seedPoint, new Scalar(0, 130, 120), rect,
                              new Scalar(10, 10, 10), new Scalar(20, 20, 20), 4);
            Imgcodecs.imwrite("C://opencv3.1//samples//flood-DSC_0068.jpg", source);
        } catch (Exception e) {
            System.out.println("error: " + e.getMessage());
        }
    }
}