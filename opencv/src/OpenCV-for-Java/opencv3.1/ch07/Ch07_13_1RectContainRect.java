package ch07;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

public class Ch07_13_1RectContainRect
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        Mat img = Mat.zeros(250, 250, CvType.CV_8UC3);

        List<Integer> pointX = new ArrayList<Integer>();
        List<Integer> pointY = new ArrayList<Integer>();
        int X0, Y0, X1, Y1, maxX, maxY, minX, minY;

        for (int i = 0; i < 20; i++) {
            X0 = (int)(Math.random() * 200 + 10);
            Y0 = (int)(Math.random() * 200 + 10);
            X1 = (int)(Math.random() * 200 + 10);
            Y1 = (int)(Math.random() * 200 + 10);
            Imgproc.rectangle(img, new Point(X0, Y0), new Point(X1, Y1), new Scalar(0, 255,
                              0));
            pointX.add(X0);
            pointX.add(X1);
            pointY.add(Y0);
            pointY.add(Y1);

        }

        maxX = Collections.max(pointX);
        maxY = Collections.max(pointY);
        minX = Collections.min(pointX);
        minY = Collections.min(pointY);

        Imgproc.rectangle(img, new Point(minX, minY), new Point(maxX, maxY),
                          new Scalar(0, 0, 255));



        Imgcodecs.imwrite("C://opencv3.1//samples//rectContainRect.jpg", img);

    }

}
