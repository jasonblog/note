package ch03;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

public class Ch03_1_999HandleMat_TestError
{

    public static void main(String[] args)
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);

        //Mat m1 = new Mat(2, 2, CvType.CV_8UC1);
        Mat m1 = new Mat(3, 2, CvType.CV_32FC1);
        m1.put(0, 0, 1);
        m1.put(0, 1, 2);
        m1.put(1, 0, 3);
        m1.put(1, 1, 4);


        Mat m2 = Mat.eye(2, 2, CvType.CV_8UC1);

        System.out.println("矩陣m1所有元素=" + m1.dump());
        System.out.println("矩陣m1的轉置矩陣=" + m1.t().dump());
        System.out.println("矩陣m1的反矩陣/逆矩陣=" + m1.inv().dump());
        System.out.println("矩陣m1的單位矩陣=" + m2.dump());

        Mat m3  = Mat.ones(2, 2, CvType.CV_8UC1);
        System.out.println("2X2全1矩陣m3所有元素=" + m3.dump());

        Mat m4 = m1.reshape(4, 1);
        System.out.println("矩陣m4是m1轉型成1列4行的矩陣,其所有元素=" + m4.dump());

        Mat m5 = m1.reshape(1, 4);
        System.out.println("矩陣m5是m1轉型成4列1行的矩陣,其所有元素=" + m5.dump());
    }

}
