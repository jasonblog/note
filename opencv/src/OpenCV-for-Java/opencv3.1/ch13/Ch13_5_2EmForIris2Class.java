package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.core.TermCriteria;
import org.opencv.ml.Boost;
import org.opencv.ml.EM;
import org.opencv.ml.Ml;
import org.opencv.ml.RTrees;

public class Ch13_5_2EmForIris2Class
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





        Mat responses = new Mat(70, 1, CvType.CV_32SC1);
        responses.put(0, 0, new int[] { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
                                      });


        EM em = EM.create();
        //2-->66.66667%,3-->42.22222%,4-->44.444447%,5-->2.2222223%
        em.setClustersNumber(
            2);//1->OpenCV Error: Assertion failed (nclusters > 1) in cv::ml::EMImpl::setClustersNumber,
        em.setCovarianceMatrixType(
            0);// (covMatType == COV_MAT_SPHERICAL || covMatType == COV_MAT_DIAGONAL || covMatType == COV_MAT_GENERIC)
        TermCriteria criteria = new TermCriteria(TermCriteria.EPS +
                TermCriteria.MAX_ITER, 300, 0.1);
        em.setTermCriteria(criteria);
        Mat logLikelihoods = new Mat();
        Mat probs = new Mat();
        //boolean r=em.train(trainingDataMat, Ml.ROW_SAMPLE, responses);
        boolean r = em.trainEM(trainingDataMat, logLikelihoods, responses, probs);
        //boolean r=em.trainM(trainingDataMat,new Mat(), logLikelihoods, responses,probs);
        System.out.println("是否有訓練成功=" + r);




        float[] smp = {

            // 使用train data 去test都相反
            5.2f, 3.5f, 1.5f, 0.2f //0,//get1
            //5.0f,3.6f,1.4f,0.2f//0,//get1
            //5.0f,2.0f,3.5f,1.0f// 1//get0
            //5.4f,3.0f,4.5f,1.5f//1//get0
        };
        Mat sample = new Mat(1, 4, CvType.CV_32FC1);
        sample.put(0, 0, smp);
        Mat results = new Mat();
        int flags = 0;

        //LR.predict(sample);會出現create() called for the missing output array
        float result = em.predict(sample);
        // System.out.println("result="+result);
        System.out.println("最大期望值預測sample=" + result);

        //測試精準值

        //預測正確累加1
        int right = 0;

        for (int i = 0; i < 30; i++) {
            result = em.predict(testingDataMat.row(i));
            //double[]  resultSet = em.predict2(testingDataMat.row(i),probs);
            //System.out.println("邏輯回歸預測=" +resultSet[0]+"--"+resultSet[1]+",probs.dump="+ probs.dump());
            //if(testingLabels[i]==resultSet[1]){

            //修改變成相反類,把預測成1改為0,預測成0改為1

            if (testingLabels[i] == 0) {
                testingLabels[i] = 1;
            } else if (testingLabels[i] == 1) {
                testingLabels[i] = 0;
            }


            if (testingLabels[i] == result) {
                right++;
            } else {
                System.out.println("預測錯誤!最大期望值預測是" + result + "正確是=" +
                                   testingLabels[i]);
                //System.out.println("預測錯誤!邏輯回歸預測是"+resultSet[1]+"正確是="+testingLabels[i]);
            }

        }

        System.out.println("最大期望值預測30組Iris測試精準值=" + ((float)right /
                           (float)30.0f) * 100 + "%");



    }

}
