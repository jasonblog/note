package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.core.TermCriteria;
import org.opencv.ml.Boost;
import org.opencv.ml.Ml;
import org.opencv.ml.RTrees;

public class Ch13_9_2BoostForIris2Class
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        float[][] trainingData = {
            //Iris-setosa,35組
            {5.1f, 3.5f, 1.4f, 0.2f},
            {4.9f, 3.0f, 1.4f, 0.2f},
            {4.7f, 3.2f, 1.3f, 0.2f},
            {4.6f, 3.1f, 1.5f, 0.2f},
            {5.0f, 3.6f, 1.4f, 0.2f},
            {5.4f, 3.9f, 1.7f, 0.4f},
            {4.6f, 3.4f, 1.4f, 0.3f},
            {5.0f, 3.4f, 1.5f, 0.2f},
            {4.4f, 2.9f, 1.4f, 0.2f},
            {4.9f, 3.1f, 1.5f, 0.1f},
            {5.4f, 3.7f, 1.5f, 0.2f},
            {4.8f, 3.4f, 1.6f, 0.2f},
            {4.8f, 3.0f, 1.4f, 0.1f},
            {4.3f, 3.0f, 1.1f, 0.1f},
            {5.8f, 4.0f, 1.2f, 0.2f},
            {5.7f, 4.4f, 1.5f, 0.4f},
            {5.4f, 3.9f, 1.3f, 0.4f},
            {5.1f, 3.5f, 1.4f, 0.3f},
            {5.7f, 3.8f, 1.7f, 0.3f},
            {5.1f, 3.8f, 1.5f, 0.3f},
            {5.4f, 3.4f, 1.7f, 0.2f},
            {5.1f, 3.7f, 1.5f, 0.4f},
            {4.6f, 3.6f, 1.0f, 0.2f},
            {5.1f, 3.3f, 1.7f, 0.5f},
            {4.8f, 3.4f, 1.9f, 0.2f},
            {5.0f, 3.0f, 1.6f, 0.2f},
            {5.0f, 3.4f, 1.6f, 0.4f},
            {5.2f, 3.5f, 1.5f, 0.2f},
            {5.2f, 3.4f, 1.4f, 0.2f},
            {4.7f, 3.2f, 1.6f, 0.2f},
            {4.8f, 3.1f, 1.6f, 0.2f},
            {5.4f, 3.4f, 1.5f, 0.4f},
            {5.2f, 4.1f, 1.5f, 0.1f},
            {5.5f, 4.2f, 1.4f, 0.2f},
            {4.9f, 3.1f, 1.5f, 0.1f},
            //Iris-versicolor,35組
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
            {5.0f, 2.0f, 3.5f, 1.0f},
            {5.9f, 3.0f, 4.2f, 1.5f},
            {6.0f, 2.2f, 4.0f, 1.0f},
            {6.1f, 2.9f, 4.7f, 1.4f},
            {5.6f, 2.9f, 3.6f, 1.3f},
            {6.7f, 3.1f, 4.4f, 1.4f},
            {5.6f, 3.0f, 4.5f, 1.5f},
            {5.8f, 2.7f, 4.1f, 1.0f},
            {6.2f, 2.2f, 4.5f, 1.5f},
            {5.6f, 2.5f, 3.9f, 1.1f},
            {5.9f, 3.2f, 4.8f, 1.8f},
            {6.1f, 2.8f, 4.0f, 1.3f},
            {6.3f, 2.5f, 4.9f, 1.5f},
            {6.1f, 2.8f, 4.7f, 1.2f},
            {6.4f, 2.9f, 4.3f, 1.3f},
            {6.6f, 3.0f, 4.4f, 1.4f},
            {6.8f, 2.8f, 4.8f, 1.4f},
            {6.7f, 3.0f, 5.0f, 1.7f},
            {6.0f, 2.9f, 4.5f, 1.5f},
            {5.7f, 2.6f, 3.5f, 1.0f},
            {5.5f, 2.4f, 3.8f, 1.1f},
            {5.5f, 2.4f, 3.7f, 1.0f},
            {5.8f, 2.7f, 3.9f, 1.2f},
            {6.0f, 2.7f, 5.1f, 1.6f},
            {5.4f, 3.0f, 4.5f, 1.5f}
        };// [70][4]

        Mat trainingDataMat = new Mat(70, 4, CvType.CV_32FC1);

        for (int i = 0; i < 70; i++) {
            trainingDataMat.put(i, 0, trainingData[i]);

        }

        float[][] testingData = {
            // Iris-setosa,15組
            { 5.0f, 3.2f, 1.2f, 0.2f }, { 5.5f, 3.5f, 1.3f, 0.2f },
            { 4.9f, 3.1f, 1.5f, 0.1f },
            { 4.4f, 3.0f, 1.3f, 0.2f },
            { 5.1f, 3.4f, 1.5f, 0.2f },
            { 5.0f, 3.5f, 1.3f, 0.3f },
            { 4.5f, 2.3f, 1.3f, 0.3f },
            { 4.4f, 3.2f, 1.3f, 0.2f },
            { 5.0f, 3.5f, 1.6f, 0.6f },
            { 5.1f, 3.8f, 1.9f, 0.4f },
            { 4.8f, 3.0f, 1.4f, 0.3f },
            { 5.1f, 3.8f, 1.6f, 0.2f },
            { 4.6f, 3.2f, 1.4f, 0.2f },
            { 5.3f, 3.7f, 1.5f, 0.2f },
            { 5.0f, 3.3f, 1.4f, 0.2f },
            // Iris-versicolor,15組
            { 6.0f, 3.4f, 4.5f, 1.6f }, { 6.7f, 3.1f, 4.7f, 1.5f },
            { 6.3f, 2.3f, 4.4f, 1.3f }, { 5.6f, 3.0f, 4.1f, 1.3f },
            { 5.5f, 2.5f, 4.0f, 1.3f }, { 5.5f, 2.6f, 4.4f, 1.2f },
            { 6.1f, 3.0f, 4.6f, 1.4f }, { 5.8f, 2.6f, 4.0f, 1.2f },
            { 5.0f, 2.3f, 3.3f, 1.0f }, { 5.6f, 2.7f, 4.2f, 1.3f },
            { 5.7f, 3.0f, 4.2f, 1.2f }, { 5.7f, 2.9f, 4.2f, 1.3f },
            { 6.2f, 2.9f, 4.3f, 1.3f }, { 5.1f, 2.5f, 3.0f, 1.1f },
            { 5.7f, 2.8f, 4.1f, 1.3f }
        };

        Mat testingDataMat = new Mat(45, 4, CvType.CV_32FC1);

        for (int i = 0; i < 30; i++) {
            testingDataMat.put(i, 0, testingData[i]);
        }

        double[] testingLabels = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
                                 };// [30]

        Mat responses = new Mat(70, 1, CvType.CV_32SC1, new Scalar(0));
        responses.put(0, 0, new int[] { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
                                      });


        Boost boost = Boost.create();
        boost.setBoostType(Boost.DISCRETE);
        boost.setWeakCount(3);
        boost.setMinSampleCount(4);
        boost.setMaxCategories(4);
        boost.setMaxDepth(2);


        boolean r = boost.train(trainingDataMat, Ml.ROW_SAMPLE, responses);
        System.out.println("是否有訓練成功=" + r);


        float[] smp = {
            5.1f, 3.4f, 1.5f, 0.2f //0
            //5.5f, 2.5f, 4.0f, 1.3f // 1
        };
        Mat sample = new Mat(1, 4, CvType.CV_32FC1);
        sample.put(0, 0, smp);
        Mat results = new Mat();
        int flags = 0;

        //LR.predict(sample);會出現create() called for the missing output array
        float result = boost.predict(sample, results, flags);
        // System.out.println("result="+result);
        System.out.println("邏輯回歸預測sample=" + results.get(0, 0)[0]);

        //測試精準值

        //預測正確累加1
        //預測正確累加1
        int right = 0;

        for (int i = 0; i < 30; i++) {
            result = boost.predict(testingDataMat.row(i), results, flags);

            //System.out.println("邏輯回歸預測=" + results.get(0, 0)[0]);
            if (testingLabels[i] == result) {
                right++;
            } else {
                System.out.println("預測錯誤!邏輯回歸預測是" + result + "正確是=" +
                                   testingLabels[i]);
            }

        }

        System.out.println("邏輯回歸預測30組Iris測試精準值=" + ((float)right /
                           (float)30.0f) * 100 + "%");



    }

}
