package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;

public class Ch13_13_2MahalanobisForIris
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }

    public static void main(String[] args)
    {

        float[][] sampleData = {
            {5.1f, 3.5f, 1.4f, 0.2f}, //Iris-setosa
            {7.0f, 3.2f, 4.7f, 1.4f}, //Iris-versicolor
            {6.3f, 3.3f, 6.0f, 2.5f}, //Iris-virginica
            //{4.9f,3.1f,1.5f,0.1f}//Iris-setosa
            {4.9f, 3.0f, 1.4f, 0.2f} //Iris-setosa
        };// [4][4]

        Mat trainingDataMat = new Mat(4, 4, CvType.CV_32FC1);

        for (int i = 0; i < 4; i++) {
            trainingDataMat.put(i, 0, sampleData[i]);

        }

        Mat covar = new Mat();
        Mat mean = new Mat(1, 4, CvType.CV_32F);

        Core.calcCovarMatrix(trainingDataMat, covar, mean,
                             Core.COVAR_ROWS | Core.COVAR_NORMAL, CvType.CV_32F);
        // System.out.println("covar="+covar.dump());
        Core.invert(covar, covar, Core.DECOMP_SVD);
        Mat m3 = new Mat(4, 4, CvType.CV_32FC1, new Scalar(3));
        //covar = covar.mul(m3);
        //System.out.println("covar=" + covar.dump() + ",size=" + covar.size());
        //System.out.println("mean=" + mean.dump());
        Mat line1 = trainingDataMat.row(0);
        //      // System.out.println("line1="+line1.dump());
        Mat line2 = trainingDataMat.row(1);
        Mat line3 = trainingDataMat.row(2);
        Mat line4 = trainingDataMat.row(3);
        double d = Core.Mahalanobis(line1, line2, covar);
        System.out.println("line1與line2的Mahalanobis距離=" + d);
        d = Core.Mahalanobis(line1, line3, covar);
        System.out.println("line1與line3的Mahalanobis距離=" + d);
        d = Core.Mahalanobis(line1, line4, covar);
        System.out.println("line1與line4的Mahalanobis距離=" + d);
        /*
          line1與line2的Mahalanobis距離=3.2287767893393204
          line1與line3的Mahalanobis距離=3.7121420750022134
          line1與line4的Mahalanobis距離=2.4454044106174178
          line1與line4最小,所以line1與line4同組

            若取消covar = covar.mul(m3);
            the same
            line1與line2的Mahalanobis距離=1.4970805109398873
            line1與line3的Mahalanobis距離=1.437010692549573
            line1與line4的Mahalanobis距離=1.4115597441161545

                若取消Core.invert(covar, covar, Core.DECOMP_SVD);
                    the same
                line1與line2的Mahalanobis距離=18.426742990130666
                line1與line3的Mahalanobis距離=24.67039908097456
                line1與line4的Mahalanobis距離=0.20346968682626132
         */
    }

}
