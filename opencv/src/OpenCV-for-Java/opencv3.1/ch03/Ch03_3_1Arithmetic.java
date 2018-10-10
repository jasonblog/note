package ch03;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;

public class Ch03_3_1Arithmetic
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }

    public static void main(String[] args)
    {
        Mat m1 = new Mat(2, 2, CvType.CV_32FC1);
        m1.put(0, 0, 1);
        m1.put(0, 1, 2);
        m1.put(1, 0, 3);
        m1.put(1, 1, 4);

        Mat m2 = new Mat(2, 2, CvType.CV_32FC1, new Scalar(8));
        System.out.println("m1=" + m1.dump());
        System.out.println("m2=" + m2.dump());

        // absdiff
        Mat absdiffMat = new Mat();
        Core.absdiff(m1, m2, absdiffMat);
        System.out.println("m1與m2兩矩每一元素陣相差之絕對值=" + absdiffMat.dump());

        // addWeighted
        Mat addWeightedMat = new Mat();
        Core.addWeighted(m1, 2, m2, 3, 4, addWeightedMat);
        System.out.println("m1*2+m2*3+4=" + addWeightedMat.dump());

        // bitwise_and
        Mat bitwiseAndMat = new Mat();
        Core.bitwise_and(m1, m1, bitwiseAndMat);
        System.out.println("m1與m1 bitwise_and=" + bitwiseAndMat.dump());

        // bitwise_not
        Mat bitwiseNotMat = new Mat();
        Core.bitwise_not(m1, bitwiseNotMat);
        System.out.println("m1的bitwise_not=" + bitwiseNotMat.dump());

        // bitwise_or
        Mat bitwiseOrMat = new Mat();
        Core.bitwise_or(m1, m1, bitwiseOrMat);
        System.out.println("m1與m1 bitwise_or=" + bitwiseOrMat.dump());

        // bitwise_xor
        Mat bitwiseXorMat = new Mat();
        Core.bitwise_xor(m1, m1, bitwiseXorMat);
        System.out.println("m1與m1 bitwise_xor=" + bitwiseXorMat.dump());

        // cubeRoot
        System.out.println("8的立方根=" + Core.cubeRoot(8));

        //exp
        Mat expMat = new Mat();
        Core.exp(m1, expMat);
        System.out.println("m1每個元素的指數=" + expMat.dump());

        //log
        Mat logMat = new Mat();
        Core.log(m1, logMat);
        System.out.println("m1每個元素的自然對數(以e為基底),=" + logMat.dump());

        //inRange
        Mat inRangeMat = new Mat();
        Scalar lowerb = new Scalar(2);
        Scalar upperb = new Scalar(3);
        Core.inRange(m1, lowerb, upperb, inRangeMat);
        System.out.println("m1每個元素的是否在[2,3]區間,Yes=255,No=0:" +
                           inRangeMat.dump());

        //normalize
        Mat normalizeMat = new Mat();
        Core.normalize(m1, normalizeMat, 5, 1, Core.NORM_L1);
        System.out.println("m1正規化L1:" + normalizeMat.dump());

        Core.normalize(m1, normalizeMat, 2, 1, Core.NORM_L2);
        System.out.println("m1正規化L2:" + normalizeMat.dump());

        Core.normalize(m1, normalizeMat, 2, 1, Core.NORM_INF);
        System.out.println("m1正規化INF:" + normalizeMat.dump());

        Core.normalize(m1, normalizeMat, 6, 0, Core.NORM_MINMAX);
        System.out.println("m1正規化MINMAX:" + normalizeMat.dump());

        //sqrt
        Mat sqrtMat = new Mat();
        Core.sqrt(m1, sqrtMat);
        System.out.println("m1每個元素的平方根:" + sqrtMat.dump());
        //計算2D向量的常度及角度
        //{1,1},{3,4},{6,8},{10,10}
        Mat vector1 = new Mat(1, 4, CvType.CV_32FC1);
        vector1.put(0, 0, 1);
        vector1.put(0, 1, 3);
        vector1.put(0, 2, 6);
        vector1.put(0, 3, 10);


        Mat vector2 = new Mat(1, 4, CvType.CV_32FC1);
        vector2.put(0, 0, 1);
        vector2.put(0, 1, 4);
        vector2.put(0, 2, 8);
        vector2.put(0, 3, 10);

        Mat magnitude = new Mat();
        Mat angle = new Mat();

        Core.cartToPolar(vector1, vector2, magnitude, angle);
        System.out.println("矩陣cartToPolar運算,magnitude=" + magnitude.dump() +
                           ",angle=" + angle.dump());


        //可逆方程,求出原2D向量
        Mat findM1 = new Mat();
        Mat findM2 = new Mat();

        Core.polarToCart(magnitude, angle, findM1, findM2);

        System.out.println("求出原2D向量,矩陣polarToCart運算,v1所有元素=" +
                           findM1.dump() + ",v2=" + findM2.dump());

    }

}
