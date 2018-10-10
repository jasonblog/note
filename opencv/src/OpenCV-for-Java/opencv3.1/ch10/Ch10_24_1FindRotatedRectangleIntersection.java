package ch10;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.RotatedRect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

public class Ch10_24_1FindRotatedRectangleIntersection
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {

        Mat image2d = new Mat(700, 700, CvType.CV_8UC3, new Scalar(255, 255, 255));

        //矩形1
        RotatedRect rect1 = new RotatedRect(new Point(175, 175), new Size(150, 150), 0);
        Imgproc.rectangle(image2d, new Point(100, 100), new Point(250, 250),
                          new Scalar(255, 0, 0));

        //矩形2
        RotatedRect rect2 = new RotatedRect(new Point(200, 200), new Size(160, 60), 0);
        Imgproc.rectangle(image2d, new Point(120, 170), new Point(280, 230),
                          new Scalar(0, 255, 0));

        Mat intersectingRegion = new Mat();

        //求交叉
        int sumOfIntersection = Imgproc.rotatedRectangleIntersection(rect1, rect2,
                                intersectingRegion);
        //System.out.println(intersectingRegion.dump());
        System.out.println("return值=" + sumOfIntersection);

        //繪出交叉點
        for (int i = 0; i < intersectingRegion.rows(); i++) {
            Point tmpPoint = new Point(intersectingRegion.get(i, 0)[0],
                                       intersectingRegion.get(i, 0)[1]);
            Imgproc.circle(image2d, tmpPoint, 4, new Scalar(0, 0, 255));
        }

        Imgcodecs.imwrite("C://opencv3.1//samples//FindRotatedRectangleIntersection.jpg",
                          image2d);
    }

}
