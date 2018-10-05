package ch13;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.TermCriteria;
import org.opencv.ml.Ml;
import org.opencv.ml.RTrees;

public class Ch13_8_1RandomForestForIris
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        IrisDatabase iris = new IrisDatabase();

        Mat rTreePriors = new Mat();
        TermCriteria criteria = new TermCriteria(TermCriteria.EPS +
                TermCriteria.MAX_ITER, 50, 0.1);

        RTrees rtree = RTrees.create();
        rtree.setMaxDepth(4);
        rtree.setMinSampleCount(2);
        rtree.setRegressionAccuracy(0);
        rtree.setUseSurrogates(false);
        rtree.setMaxCategories(2);
        rtree.setPriors(rTreePriors);
        rtree.setCalculateVarImportance(false);
        rtree.setActiveVarCount(10);
        rtree.setTermCriteria(criteria);


        boolean r = rtree.train(iris.getTrainingDataMat(), Ml.ROW_SAMPLE,
                                iris.getTrainingLabelsMat());
        System.out.println("琌Τ癡絤Θ=" + r);

        //繦獽т3舱代刚

        float result0 = rtree.predict(iris.getTestSample0FrTestMat());
        System.out.println("繦诀此狶箇代result0=" + result0 + "摸");

        float result1 = rtree.predict(iris.getTestSample1FrTestMat());
        System.out.println("繦诀此狶箇代result1=" + result1 + "摸");

        float result2 = rtree.predict(iris.getTestSample2FrTestMat());
        System.out.println("繦诀此狶箇代result2=" + result2 + "摸");

        //代刚弘非

        //箇代タ絋仓1
        int right = 0;
        float result;
        float[] answer = iris.getTestingLabels();

        //System.out.println("舱计"=iris.getTestingDataMat().rows());
        for (int i = 0; i < iris.getTestingDataMat().rows(); i++) {

            //System.out.println(iris.getTestingDataMat().row(i).dump());
            result = rtree.predict(iris.getTestingDataMat().row(i));

            if (result == answer[i]) {
                right++;
            } else {
                System.out.println("箇代岿粇!繦诀此狶箇代琌" + result + "タ絋琌=" + answer[i]);
            }

        }

        System.out.println("繦诀此狶代刚弘非=" + ((float)right /
                           (float)iris.getTestingDataMat().rows()) * 100 + "%");




    }

}
