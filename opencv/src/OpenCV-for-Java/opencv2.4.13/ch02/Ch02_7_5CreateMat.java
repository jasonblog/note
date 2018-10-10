package ch02;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

public class Ch02_7_5CreateMat
{

    public static void main(String[] args)
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);

        Mat m1  = new Mat(2, 2, CvType.CV_8UC1) {
            {
                put(0, 0, 1);
                put(0, 1, 2);
                put(1, 0, 3);
                put(1, 1, 4);
            }

        };


        System.out.println("矩陣m1的行數=" + m1.cols());
        System.out.println("矩陣m1的列數=" + m1.rows());
        System.out.println("矩陣m1所有元素個數=" + m1.total());
        System.out.println("矩陣m1的Size=" + m1.size());
        System.out.println("矩陣m1(1,1)的元素=" + m1.get(0, 0)[0]);
        System.out.println("矩陣m1(2,2)的元素=" + m1.get(1, 1)[0]);
        System.out.println("矩陣m1所有元素=" + m1.dump());


        Mat m2 = Mat.zeros(1, 3, CvType.CV_8UC1);
        System.out.println("矩陣m2所有元素=" + m2.dump());

        Mat m3 = Mat.ones(1, 3, CvType.CV_8UC1);
        System.out.println("矩陣m3所有元素=" + m3.dump());

    }

}
