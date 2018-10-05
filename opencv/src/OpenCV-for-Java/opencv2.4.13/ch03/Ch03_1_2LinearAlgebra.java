package ch03;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;

public class Ch03_1_2LinearAlgebra
{
    static{System.loadLibrary(Core.NATIVE_LIBRARY_NAME);}
    public static void main(String[] args)
    {


        Mat m1 = new Mat(2, 2, CvType.CV_32FC1);
        m1.put(0, 0, 1);
        m1.put(0, 1, 2);
        m1.put(1, 0, 3);
        m1.put(1, 1, 4);

        Mat m2 = m1.clone();

        System.out.println("矩陣m2是複製m1,所有元素=" + m2.dump());

        //加
        Mat m3 = new Mat();
        Core.add(m1, m2, m3);
        System.out.println("m1+m2=矩陣m3所有元素=" + m3.dump());
        m3 = new Mat();
        Core.scaleAdd(m1, 1, m2, m3);
        System.out.println("m1+m2=矩陣m3另一寫法=" + m3.dump());

        //減
        Mat m4 = new Mat();
        Core.subtract(m1, m2, m4);
        System.out.println("m1-m2=矩陣m4所有元素=" + m4.dump());

        //乘
        Mat m5 = new Mat();
        Core.gemm(m1, m2, 1, new Mat(), 0, m5);
        System.out.println("m1*m2=矩陣m5所有元素=" + m5.dump());

        Mat m6 = m1.mul(m2);
        System.out.println("m1,m2=矩陣內元素對乘m6所有元素=" + m6.dump());


        Mat m7 = new Mat(2, 2, CvType.CV_32FC1, new Scalar(3));
        Mat m8 = new Mat();
        m8 = m1.mul(m7);
        System.out.println("m1矩陣內所有元素都乘以3,即3*[matrix]=" + m8.dump());

        //除
        Mat m9 = new Mat(2, 2, CvType.CV_32FC1);
        Core.divide(m1, Scalar.all(3), m9);
        System.out.println("(1/3)m1矩陣m9所有元素=" + m9.dump());

        Mat m10  = new Mat(2, 2, CvType.CV_32FC1, new Scalar(2));
        Mat m11 = new Mat();
        Core.divide(m1, m10, m11);
        System.out.println("(1/2)m1矩陣m11所有元素=" + m11.dump());

        //mask
        Mat mask = new Mat(2, 2, CvType.CV_8UC1);
        mask.put(0, 0, 1);
        mask.put(0, 1, 1);
        mask.put(1, 0, 1);
        mask.put(1, 1, 0);

        m3 = new Mat();
        Core.add(m1, m2, m3, mask);
        System.out.println("m1+m2=矩陣m3及使用遮罩(1,1)位置處理=" + m3.dump());


    }

}
