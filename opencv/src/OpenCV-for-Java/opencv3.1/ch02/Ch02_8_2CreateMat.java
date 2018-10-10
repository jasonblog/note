package ch02;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

public class Ch02_8_2CreateMat
{

    public static void main(String[] args)
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);

        byte[][] javaData = {
            {1, 2},
            {3, 4},
            {5, 6},
            {7, 8}
        };//[4][2]

        Mat m1  = new Mat(4, 2, CvType.CV_8UC1);
        m1.put(0, 0, javaData[0]);
        m1.put(1, 0, javaData[1]);
        m1.put(2, 0, javaData[2]);
        m1.put(3, 0, javaData[3]);

        System.out.println("矩陣m1的行數=" + m1.cols());
        System.out.println("矩陣m1的列數=" + m1.rows());
        System.out.println("矩陣m1所有元素個數=" + m1.total());
        System.out.println("矩陣m1的Size=" + m1.size());
        System.out.println("矩陣m1(1,1)的元素=" + m1.get(0, 0)[0]);
        System.out.println("矩陣m1(3,1)的元素=" + m1.get(3, 1)[0]);
        System.out.println("矩陣m1所有元素=" + m1.dump());

    }

}
