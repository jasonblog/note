package ch10;

import java.util.ArrayList;
import java.util.List;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Point;
import org.opencv.core.RotatedRect;
import org.opencv.core.Scalar;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;
//³Ì¤º¾ò¶ê
public class Ch10_4_7FitEllipseContainUnknowPoints
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        Mat img = Mat.zeros(250, 250, CvType.CV_8UC3);

        MatOfPoint2f contours = new MatOfPoint2f();
        List<Point> points = new ArrayList<Point>();
        //contours
        int X, Y;

        for (int i = 0; i < 20; i++) {
            X = (int)(Math.random() * 200 + 40);
            Y = (int)(Math.random() * 200 + 40);
            Core.circle(img, new Point(X, Y), 2, new Scalar(0, 0, 255)); //red
            points.add(new Point(X, Y));

        }

        contours.fromList(points);
        RotatedRect r = Imgproc.fitEllipse(contours);
        Core.ellipse(img, r, new Scalar(0, 255, 0), 3, 8);

        Highgui.imwrite("C://opencv3.1//samples//rectContainPoint-fitEllipse.jpg", img);

    }

}
