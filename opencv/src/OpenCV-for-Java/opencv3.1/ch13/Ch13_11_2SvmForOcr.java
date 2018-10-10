package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.TermCriteria;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.ml.Ml;
import org.opencv.ml.NormalBayesClassifier;
import org.opencv.ml.SVM;


public class Ch13_11_2SvmForOcr
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }

    public static void main(String[] args)
    {
        OcrDatabase ocr = new OcrDatabase();

        SVM svm = SVM.create();
        TermCriteria criteria = new TermCriteria(TermCriteria.MAX_ITER, 100, 1e-6);
        svm.setKernel(svm.LINEAR);
        svm.setType(svm.C_SVC);
        svm.setGamma(0.5);
        svm.setNu(0.5);
        //1-->97.77778%,2-->100%
        svm.setC(2);
        svm.setTermCriteria(criteria);

        boolean r = svm.train(ocr.getTrainingDataMat(), Ml.ROW_SAMPLE,
                              ocr.getTrainingLabelsMat());

        System.out.println("是否有訓練成功=" + r);

        //隨便找3組測試

        float result0 = svm.predict(ocr.getTestSample0FrTestMat());
        System.out.println("支援向量機預測result0=" + result0 + "類");

        float result1 = svm.predict(ocr.getTestSample1FrTestMat());
        System.out.println("支援向量機預測result1=" + result1 + "類");

        float result2 = svm.predict(ocr.getTestSample2FrTestMat());
        System.out.println("支援向量機預測result2=" + result2 + "類");

        //測試精準值

        //預測正確累加1
        int right = 0;
        float result;
        float[] answer = ocr.getTestingLabels();

        //System.out.println("組數"=iris.getTestingDataMat().rows());
        for (int i = 0; i < ocr.getTestingDataMat().rows(); i++) {

            //System.out.println(iris.getTestingDataMat().row(i).dump());
            result = svm.predict(ocr.getTestingDataMat().row(i));

            if (result == answer[i]) {
                right++;
            } else {
                System.out.println("預測錯誤!支援向量機預測是" + result + ",正確是=" +
                                   answer[i]);
            }

        }

        //100%
        System.out.println("支援向量機測試精準值=" + ((float)right /
                           (float)ocr.getTestingDataMat().rows()) * 100 + "%");

        //again test 7
        result = svm.predict(ocr.sample7);
        System.out.println("預測7結果=" + result);


    }
}
