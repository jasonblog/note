package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.core.TermCriteria;
import org.opencv.ml.DTrees;
import org.opencv.ml.EM;
import org.opencv.ml.Ml;

public class Ch13_5_1EmForIris
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        IrisDatabase iris = new IrisDatabase();





        EM em = EM.create();

        System.out.println("COV_MAT_DEFAULT=" + em.COV_MAT_DEFAULT);
        System.out.println("COV_MAT_DIAGONAL=" + em.COV_MAT_DIAGONAL);
        System.out.println("COV_MAT_GENERIC=" + em.COV_MAT_GENERIC);
        System.out.println("COV_MAT_SPHERICAL=" + em.COV_MAT_SPHERICAL);



        //2-->66.66667%,3-->42.22222%,4-->44.444447%,5-->2.2222223%
        em.setClustersNumber(
            3);//1->OpenCV Error: Assertion failed (nclusters > 1) in cv::ml::EMImpl::setClustersNumber,
        em.setCovarianceMatrixType(
            2);//// (covMatType == COV_MAT_SPHERICAL || covMatType == COV_MAT_DIAGONAL || covMatType == COV_MAT_GENERIC)
        TermCriteria criteria = new TermCriteria(TermCriteria.EPS +
                TermCriteria.MAX_ITER, 300, 0.1);
        em.setTermCriteria(criteria);
        Mat logLikelihoods = new Mat();
        Mat probs = new Mat();
        boolean r = em.train(iris.getTrainingDataMat(), Ml.ROW_SAMPLE,
                             iris.getTrainingLabelsMat());
        System.out.println("是否有訓練成功=" + r);

        //隨便找3組測試

        float result0 = em.predict(iris.getTestSample0FrTestMat());
        System.out.println("最大期望值預測result0=" + result0 + "類");

        float result1 = em.predict(iris.getTestSample1FrTestMat());
        System.out.println("最大期望值預測result1=" + result1 + "類");

        float result2 = em.predict(iris.getTestSample2FrTestMat());
        System.out.println("最大期望值預測result2=" + result2 + "類");

        //測試精準值

        //預測正確累加1
        int right = 0;
        float result;
        float[] answer = iris.getTestingLabels();

        //System.out.println("組數"=iris.getTestingDataMat().rows());
        for (int i = 0; i < iris.getTestingDataMat().rows(); i++) {

            //System.out.println(iris.getTestingDataMat().row(i).dump());
            result = em.predict(iris.getTestingDataMat().row(i));

            if (result == answer[i]) {
                right++;
            } else {
                System.out.println("預測錯誤!最大期望值預測是" + result + "正確是=" +
                                   answer[i]);
            }

        }

        System.out.println("最大期望值測試精準值=" + ((float)right /
                           (float)iris.getTestingDataMat().rows()) * 100 + "%");




    }

}
