package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.TermCriteria;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.ml.Ml;
import org.opencv.ml.NormalBayesClassifier;


public class Ch13_3_2BayesForOcr
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }

    public static void main(String[] args)
    {
        OcrDatabase ocr = new OcrDatabase();
        NormalBayesClassifier nBayes = NormalBayesClassifier.create();
        boolean r = nBayes.train(ocr.getTrainingDataMat(), Ml.ROW_SAMPLE,
                                 ocr.getTrainingLabelsMat());

        System.out.println("是否有訓練成功=" + r);

        //隨便找3組測試

        float result0 = nBayes.predict(ocr.getTestSample0FrTestMat());
        System.out.println("常態貝葉斯分類器預測result0=" + result0 + "類");

        float result1 = nBayes.predict(ocr.getTestSample1FrTestMat());
        System.out.println("常態貝葉斯分類器預測result1=" + result1 + "類");

        float result2 = nBayes.predict(ocr.getTestSample2FrTestMat());
        System.out.println("常態貝葉斯分類器預測result2=" + result2 + "類");

        //測試精準值

        //預測正確累加1
        int right = 0;
        float result;
        float[] answer = ocr.getTestingLabels();

        //System.out.println("組數"=iris.getTestingDataMat().rows());
        for (int i = 0; i < ocr.getTestingDataMat().rows(); i++) {

            //System.out.println(iris.getTestingDataMat().row(i).dump());
            result = nBayes.predict(ocr.getTestingDataMat().row(i));

            if (result == answer[i]) {
                right++;
            } else {
                System.out.println("預測錯誤!常態貝葉斯分類器預測是" + result + ",正確是=" +
                                   answer[i]);
            }

        }

        System.out.println("常態貝葉斯分類器測試精準值=" + ((float)right /
                           (float)ocr.getTestingDataMat().rows()) * 100 + "%");

        //again test 7
        result = nBayes.predict(ocr.sample7);
        System.out.println("預測7結果=" + result);
    }
}
