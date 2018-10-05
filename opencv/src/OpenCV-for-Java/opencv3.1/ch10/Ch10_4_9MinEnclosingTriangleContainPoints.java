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
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
//boundingRect
public class Ch10_4_9MinEnclosingTriangleContainPoints
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        Mat img = Mat.zeros(380, 320, CvType.CV_8UC3);

        MatOfPoint contours = new MatOfPoint();
        List<Point> points = new ArrayList<Point>();
        //contours
        int X, Y;

        for (int i = 0; i < 20; i++) {
            X = (int)(Math.random() * 150 + 40);
            Y = (int)(Math.random() * 150 + 80);
            Imgproc.circle(img, new Point(X, Y), 2, new Scalar(0, 255, 0)); //red
            points.add(new Point(X, Y));

        }

        contours.fromList(points);
        Mat triangle = new Mat();
        //Rect r=Imgproc.boundingRect(contours);
        //Core.rectangle(img, new Point(minX,minY), new Point(maxX,maxY), new Scalar(0, 0, 255));
        //Imgproc.rectangle(img, r.tl(), r.br(), new Scalar(0,  0,255));
        Imgproc.minEnclosingTriangle(contours, triangle);
        //System.out.println(triangle.dump());
        //System.out.println(triangle.get(0, 0)[0]+","+triangle.get(0, 0)[1]);


        Point[] allPoint = new Point[3];

        for (int i = 0; i < triangle.rows(); i++) {
            Point tmpPoint = new Point(triangle.get(i, 0)[0], triangle.get(i, 0)[1]);
            allPoint[i] = tmpPoint;
        }


        for (int i = 0; i < 3; i++) {
            Imgproc.line(img, allPoint[i], allPoint[(i + 1) % 3], new Scalar(0, 0, 255), 2);
        }


        Imgcodecs.imwrite("C://opencv3.1//samples//MinEnclosingTriangleContainPoints.jpg",
                          img);

    }

}
