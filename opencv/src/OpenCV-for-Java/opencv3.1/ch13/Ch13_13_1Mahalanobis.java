package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;

public class Ch13_13_1Mahalanobis
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }

    public static void main(String[] args)
    {

        float[][] sampleData = { { 1.0f, 2.0f }, { 1.0f, 3.0f },
            { 2.0f, 2.0f }, { 3.0f, 1.0f }

        };// [4][2]

        Mat trainingDataMat = new Mat(4, 2, CvType.CV_32FC1);

        for (int i = 0; i < 4; i++) {
            trainingDataMat.put(i, 0, sampleData[i]);

        }

        // System.out.println(trainingDataMat.dump());
        // Mat covar = new Mat(2, 2, CvType.CV_32F);
        Mat covar = new Mat();
        Mat mean = new Mat(1, 4, CvType.CV_32F);

        Core.calcCovarMatrix(trainingDataMat, covar, mean, Core.COVAR_ROWS
                             | Core.COVAR_NORMAL, CvType.CV_32F);
        // System.out.println("covar="+covar.dump());
        Core.invert(covar, covar, Core.DECOMP_SVD);
        Mat m3 = new Mat(2, 2, CvType.CV_32FC1, new Scalar(3));
        covar = covar.mul(m3);
        System.out.println("covar=" + covar.dump() + ",size=" + covar.size());
        System.out.println("mean=" + mean.dump());
        Mat line1 = trainingDataMat.row(0);
        // System.out.println("line1="+line1.dump());
        Mat line2 = trainingDataMat.row(1);
        Mat line3 = trainingDataMat.row(2);
        Mat line4 = trainingDataMat.row(3);
        double d = Core.Mahalanobis(line1, line2, covar);
        System.out.println("line1與line2的Mahalanobis距離=" + d);
        d = Core.Mahalanobis(line1, line3, covar);
        System.out.println("line1與line3的Mahalanobis距離=" + d);
    }

}
