package ch10;

import java.util.ArrayList;
import java.util.List;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
public class Ch10_4_6MinEnclosingCircleContainUnknowPoints
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        Mat img = Mat.zeros(300, 300, CvType.CV_8UC3);

        MatOfPoint2f contours = new MatOfPoint2f();
        List<Point> points = new ArrayList<Point>();
        //contours
        int X, Y;

        for (int i = 0; i < 20; i++) {
            X = (int)(Math.random() * 200 + 40);
            Y = (int)(Math.random() * 200 + 40);
            Imgproc.circle(img, new Point(X, Y), 2, new Scalar(0, 0, 255)); //red
            points.add(new Point(X, Y));

        }

        contours.fromList(points);
        Point center = new Point();
        float[] radius = new float[1];
        Imgproc.minEnclosingCircle(contours, center, radius);
        Imgproc.circle(img, center, (int)radius[0], new Scalar(0, 255, 0), 2);
        Imgcodecs.imwrite("C://opencv3.1//samples//rectContainPoint-minEnclosingCircle.jpg",
                          img);

    }

}
