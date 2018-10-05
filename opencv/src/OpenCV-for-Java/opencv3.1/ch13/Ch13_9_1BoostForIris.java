package ch13;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.TermCriteria;
import org.opencv.ml.Boost;
import org.opencv.ml.Ml;
import org.opencv.ml.RTrees;


//籔13_9_2ゑ耕(3じvs 2じ)
public class Ch13_9_1BoostForIris
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        IrisDatabase iris = new IrisDatabase();

        Boost boost = Boost.create();
        boost.setBoostType(Boost.DISCRETE);
        boost.setWeakCount(3);
        boost.setMinSampleCount(4);
        boost.setMaxCategories(4);
        boost.setMaxDepth(2);


        boolean r = boost.train(iris.getTrainingDataMat(), Ml.ROW_SAMPLE,
                                iris.getTrainingLabelsMat());
        System.out.println("琌Τ癡絤Θ=" + r);

        //繦獽т3舱代刚

        float result0 = boost.predict(iris.getTestSample0FrTestMat());
        System.out.println("Boost箇代result0=" + result0 + "摸");

        float result1 = boost.predict(iris.getTestSample1FrTestMat());
        System.out.println("Boost箇代result1=" + result1 + "摸");

        float result2 = boost.predict(iris.getTestSample2FrTestMat());
        System.out.println("Boost箇代result2=" + result2 + "摸");

        //代刚弘非

        //箇代タ絋仓1
        int right = 0;
        float result;
        float[] answer = iris.getTestingLabels();

        //System.out.println("舱计"=iris.getTestingDataMat().rows());
        for (int i = 0; i < iris.getTestingDataMat().rows(); i++) {

            //System.out.println(iris.getTestingDataMat().row(i).dump());
            result = boost.predict(iris.getTestingDataMat().row(i));

            if (result == answer[i]) {
                right++;
            } else {
                System.out.println("箇代岿粇!Boost箇代琌" + result + "タ絋琌=" + answer[i]);
            }

        }

        System.out.println("Boost代刚弘非=" + ((float)right /
                           (float)iris.getTestingDataMat().rows()) * 100 + "%");




    }

}
