package ch10;

import java.util.ArrayList;
import java.util.List;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;
//boundingRect
public class Ch10_4_4RectContainUnknowPoints
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        Mat img = Mat.zeros(250, 250, CvType.CV_8UC3);

        MatOfPoint contours = new MatOfPoint();
        List<Point> points = new ArrayList<Point>();
        //contours
        int X, Y;

        for (int i = 0; i < 20; i++) {
            X = (int)(Math.random() * 200 + 10);
            Y = (int)(Math.random() * 200 + 10);
            Core.circle(img, new Point(X, Y), 2, new Scalar(0, 255, 0)); //red
            points.add(new Point(X, Y));

        }

        contours.fromList(points);
        Rect r = Imgproc.boundingRect(contours);
        //Core.rectangle(img, new Point(minX,minY), new Point(maxX,maxY), new Scalar(0, 0, 255));
        Core.rectangle(img, r.tl(), r.br(), new Scalar(0,  0, 255));

        Highgui.imwrite("C://opencv3.1//samples//rectContainPoint-boundingRect.jpg",
                        img);

    }

}
