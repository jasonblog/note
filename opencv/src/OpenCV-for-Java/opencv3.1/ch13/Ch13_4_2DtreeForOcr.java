package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.TermCriteria;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.ml.DTrees;
import org.opencv.ml.Ml;
import org.opencv.ml.NormalBayesClassifier;


public class Ch13_4_2DtreeForOcr
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }

    public static void main(String[] args)
    {
        OcrDatabase ocr = new OcrDatabase();
        Mat dTreePriors = new Mat();
        DTrees dtree = DTrees.create(); //創建dtree
        //1->20%,2,3->30%,5,6,8,9->50%
        dtree.setMaxDepth(8);//設定最大深度
        dtree.setMinSampleCount(2);
        dtree.setRegressionAccuracy(0);
        dtree.setUseSurrogates(false);
        dtree.setMaxCategories(2);
        dtree.setPriors(dTreePriors);
        dtree.setUse1SERule(true);
        dtree.setCVFolds(
            0);//交叉驗證,//set 10,it will get Exception in thread "main" java.lang.Exception: unknown exception
        dtree.setTruncatePrunedTree(true);

        boolean r = dtree.train(ocr.getTrainingDataMat(), Ml.ROW_SAMPLE,
                                ocr.getTrainingLabelsMat());

        System.out.println("是否有訓練成功=" + r);

        //隨便找3組測試

        float result0 = dtree.predict(ocr.getTestSample0FrTestMat());
        System.out.println("決策樹預測result0=" + result0 + "類");

        float result1 = dtree.predict(ocr.getTestSample1FrTestMat());
        System.out.println("決策樹預測result1=" + result1 + "類");

        float result2 = dtree.predict(ocr.getTestSample2FrTestMat());
        System.out.println("決策樹預測result2=" + result2 + "類");

        //測試精準值

        //預測正確累加1
        int right = 0;
        float result;
        float[] answer = ocr.getTestingLabels();

        //System.out.println("組數"=iris.getTestingDataMat().rows());
        for (int i = 0; i < ocr.getTestingDataMat().rows(); i++) {

            //System.out.println(iris.getTestingDataMat().row(i).dump());
            result = dtree.predict(ocr.getTestingDataMat().row(i));

            if (result == answer[i]) {
                right++;
            } else {
                System.out.println("預測錯誤!決策樹預測是" + result + ",正確是=" + answer[i]);
            }

        }

        System.out.println("決策樹測試精準值=" + ((float)right /
                           (float)ocr.getTestingDataMat().rows()) * 100 + "%");

        //again test 7
        result = dtree.predict(ocr.sample7);
        System.out.println("預測7結果=" + result);


    }
}
