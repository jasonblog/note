package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.TermCriteria;
import org.opencv.highgui.Highgui;
import org.opencv.ml.CvANN_MLP;
import org.opencv.ml.CvNormalBayesClassifier;
import org.opencv.ml.CvSVM;
import org.opencv.ml.CvSVMParams;


public class Bayes2ok
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {

        //train data,Iris-setosa=0,Iris-versicolor=1,Iris-virginica=2
        float[] labels = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}; //[30]
        Mat labelsMat = new Mat(30, 1, CvType.CV_32FC1);

        //      for(int i=0;i<30;i++){
        //          labelsMat.put(i, 0, labels[i]);
        //      }

        //System.out.println(labelsMat.dump());

        float[][] trainingData = {{5.1f, 3.5f, 1.4f, 0.2f},
            {4.9f, 3.0f, 1.4f, 0.2f},
            {4.7f, 3.2f, 1.3f, 0.2f},
            {4.6f, 3.1f, 1.5f, 0.2f},
            {5.0f, 3.6f, 1.4f, 0.2f},
            {5.4f, 3.9f, 1.7f, 0.4f},
            {4.6f, 3.4f, 1.4f, 0.3f},
            {5.0f, 3.4f, 1.5f, 0.2f},
            {4.4f, 2.9f, 1.4f, 0.2f},
            {4.9f, 3.1f, 1.5f, 0.1f},
            {7.0f, 3.2f, 4.7f, 1.4f},
            {6.4f, 3.2f, 4.5f, 1.5f},
            {6.9f, 3.1f, 4.9f, 1.5f},
            {5.5f, 2.3f, 4.0f, 1.3f},
            {6.5f, 2.8f, 4.6f, 1.5f},
            {5.7f, 2.8f, 4.5f, 1.3f},
            {6.3f, 3.3f, 4.7f, 1.6f},
            {4.9f, 2.4f, 3.3f, 1.0f},
            {6.6f, 2.9f, 4.6f, 1.3f},
            {5.2f, 2.7f, 3.9f, 1.4f},
            {6.3f, 3.3f, 6.0f, 2.5f},
            {5.8f, 2.7f, 5.1f, 1.9f},
            {7.1f, 3.0f, 5.9f, 2.1f},
            {6.3f, 2.9f, 5.6f, 1.8f},
            {6.5f, 3.0f, 5.8f, 2.2f},
            {7.6f, 3.0f, 6.6f, 2.1f},
            {4.9f, 2.5f, 4.5f, 1.7f},
            {7.3f, 2.9f, 6.3f, 1.8f},
            {6.7f, 2.5f, 5.8f, 1.8f},
            {7.2f, 3.6f, 6.1f, 2.5f}

        };//[30][4]
        Mat trainingDataMat = new Mat(30, 4, CvType.CV_32FC1);


        for (int i = 0; i < 30; i++) {
            labelsMat.put(i, 0, labels[i]);
            trainingDataMat.put(i, 0, trainingData[i]);

        }

        //System.out.println(trainingDataMat.dump());

        CvNormalBayesClassifier nBayes = new CvNormalBayesClassifier();




        Mat varIdx = new Mat();
        Mat sampleIdx = new Mat();
        nBayes.train(trainingDataMat, labelsMat, varIdx, sampleIdx, false);


        //      System.out.println("varIdx="+varIdx.dump());
        //      System.out.println("sampleIdx="+sampleIdx.dump());

        //ÀH«K§ä3²Õ
        Mat sample1 = new Mat(1, 4, CvType.CV_32FC1);
        float[] smp1 = {
            5.7f, 2.5f, 5.0f, 2.0f
        };
        sample1.put(0, 0, smp1);

        Mat sample2 = new Mat(1, 4, CvType.CV_32FC1);
        float[] smp2 = {
            5.1f, 2.5f, 3.0f, 1.1f
        };
        sample2.put(0, 0, smp2);


        Mat sample3 = new Mat(1, 4, CvType.CV_32FC1);
        float[] smp3 = {
            5.1f, 3.8f, 1.9f, 0.4f
        };
        sample3.put(0, 0, smp3);



        //float result=ann.predict(sample, results, flags);
        float result = nBayes.predict(sample1);
        //System.out.println("result="+result+",("+i+","+j+")");
        System.out.println("result=" + result);
        result = nBayes.predict(sample2);
        System.out.println("result=" + result);
        result = nBayes.predict(sample3);
        System.out.println("result=" + result);

        //System.out.println("c="+c);







    }
}
