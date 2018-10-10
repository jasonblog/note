package ch13;

import org.opencv.core.Core;
import org.opencv.core.Core.MinMaxLocResult;
import org.opencv.ml.ANN_MLP;
import org.opencv.ml.Ml;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.TermCriteria;

//官網有說明http://docs.opencv.org/3.0-beta/modules/ml/doc/neural_networks.html
public class Ch13_10_3AnnForOCR
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {

        OcrDatabase ocr = new OcrDatabase();

        ANN_MLP ann = ANN_MLP.create();

        Mat layerSize = new Mat(4, 1, CvType.CV_32SC1);


        int[] layerSizeAry = {144, 20, 10, 10};
        layerSize.put(0, 0, layerSizeAry[0]);
        layerSize.put(1, 0, layerSizeAry[1]);
        layerSize.put(2, 0, layerSizeAry[2]);
        layerSize.put(3, 0, layerSizeAry[3]);
        //      layerSize.put(0,4,layerSizeAry[4]);
        //System.out.println(layerSize.dump());


        ann.setLayerSizes(layerSize);
        ann.setTrainMethod(ann.BACKPROP);
        TermCriteria criteria = new TermCriteria(TermCriteria.MAX_ITER |
                TermCriteria.EPS, 300, 0.001);
        //ann.setTrainMethod(ann.RPROP);
        ann.setTermCriteria(criteria);//註解:97.77778%,open:100%
        //          ann.setBackpropMomentumScale(0.01);
        //          ann.setBackpropWeightScale(0.01);
        //          ann.setRpropDW0(0.1);
        //          ann.setRpropDWPlus(1.2);
        //          ann.setRpropDWMinus(0.5);
        //          ann.setRpropDWMin(1);
        //          ann.setRpropDWMax(50);

        ann.setActivationFunction(ann.SIGMOID_SYM);//要在set layer之後

        boolean r = ann.train(ocr.getTrainingDataMat(), Ml.ROW_SAMPLE,
                              ocr.getTrainingLabelsFloatMat());
        System.out.println("是否有訓練成功=" + r);
        Mat results = new Mat();



        //隨便找3組測試

        float result0 = ann.predict(ocr.getTestSample0FrTestMat());
        System.out.println("類神經網路預測result0=" + result0 + "類");

        float result1 = ann.predict(ocr.getTestSample1FrTestMat());
        System.out.println("類神經網路預測result1=" + result1 + "類");

        float result2 = ann.predict(ocr.getTestSample2FrTestMat());
        System.out.println("類神經網路預測result2=" + result2 + "類");


        //測試精準值

        //預測正確累加1
        int right = 0;
        MinMaxLocResult minMaxLocResult;
        float[] answer = ocr.getTestingLabels();

        //System.out.println("組數"=iris.getTestingDataMat().rows());
        for (int i = 0; i < ocr.getTestingDataMat().rows(); i++) {

            //System.out.println(iris.getTestingDataMat().row(i).dump());
            float result = ann.predict(ocr.getTestingDataMat().row(i));

            if (result == answer[i]) {
                right++;
            } else {
                System.out.println("預測錯誤!類神經網路預測是" + result + ",正確是=" +
                                   answer[i]);
            }

        }

        System.out.println("類神經網路測試精準值=" + ((float)right /
                           (float)ocr.getTestingDataMat().rows()) * 100 + "%");

        //again test 7
        float result7 = ann.predict(ocr.sample7);
        System.out.println("預測7結果=" + result7);
    }

}
