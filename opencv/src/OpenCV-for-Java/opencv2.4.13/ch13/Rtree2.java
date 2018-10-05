package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.core.TermCriteria;
import org.opencv.highgui.Highgui;
import org.opencv.ml.CvANN_MLP;
import org.opencv.ml.CvRTParams;
import org.opencv.ml.CvRTrees;
import org.opencv.ml.CvSVM;
import org.opencv.ml.CvSVMParams;

//ok,¦³¤ÀÃþ
public class Rtree2
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {

        Mat data = new Mat(6, 3, CvType.CV_32FC1, new Scalar(0));
        data.put(0, 0, new float[] {1.69f, 1, 0});
        data.put(1, 0, new float[] {1.76f, 0, 0});
        data.put(2, 0, new float[] {1.80f, 0, 0});
        data.put(3, 0, new float[] {1.82f, 1, 1});
        data.put(4, 0, new float[] {1.83f, 2, 2});
        data.put(5, 0, new float[] {2.83f, 4, 1});

        Mat responses = new Mat(6, 1, CvType.CV_32SC1, new Scalar(0));
        responses.put(0, 0, new int[] {0, 0, 0, 1, 2, 2});

        CvRTParams  params = new CvRTParams();
        params.set_max_depth(25);
        params.set_min_sample_count(5);
        params.set_regression_accuracy(0);
        params.set_use_surrogates(false);
        params.set_max_categories(15);
        params.set_calc_var_importance(false);
        params.set_term_crit(new TermCriteria(TermCriteria.MAX_ITER | TermCriteria.EPS,
                                              100, 1E-6));


        CvRTrees tree = new CvRTrees();
        // boolean r=tree.train(data, 1, responses);
        boolean r = tree.train(data, 1, responses, new Mat(), new Mat(), new Mat(),
                               new Mat(), params);
        System.out.println(r);
        float[] smp = {
            //121,122,23,124,125//-1
            //1.85f,2,2//2
            //1.77f,1,1//0
            //5.3f,2,5//2
            //1.80f, 0, 0//0
            1.8f, 1, 1//2
        };
        Mat sample = new Mat(1, 3, CvType.CV_32FC1);
        sample.put(0, 0, smp);
        System.out.println(sample.dump());
        Mat missing = new Mat();
        float result = tree.predict(sample, missing);
        System.out.println(missing.dump());
        System.out.println(result);

    }
}
