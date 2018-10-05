package ch03;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

public class Ch03_1_999TestPolar
{

    public static void main(String[] args)
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);

        Mat m1 = new Mat(1, 4, CvType.CV_32FC1);
        m1.put(0, 0, 1);
        m1.put(0, 1, 3);
        m1.put(0, 2, 6);
        m1.put(0, 3, 10);


        Mat m2 = new Mat(1, 4, CvType.CV_32FC1);
        m2.put(0, 0, 1);
        m2.put(0, 1, 4);
        m2.put(0, 2, 8);
        m2.put(0, 3, 10);
        Mat magnitude = new Mat();
        Mat angle = new Mat();

        Core.cartToPolar(m1, m2, magnitude, angle);

        System.out.println("矩陣m1所有元素=" + magnitude.dump() + "," + angle.dump());


        Mat findM1 = new Mat();
        Mat findM2 = new Mat();

        Core.polarToCart(magnitude, angle, findM1, findM2);

        System.out.println("矩陣m1所有元素=" + findM1.dump() + "," + findM2.dump());
    }

}
