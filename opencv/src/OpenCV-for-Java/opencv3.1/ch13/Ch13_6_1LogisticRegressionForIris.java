package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.core.TermCriteria;
import org.opencv.ml.LogisticRegression;
import org.opencv.ml.Ml;


//與13_6_2不同組數(train:20,70),不同種類比較(Iris-setosa,versicolor vs setosa,virginica)
public class
    Ch13_6_1LogisticRegressionForIris  // ok正確,但是似乎只能2元(即0,1),不能(0,1,2)
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }

    public static void main(String[] args)
    {

        float[][] trainingData = {
            //Iris-setosa
            { 5.1f, 3.5f, 1.4f, 0.2f },
            { 4.9f, 3.0f, 1.4f, 0.2f }, { 4.7f, 3.2f, 1.3f, 0.2f },
            { 4.6f, 3.1f, 1.5f, 0.2f }, { 5.0f, 3.6f, 1.4f, 0.2f },
            { 5.4f, 3.9f, 1.7f, 0.4f }, { 4.6f, 3.4f, 1.4f, 0.3f },
            { 5.0f, 3.4f, 1.5f, 0.2f }, { 4.4f, 2.9f, 1.4f, 0.2f },
            { 4.9f, 3.1f, 1.5f, 0.1f },
            //Iris-versicolor,10組
            {7.0f, 3.2f, 4.7f, 1.4f},
            {6.4f, 3.2f, 4.5f, 1.5f},
            {6.9f, 3.1f, 4.9f, 1.5f},
            {5.5f, 2.3f, 4.0f, 1.3f},
            {6.5f, 2.8f, 4.6f, 1.5f},
            {5.7f, 2.8f, 4.5f, 1.3f},
            {6.3f, 3.3f, 4.7f, 1.6f},
            {4.9f, 2.4f, 3.3f, 1.0f},
            {6.6f, 2.9f, 4.6f, 1.3f},
            {5.2f, 2.7f, 3.9f, 1.4f}
        };// [20][4]

        Mat trainingDataMat = new Mat(20, 4, CvType.CV_32FC1);

        for (int i = 0; i < 20; i++) {
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

        Mat testingDataMat = new Mat(30, 4, CvType.CV_32FC1);

        for (int i = 0; i < 30; i++) {
            testingDataMat.put(i, 0, testingData[i]);
        }

        double[] testingLabels = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
                                 };// [30]

        Mat responses = new Mat(20, 1, CvType.CV_32FC1, new Scalar(0));
        responses.put(0, 0, new float[] { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
                                          1, 1, 1, 1, 1, 1, 1, 1
                                        });
        //responses.put(0, 0, new float[] { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,1, 1, 1, 1, 1, 1 ,1});
        TermCriteria criteria = new TermCriteria(TermCriteria.EPS +
                TermCriteria.MAX_ITER, 300, 0.1);
        LogisticRegression LR = LogisticRegression.create();

        //0.9,1-->100%,0.1->50%,0.5->63.3%,0.6->90%
        LR.setLearningRate(1);
        LR.setIterations(10);
        LR.setRegularization(LogisticRegression.REG_L2);
        LR.setTrainMethod(LogisticRegression.BATCH);
        LR.setMiniBatchSize(1);
        LR.setTermCriteria(criteria);
        //      System.out.println(trainingDataMat.size());
        //      System.out.println(responses.size());

        boolean r = LR.train(trainingDataMat, Ml.ROW_SAMPLE, responses);
        System.out.println("是否有訓練成功=" + r);

        float[] smp = {
            //5.1f,3.4f,1.5f,0.2f//0
            //5.1f, 3.5f, 1.4f, 0.2f//0
            6.4f, 3.2f, 4.5f, 1.5f //1
        };
        Mat sample = new Mat(1, 4, CvType.CV_32FC1);
        sample.put(0, 0, smp);
        Mat results = new Mat();
        int flags = 0;

        //LR.predict(sample);會出現create() called for the missing output array
        float result = LR.predict(sample, results, flags);
        //System.out.println("result="+result);
        System.out.println("邏輯迴歸預測sample=" + results.get(0, 0)[0]);
        // System.out.println("flags="+flags);


        //預測正確累加1
        int right = 0;
        double LRresult;

        for (int i = 0; i < 30; i++) {
            LR.predict(testingDataMat.row(i), results, flags);
            LRresult = results.get(0, 0)[0];

            //System.out.println("邏輯回歸預測=" + results.get(0, 0)[0]);
            if (testingLabels[i] == LRresult) {
                right++;
            } else {
                System.out.println("預測錯誤!邏輯迴歸預測是" + LRresult + "正確是=" +
                                   testingLabels[i]);
            }

        }

        System.out.println("邏輯迴歸預測30組Iris測試精準值=" + ((float)right /
                           (float)30.0f) * 100 + "%");
    }
}
