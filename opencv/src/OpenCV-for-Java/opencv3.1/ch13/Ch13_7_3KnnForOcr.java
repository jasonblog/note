package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.ml.KNearest;
import org.opencv.ml.Ml;


public class Ch13_7_3KnnForOcr
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }

    public static void main(String[] args)
    {
        OcrDatabase ocr = new OcrDatabase();
        Mat results = new Mat();
        Mat neighborResponses = new Mat();
        Mat dist = new Mat();

        KNearest knn = KNearest.create();
        //knn.setDefaultK(10);
        //knn.setIsClassifier(true);
        boolean r = knn.train(ocr.getTrainingDataMat(), Ml.ROW_SAMPLE,
                              ocr.getTrainingLabelsMat());


        System.out.println("琌Τ癡絤Θ=" + r);

        //繦獽т3舱代刚

        float result0 = knn.predict(ocr.getTestSample0FrTestMat());
        System.out.println("KNN箇代result0=" + result0 + "摸");

        float result1 = knn.predict(ocr.getTestSample1FrTestMat());
        System.out.println("KNN箇代result1=" + result1 + "摸");

        float result2 = knn.predict(ocr.getTestSample2FrTestMat());
        System.out.println("KNN箇代result2=" + result2 + "摸");

        //代刚弘非

        //箇代タ絋仓1
        int right = 0;
        float result;
        float[] answer = ocr.getTestingLabels();

        //System.out.println("舱计"=iris.getTestingDataMat().rows());
        for (int i = 0; i < ocr.getTestingDataMat().rows(); i++) {

            //System.out.println(iris.getTestingDataMat().row(i).dump());
            // result=knn.predict(ocr.getTestingDataMat().row(i));
            result = knn.findNearest(ocr.getTestingDataMat().row(i), 10, results,
                                     neighborResponses, dist);

            if (result == answer[i]) {
                right++;
            } else {
                System.out.println("箇代岿粇!KNN箇代琌" + result + ",タ絋琌=" + answer[i]);
            }

        }

        //100%
        System.out.println("KNN代刚弘非=" + ((float)right / (float)
                                               ocr.getTestingDataMat().rows()) * 100 + "%");

        //again test 7
        result = knn.predict(ocr.sample7);
        System.out.println("箇代7挡狦=" + result);
    }
}
