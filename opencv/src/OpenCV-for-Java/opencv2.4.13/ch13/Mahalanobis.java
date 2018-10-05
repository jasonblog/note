package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;

public class Mahalanobis
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {

        int matSize = 10;
        Mat grayRnd_32f = new Mat(matSize, matSize, CvType.CV_32F);
        Core.randu(grayRnd_32f, 0, 256);
        Mat covar = new Mat(matSize, matSize, CvType.CV_32F);
        Mat mean = new Mat(1, matSize, CvType.CV_32F);
        Core.calcCovarMatrix(grayRnd_32f, covar, mean,
                             Core.COVAR_ROWS | Core.COVAR_NORMAL, CvType.CV_32F);
        //covar = covar.inv();¿ù»~
        Core.divide(covar, Scalar.all(grayRnd_32f.rows() - 1),
                    covar); //covar=covar/(trainingDataMat.rows()-1)
        Core.invert(covar, covar, Core.DECOMP_SVD);
        System.out.println(grayRnd_32f.dump());
        System.out.println(covar.dump());
        Mat line1 = grayRnd_32f.row(0);
        Mat line2 = grayRnd_32f.row(1);
        Mat line3 = grayRnd_32f.row(2);

        double d = Core.Mahalanobis(line1, line1, covar);
        System.out.println("d1=" + d);
        d = Core.Mahalanobis(line1, line2, covar);
        System.out.println("d2=" + d);
        d = Core.Mahalanobis(line2, line1, covar);
        System.out.println("d2=" + d);
        d = Core.Mahalanobis(line1, line3, covar);
        System.out.println("d2=" + d);
        d = Core.Mahalanobis(mean, line1, covar);
        System.out.println("d2=" + d);
        d = Core.Mahalanobis(mean, mean, covar);
        System.out.println("d2=" + d);

    }

}
