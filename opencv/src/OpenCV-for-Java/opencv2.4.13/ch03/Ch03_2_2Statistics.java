package ch03;

import org.opencv.core.Core;
import org.opencv.core.Core.MinMaxLocResult;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfDouble;
import org.opencv.core.Scalar;

public class Ch03_2_2Statistics
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

        System.out.println("痻皚m2琌狡籹m1,┮Τじ=" + m2.dump());

        Mat m3 = new Mat();
        Mat m4  = new Mat(2, 2, CvType.CV_32FC1, new Scalar(9));
        Core.max(m2, m4, m3);
        System.out.println("痻皚m2籔m4 Max=" + m3.dump());

        Mat m5 = new Mat();
        Core.min(m2, m4, m5);
        System.out.println("痻皚m2籔m4 Min=" + m5.dump());

        System.out.println("痻皚m2キА=" + Core.mean(m2));
        System.out.println("痻皚m4キА=" + Core.mean(m4));

        MatOfDouble mean = new MatOfDouble();
        MatOfDouble stddev = new MatOfDouble();

        Core.meanStdDev(m2, mean, stddev);
        System.out.println("痻皚m2キА=" + mean.get(0, 0)[0]);
        System.out.println("痻皚m2夹非畉=" + stddev.get(0, 0)[0]);
        System.out.println("痻皚m2羆㎝=" + Core.sumElems(m2).val[0]);
        System.out.println("痻皚m2獶箂じ计=" + Core.countNonZero(m1));

        MinMaxLocResult m6 = new MinMaxLocResult();
        m6 = Core.minMaxLoc(m2);
        System.out.println("痻皚m2ず程=" + m6.maxVal + ",程=" + m6.minVal);

        double norm = Core.norm(m1);
        System.out.println("痻皚m2膀セ絛计=" + norm);

        //Аっだガ
        Mat uniformlyDist = new Mat(3, 3, CvType.CV_32FC1);
        Core.randu(uniformlyDist, 100, 150);
        System.out.println("ミ3x3Аっだガ繦诀(100~150)痻皚=" +
                           uniformlyDist.dump());


        //盽篈だガ
        Mat normallyDist = new Mat(3, 3, CvType.CV_32FC1);
        Core.randn(normallyDist, 10, 7.5);
        System.out.println("ミ3x3盽篈だガ(キА=10,夹非畉=7.5)痻皚=" +
                           normallyDist.dump());



    }

}
