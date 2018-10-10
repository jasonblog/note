package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.TermCriteria;
import org.opencv.highgui.Highgui;
import org.opencv.ml.CvANN_MLP;
import org.opencv.ml.CvRTParams;
import org.opencv.ml.CvRTrees;
import org.opencv.ml.CvSVM;
import org.opencv.ml.CvSVMParams;


public class Rtree
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {

        //train data
        float[] labels = {0.0f, 1.0f, 1.0f, 2.0f}; //[4]
        Mat labelsMat = new Mat(4, 1, CvType.CV_32FC1);
        labelsMat.put(0, 0, new float[] {0.0f, 1.0f, 1.0f, 2.0f});

        labelsMat.put(3, 0, labels[3]);
        //      labelsMat.put(0, 0, labels[0]);
        //      labelsMat.put(1, 0, labels[1]);
        //      labelsMat.put(2, 0, labels[2]);
        //      labelsMat.put(3, 0, labels[3]);

        //System.out.println(labelsMat.dump());

        float[][] trainingData = {{501, 10}, {255, 10}, {501, 255}, {10, 501}}; //[4][2]
        Mat trainingDataMat = new Mat(4, 2, CvType.CV_32FC1);
        trainingDataMat.put(0, 0, trainingData[0]);
        trainingDataMat.put(1, 0, trainingData[1]);
        trainingDataMat.put(2, 0, trainingData[2]);
        trainingDataMat.put(3, 0, trainingData[3]);
        //System.out.println(trainingDataMat.dump());

        CvRTParams  params = new CvRTParams();
        params.set_max_depth(25);
        params.set_min_sample_count(5);
        params.set_regression_accuracy(0);
        params.set_use_surrogates(false);
        params.set_max_categories(15);
        params.set_calc_var_importance(false);
        params.set_term_crit(new TermCriteria(TermCriteria.MAX_ITER | TermCriteria.EPS,
                                              100, 1E-6));



        CvRTrees rtree = new CvRTrees();
        int tflag = 1;

        //boolean r=rtree.train(trainingDataMat, tflag, labelsMat);
        boolean r = rtree.train(trainingDataMat, tflag, labelsMat, new Mat(), new Mat(),
                                new Mat(), new Mat(), params);
        System.out.println(r);

        float[] smp = {
            //121,122,23,124,125//-1
            //51,110//2
            //501,255//2
            //501,10//1
            //255,10
        };
        Mat sample = new Mat(1, 2, CvType.CV_32FC1);
        sample.put(0, 0, smp);
        System.out.println(sample.dump());
        Mat missing = new Mat();
        float result = rtree.predict(sample, missing);

        System.out.println(missing.dump());
        System.out.println(result);


    }
}
