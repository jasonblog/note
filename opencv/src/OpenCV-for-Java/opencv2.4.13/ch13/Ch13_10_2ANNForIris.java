package ch13;

import org.opencv.core.Core;
import org.opencv.core.Core.MinMaxLocResult;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.ml.CvANN_MLP;

//官網有說明http://docs.opencv.org/3.0-beta/modules/ml/doc/neural_networks.html
public class Ch13_10_2ANNForIris
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {

        IrisDatabase iris = new IrisDatabase();

        CvANN_MLP ann = new CvANN_MLP();
        //ann.setTrainMethod(ANN_MLP.BACKPROP, 0.1, 0.1);



        //      System.out.println(trainingDataMat.get(20,0)[0]);
        //      System.out.println(trainingDataMat.get(20,1)[0]);


        Mat layerSize = new Mat(3, 1, CvType.CV_32SC1);
        int[] layerSizeAry = {4, 10, 3};
        //layerSize.put(0, 0, layerSizeAry);
        layerSize.put(0, 0, layerSizeAry[0]);
        layerSize.put(1, 0, layerSizeAry[1]);
        layerSize.put(2, 0, layerSizeAry[2]);
        //      layerSize.put(0,3,layerSizeAry[3]);
        //      layerSize.put(0,4,layerSizeAry[4]);
        //System.out.println(layerSize.dump());


        ann.create(layerSize);
        Mat sampleWeights = new Mat();

        int r = ann.train(iris.getTrainingDataMat(), iris.getTrainingLabelsMatForAnn(),
                          sampleWeights);
        System.out.println("train return=" + r);
        Mat results = new Mat();



        //隨便找3組測試

        float result0 = ann.predict(iris.getTestSample0FrTestMat(), results);
        System.out.println("result0=" + result0);
        System.out.println("results=" + results.dump());
        MinMaxLocResult minMaxLocResult0 = Core.minMaxLoc(results);
        System.out.println("類神經網路預測=" + minMaxLocResult0.maxLoc.x);
        System.out.println("=============================================");

        float result1 = ann.predict(iris.getTestSample1FrTestMat(), results);
        System.out.println("result1=" + result1);
        System.out.println("results=" + results.dump());
        MinMaxLocResult minMaxLocResult1 = Core.minMaxLoc(results);
        System.out.println("類神經網路預測=" + minMaxLocResult1.maxLoc.x);
        System.out.println("=============================================");

        float result2 = ann.predict(iris.getTestSample2FrTestMat(), results);
        System.out.println("result2=" + result2);
        System.out.println("results=" + results.dump());
        MinMaxLocResult minMaxLocResult2 = Core.minMaxLoc(results);
        System.out.println("類神經網路預測=" + minMaxLocResult2.maxLoc.x);
        System.out.println("=============================================");


        //測試精準值

        //預測正確累加1
        int right = 0;
        MinMaxLocResult minMaxLocResult;
        float[] answer = iris.getTestingLabels();

        //System.out.println("組數"=iris.getTestingDataMat().rows());
        for (int i = 0; i < iris.getTestingDataMat().rows(); i++) {

            //System.out.println(iris.getTestingDataMat().row(i).dump());
            ann.predict(iris.getTestingDataMat().row(i), results);
            minMaxLocResult = Core.minMaxLoc(results);

            if (minMaxLocResult.maxLoc.x == answer[i]) {
                right++;
            } else {
                System.out.println("預測錯誤!類神經網路預測是" + minMaxLocResult.maxLoc.x +
                                   "正確是=" + answer[i]);
            }

        }

        System.out.println("類神經網路測試精準值=" + ((float)right /
                           (float)iris.getTestingDataMat().rows()) * 100 + "%");

    }

}
