package ch02;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;

public class Ch02_7_4CreateMat
{

    public static void main(String[] args)
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);

        Mat m1  = new Mat(2, 2, CvType.CV_8UC1, new Scalar(9));


        System.out.println("矩陣m1的行數=" + m1.cols());
        System.out.println("矩陣m1的列數=" + m1.rows());
        System.out.println("矩陣m1所有元素個數=" + m1.total());
        System.out.println("矩陣m1的Size=" + m1.size());
        System.out.println("矩陣m1(1,1)的元素=" + m1.get(0, 0)[0]);
        System.out.println("矩陣m1(2,2)的元素=" + m1.get(1, 1)[0]);
        System.out.println("矩陣m1所有元素=" + m1.dump());

    }

}
