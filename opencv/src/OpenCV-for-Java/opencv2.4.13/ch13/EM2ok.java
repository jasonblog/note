package ch13;

import java.util.Arrays;
import java.util.Random;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Range;
import org.opencv.core.Scalar;
import org.opencv.core.TermCriteria;
import org.opencv.highgui.Highgui;

public class EM2ok
{
    public static void main(String[] args) throws Exception{
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);

        float[] labels = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}; //[30]
        Mat labelsMat = new Mat(30, 1, CvType.CV_32SC1);

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

        for (int i = 0; i < 30; i++)
        {
            labelsMat.put(i, 0, labels[i]);
            trainingDataMat.put(i, 0, trainingData[i]);

        }

        Mat logLikelihoods = new Mat();
        Mat probs = new Mat();

        org.opencv.ml.EM em = new org.opencv.ml.EM();
        boolean r = em.train(trainingDataMat, logLikelihoods, labelsMat, probs);
        System.out.println(r);
        //   System.out.println(logLikelihoods.dump());
        //   System.out.println( probs.dump());


        float[] smp = {
            // 6.7f,2.5f,5.8f,1.8f
            //4.9f,3.0f,1.4f,0.2f//get 1
            //4.6f,3.1f,1.5f,0.2f//get 1

            //6.7f,2.5f,5.8f,1.8f//get 4   -1=3
            //5.5f,2.3f,4.0f,1.3f//get 3    -1=2
            7.6f, 3.0f, 6.6f, 2.1f //get 4
        };
        Mat sample = new Mat(1, 4, CvType.CV_32FC1);
        sample.put(0, 0, smp);
        System.out.println(sample.dump());
        double[] result = em.predict(sample);

        System.out.println("result=" + Arrays.toString(result));


    }
}
