package ch03;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;

public class Ch03_1_3LinearAlgebra
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
        System.out.println("痻皚m2trace(格)=" + Core.trace(m2).val[0]);

        //―eigenvalues,eigenvectors
        Mat eigenvalues = new Mat();
        Mat eigenvectors = new Mat();
        //boolean computeEigenvectors = true;
        Core.eigen(m1, eigenvalues, eigenvectors);
        System.out.println("m1疭紉=" + eigenvalues.dump());
        System.out.println("m1疭紉秖=" + eigenvectors.dump());

        //よ畉/跑计痻皚(covariance matrix)
        Mat covar = new Mat(2, 2, CvType.CV_32FC1);
        Mat mean = new Mat(1, 2, CvType.CV_32F);
        Core.calcCovarMatrix(m2, covar, mean, Core.COVAR_ROWS | Core.COVAR_NORMAL,
                             CvType.CV_32F);
        System.out.println("m2よ畉/跑计痻皚=" + covar.dump() + "by colキА" +
                           mean.dump());

        //ㄢㄢゑ耕
        Mat compare = new Mat();
        Mat m4 = new Mat(2, 2, CvType.CV_32FC1, new Scalar(9));
        Core.compare(m1, m4, compare, Core.CMP_GT);//琌
        System.out.println("m1琌m4じ:" + compare.dump());

        compare = new Mat();
        Core.compare(m1, m4, compare, Core.CMP_LT);//琌
        System.out.println("m1琌m4じ:" + compare.dump());

        compare = new Mat();
        Core.compare(m1, m4, compare, Core.CMP_EQ);//琌单
        System.out.println("m1琌单m4じ:" + compare.dump());

        compare = new Mat();
        Core.compare(m1, m4, compare, Core.CMP_GT);//琌单
        System.out.println("m1琌单m4じ:" + compare.dump());

        compare = new Mat();
        Core.compare(m1, m4, compare, Core.CMP_LT);//琌单
        System.out.println("m1琌单m4じ:" + compare.dump());
    }

}
