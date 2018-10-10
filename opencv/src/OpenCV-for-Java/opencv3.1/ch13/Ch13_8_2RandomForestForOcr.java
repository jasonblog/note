package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.TermCriteria;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.ml.DTrees;
import org.opencv.ml.Ml;
import org.opencv.ml.NormalBayesClassifier;
import org.opencv.ml.RTrees;


public class Ch13_8_2RandomForestForOcr
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }

    public static void main(String[] args)
    {
        OcrDatabase ocr = new OcrDatabase();
        Mat rTreePriors = new Mat();
        TermCriteria criteria = new TermCriteria(TermCriteria.EPS +
                TermCriteria.MAX_ITER, 50, 0.1);

        RTrees rtree = RTrees.create();
        //4-->60%,5-->90%,6-->70%,8,9-->50%,
        rtree.setMaxDepth(5);
        //3,4-->70%,2-->90%
        rtree.setMinSampleCount(2);
        rtree.setRegressionAccuracy(0);
        rtree.setUseSurrogates(false);
        rtree.setMaxCategories(2);
        rtree.setPriors(rTreePriors);
        rtree.setCalculateVarImportance(false);
        //8,10-->90%,9,11,20,30-->70%,12,40-->80,5,50-->70%,7-->60%
        //盢把计砞﹚倒onlineOCR versionㄏノ
        rtree.setActiveVarCount(10);
        rtree.setTermCriteria(criteria);


        boolean r = rtree.train(ocr.getTrainingDataMat(), Ml.ROW_SAMPLE,
                                ocr.getTrainingLabelsMat());

        System.out.println("琌Τ癡絤Θ=" + r);

        //繦獽т3舱代刚

        float result0 = rtree.predict(ocr.getTestSample0FrTestMat());
        System.out.println("繦诀此狶箇代result0=" + result0 + "摸");

        float result1 = rtree.predict(ocr.getTestSample1FrTestMat());
        System.out.println("繦诀此狶箇代result1=" + result1 + "摸");

        float result2 = rtree.predict(ocr.getTestSample2FrTestMat());
        System.out.println("繦诀此狶箇代result2=" + result2 + "摸");

        //代刚弘非

        //箇代タ絋仓1
        int right = 0;
        float result;
        float[] answer = ocr.getTestingLabels();

        //System.out.println("舱计"=iris.getTestingDataMat().rows());
        for (int i = 0; i < ocr.getTestingDataMat().rows(); i++) {

            //System.out.println(iris.getTestingDataMat().row(i).dump());
            result = rtree.predict(ocr.getTestingDataMat().row(i));

            if (result == answer[i]) {
                right++;
            } else {
                System.out.println("箇代岿粇!繦诀此狶箇代琌" + result + ",タ絋琌=" + answer[i]);
            }

        }

        System.out.println("繦诀此狶代刚弘非=" + ((float)right /
                           (float)ocr.getTestingDataMat().rows()) * 100 + "%");

        //again test 7
        result = rtree.predict(ocr.sample7);
        System.out.println("箇代7挡狦=" + result);


    }
}
