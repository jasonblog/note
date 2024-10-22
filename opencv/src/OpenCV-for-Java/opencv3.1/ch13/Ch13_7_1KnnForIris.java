package ch13;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.ml.KNearest;
import org.opencv.ml.Ml;

public class Ch13_7_1KnnForIris
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        IrisDatabase iris = new IrisDatabase();
        KNearest knn = KNearest.create();

        System.out.println("knn.BRUTE_FORCE=" + knn.BRUTE_FORCE);
        System.out.println("knn.KDTREE=" + knn.KDTREE);
        //1,3-->100%,2,5,6,7,8-->97.77,4-->95.5%,6
        knn.setDefaultK(8);
        knn.setIsClassifier(true);

        boolean r = knn.train(iris.getTrainingDataMat(), Ml.ROW_SAMPLE,
                              iris.getTrainingLabelsMat());
        System.out.println("琌Τ癡絤Θ=" + r);

        //繦獽т3舱代刚

        float result0 = knn.predict(iris.getTestSample0FrTestMat());
        System.out.println("KNN箇代result0=" + result0 + "摸");

        float result1 = knn.predict(iris.getTestSample1FrTestMat());
        System.out.println("KNN箇代result1=" + result1 + "摸");

        float result2 = knn.predict(iris.getTestSample2FrTestMat());
        System.out.println("KNN箇代result2=" + result2 + "摸");

        //代刚弘非

        //箇代タ絋仓1
        int right = 0;
        float result;
        float[] answer = iris.getTestingLabels();

        //System.out.println("舱计"=iris.getTestingDataMat().rows());
        for (int i = 0; i < iris.getTestingDataMat().rows(); i++) {

            //System.out.println(iris.getTestingDataMat().row(i).dump());
            result = knn.predict(iris.getTestingDataMat().row(i));

            if (result == answer[i]) {
                right++;
            } else {
                System.out.println("箇代岿粇!KNN箇代琌" + result + "タ絋琌=" + answer[i]);
            }

        }

        System.out.println("KNN代刚弘非=" + ((float)right / (float)
                                               iris.getTestingDataMat().rows()) * 100 + "%");




    }

}
