package ch13;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.TermCriteria;
import org.opencv.ml.Ml;
import org.opencv.ml.RTrees;
import org.opencv.ml.SVM;

public class Ch13_11_1SvmForIris
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        IrisDatabase iris = new IrisDatabase();


        SVM svm = SVM.create();
        TermCriteria criteria = new TermCriteria(TermCriteria.MAX_ITER, 100, 1e-6);
        svm.setKernel(svm.LINEAR);
        svm.setType(svm.C_SVC);
        svm.setGamma(0.5);
        svm.setNu(0.5);
        //1-->97.77778%,2-->100%
        svm.setC(2);
        svm.setTermCriteria(criteria);

        boolean r = svm.train(iris.getTrainingDataMat(), Ml.ROW_SAMPLE,
                              iris.getTrainingLabelsMat());
        System.out.println("是否有訓練成功=" + r);

        //隨便找3組測試

        float result0 = svm.predict(iris.getTestSample0FrTestMat());
        System.out.println("支援向量機預測result0=" + result0 + "類");

        float result1 = svm.predict(iris.getTestSample1FrTestMat());
        System.out.println("支援向量機預測result1=" + result1 + "類");

        float result2 = svm.predict(iris.getTestSample2FrTestMat());
        System.out.println("支援向量機預測result2=" + result2 + "類");

        //測試精準值

        //預測正確累加1
        int right = 0;
        float result;
        float[] answer = iris.getTestingLabels();

        //System.out.println("組數"=iris.getTestingDataMat().rows());
        for (int i = 0; i < iris.getTestingDataMat().rows(); i++) {

            //System.out.println(iris.getTestingDataMat().row(i).dump());
            result = svm.predict(iris.getTestingDataMat().row(i));

            if (result == answer[i]) {
                right++;
            } else {
                System.out.println("預測錯誤!支援向量機預測是" + result + "正確是=" +
                                   answer[i]);
            }

        }

        System.out.println("支援向量機測試精準值=" + ((float)right /
                           (float)iris.getTestingDataMat().rows()) * 100 + "%");




    }

}
