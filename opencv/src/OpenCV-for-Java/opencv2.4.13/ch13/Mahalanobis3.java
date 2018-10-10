package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;

public class Mahalanobis3
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {

        float[][] sampleData = {
            {1.0f, 2.0f},
            {1.0f, 3.0f},
            {2.0f, 2.0f},
            {3.0f, 1.0f}

        };//[4][2]

        Mat trainingDataMat = new Mat(4, 2, CvType.CV_32FC1);

        for (int i = 0; i < 4; i++) {
            trainingDataMat.put(i, 0, sampleData[i]);

        }

        System.out.println(trainingDataMat.dump());
        Mat covar = new Mat(12, 12, CvType.CV_32F);

        Mat mean = new Mat(1, 4, CvType.CV_32F);

        Core.calcCovarMatrix(trainingDataMat, covar, mean,
                             Core.COVAR_ROWS | Core.COVAR_NORMAL, CvType.CV_32F);
        System.out.println("covar=" + covar.dump());
        //covar = covar.inv();
        Core.divide(covar, Scalar.all(trainingDataMat.rows() - 1),
                    covar); //covar=covar/(trainingDataMat.rows()-1)
        Core.invert(covar, covar, Core.DECOMP_SVD);
        System.out.println("covar=" + covar.dump());
        System.out.println("mean=" + mean.dump());
        Mat line1 = trainingDataMat.row(0);
        System.out.println("line1=" + line1.dump());
        Mat line2 = trainingDataMat.row(1);
        Mat line3 = trainingDataMat.row(2);
        Mat line4 = trainingDataMat.row(3);
        //¤w½T»{¥¿½T
        double d = Core.Mahalanobis(line1, line2, covar);
        System.out.println("1,2-d2=" + d);
        d = Core.Mahalanobis(line1, line3, covar);
        System.out.println("1,3-d2=" + d);
        d = Core.Mahalanobis(line1, line4, covar);
        System.out.println("1,4-d2=" + d);
        d = Core.Mahalanobis(line2, line3, covar);
        System.out.println("2,3-d2=" + d);
        d = Core.Mahalanobis(line2, line4, covar);
        System.out.println("2,4-d2=" + d);
        d = Core.Mahalanobis(line3, line4, covar);
        System.out.println("3,4-d2=" + d);


    }

}
