package ch07;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.highgui.Highgui;

public class Ch07_12_1RectContainPoint
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        Mat img = Mat.zeros(250, 250, CvType.CV_8UC3);

        List<Integer> pointX = new ArrayList<Integer>();
        List<Integer> pointY = new ArrayList<Integer>();
        int X, Y, maxX, maxY, minX, minY;

        for (int i = 0; i < 20; i++) {
            X = (int)(Math.random() * 200 + 10);
            Y = (int)(Math.random() * 200 + 10);
            Core.circle(img, new Point(X, Y), 2, new Scalar(0, 255, 0));
            pointX.add(X);
            pointY.add(Y);

        }

        maxX = Collections.max(pointX);
        maxY = Collections.max(pointY);
        minX = Collections.min(pointX);
        minY = Collections.min(pointY);

        Core.rectangle(img, new Point(minX, minY), new Point(maxX, maxY), new Scalar(0,
                       0, 255));



        Highgui.imwrite("C://opencv3.1//samples//rectContainPoint.jpg", img);

    }

}
