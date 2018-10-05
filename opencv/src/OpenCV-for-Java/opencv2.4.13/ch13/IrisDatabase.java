package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

public class IrisDatabase
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    /*
     * 按照機器學習慣例,將資料分7:3,7即訓練用,3即測試用.
     * Iris-setosa設定標籤為0
     * Iris-versicolor設定標籤為1
     * Iris-virginica設定標籤為2
     */

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
        {5.4f, 3.0f, 4.5f, 1.5f},
        //Iris-virginica,35組
        {6.3f, 3.3f, 6.0f, 2.5f},
        {5.8f, 2.7f, 5.1f, 1.9f},
        {7.1f, 3.0f, 5.9f, 2.1f},
        {6.3f, 2.9f, 5.6f, 1.8f},
        {6.5f, 3.0f, 5.8f, 2.2f},
        {7.6f, 3.0f, 6.6f, 2.1f},
        {4.9f, 2.5f, 4.5f, 1.7f},
        {7.3f, 2.9f, 6.3f, 1.8f},
        {6.7f, 2.5f, 5.8f, 1.8f},
        {7.2f, 3.6f, 6.1f, 2.5f},
        {6.5f, 3.2f, 5.1f, 2.0f},
        {6.4f, 2.7f, 5.3f, 1.9f},
        {6.8f, 3.0f, 5.5f, 2.1f},
        {5.7f, 2.5f, 5.0f, 2.0f},
        {5.8f, 2.8f, 5.1f, 2.4f},
        {6.4f, 3.2f, 5.3f, 2.3f},
        {6.5f, 3.0f, 5.5f, 1.8f},
        {7.7f, 3.8f, 6.7f, 2.2f},
        {7.7f, 2.6f, 6.9f, 2.3f},
        {6.0f, 2.2f, 5.0f, 1.5f},
        {6.9f, 3.2f, 5.7f, 2.3f},
        {5.6f, 2.8f, 4.9f, 2.0f},
        {7.7f, 2.8f, 6.7f, 2.0f},
        {6.3f, 2.7f, 4.9f, 1.8f},
        {6.7f, 3.3f, 5.7f, 2.1f},
        {7.2f, 3.2f, 6.0f, 1.8f},
        {6.2f, 2.8f, 4.8f, 1.8f},
        {6.1f, 3.0f, 4.9f, 1.8f},
        {6.4f, 2.8f, 5.6f, 2.1f},
        {7.2f, 3.0f, 5.8f, 1.6f},
        {7.4f, 2.8f, 6.1f, 1.9f},
        {7.9f, 3.8f, 6.4f, 2.0f},
        {6.4f, 2.8f, 5.6f, 2.2f},
        {6.3f, 2.8f, 5.1f, 1.5f},
        {6.1f, 2.6f, 5.6f, 1.4f}

    };

    float[] trainingLabels = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}; //[105]
    float[] testingLabels = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}; //[45]

    float[][] testingData = {
        //Iris-setosa,15組
        {5.0f, 3.2f, 1.2f, 0.2f},
        {5.5f, 3.5f, 1.3f, 0.2f},
        {4.9f, 3.1f, 1.5f, 0.1f},
        {4.4f, 3.0f, 1.3f, 0.2f},
        {5.1f, 3.4f, 1.5f, 0.2f},
        {5.0f, 3.5f, 1.3f, 0.3f},
        {4.5f, 2.3f, 1.3f, 0.3f},
        {4.4f, 3.2f, 1.3f, 0.2f},
        {5.0f, 3.5f, 1.6f, 0.6f},
        {5.1f, 3.8f, 1.9f, 0.4f},
        {4.8f, 3.0f, 1.4f, 0.3f},
        {5.1f, 3.8f, 1.6f, 0.2f},
        {4.6f, 3.2f, 1.4f, 0.2f},
        {5.3f, 3.7f, 1.5f, 0.2f},
        {5.0f, 3.3f, 1.4f, 0.2f},
        //Iris-versicolor,15組
        {6.0f, 3.4f, 4.5f, 1.6f},
        {6.7f, 3.1f, 4.7f, 1.5f},
        {6.3f, 2.3f, 4.4f, 1.3f},
        {5.6f, 3.0f, 4.1f, 1.3f},
        {5.5f, 2.5f, 4.0f, 1.3f},
        {5.5f, 2.6f, 4.4f, 1.2f},
        {6.1f, 3.0f, 4.6f, 1.4f},
        {5.8f, 2.6f, 4.0f, 1.2f},
        {5.0f, 2.3f, 3.3f, 1.0f},
        {5.6f, 2.7f, 4.2f, 1.3f},
        {5.7f, 3.0f, 4.2f, 1.2f},
        {5.7f, 2.9f, 4.2f, 1.3f},
        {6.2f, 2.9f, 4.3f, 1.3f},
        {5.1f, 2.5f, 3.0f, 1.1f},
        {5.7f, 2.8f, 4.1f, 1.3f},
        //Iris-virginica,15組
        {7.7f, 3.0f, 6.1f, 2.3f},
        {6.3f, 3.4f, 5.6f, 2.4f},
        {6.4f, 3.1f, 5.5f, 1.8f},
        {6.0f, 3.0f, 4.8f, 1.8f},
        {6.9f, 3.1f, 5.4f, 2.1f},
        {6.7f, 3.1f, 5.6f, 2.4f},
        {6.9f, 3.1f, 5.1f, 2.3f},
        {5.8f, 2.7f, 5.1f, 1.9f},
        {6.8f, 3.2f, 5.9f, 2.3f},
        {6.7f, 3.3f, 5.7f, 2.5f},
        {6.7f, 3.0f, 5.2f, 2.3f},
        {6.3f, 2.5f, 5.0f, 1.9f},
        {6.5f, 3.0f, 5.2f, 2.0f},
        {6.2f, 3.4f, 5.4f, 2.3f},
        {5.9f, 3.0f, 5.1f, 1.8f}

    };
    Mat trainingLabelsMat = new Mat(105, 1,
                                    CvType.CV_32FC1); //CV_32FC1會Exception in thread "main" java.lang.Exception: unknown exception
    Mat trainingDataMat = new Mat(105, 4, CvType.CV_32FC1);
    Mat testingDataMat = new Mat(45, 4, CvType.CV_32FC1);
    Mat trainingLabelsMatForAnn = new Mat(105, 3,
                                          CvType.CV_32FC1); //ann採用1:{1,0,0},2:{0,1,0}

    //測試用,從test隨機抓1組lable0的資料
    Mat testSample0FrTestMat = new Mat(1, 4, CvType.CV_32FC1);

    //驗證測試用,從train隨機抓1組lable0的資料
    Mat testSample0FrTrainMat = new Mat(1, 4, CvType.CV_32FC1);

    //測試用,從test隨機抓1組lable1的資料
    Mat testSample1FrTestMat = new Mat(1, 4, CvType.CV_32FC1);

    //驗證測試用,從train隨機抓1組lable1的資料
    Mat testSample1FrTrainMat = new Mat(1, 4, CvType.CV_32FC1);


    //測試用,從test隨機抓1組lable2的資料
    Mat testSample2FrTestMat = new Mat(1, 4, CvType.CV_32FC1);

    //驗證測試用,從train隨機抓1組lable2的資料
    Mat testSample2FrTrainMat = new Mat(1, 4, CvType.CV_32FC1);


    IrisDatabase()
    {
        for (int i = 0; i < 105; i++) {
            trainingLabelsMat.put(i, 0, trainingLabels[i]);
            trainingDataMat.put(i, 0, trainingData[i]);

            if (trainingLabels[i] == 0) {
                trainingLabelsMatForAnn.put(i, 0, new float[] {1, 0, 0});
            } else if (trainingLabels[i] == 1) {
                trainingLabelsMatForAnn.put(i, 0, new float[] {0, 1, 0});
            } else if (trainingLabels[i] == 2) {
                trainingLabelsMatForAnn.put(i, 0, new float[] {0, 0, 1});
            }



        }

        for (int i = 0; i < 45; i++) {
            testingDataMat.put(i, 0, testingData[i]);
        }

        testSample0FrTestMat.put(0, 0, new float[] {5.0f, 3.5f, 1.6f, 0.6f});
        testSample1FrTestMat.put(0, 0, new float[] {5.8f, 2.6f, 4.0f, 1.2f});
        testSample2FrTestMat.put(0, 0, new float[] {6.7f, 3.0f, 5.2f, 2.3f});
    }

    public float[] getTestingLabels()
    {
        return testingLabels;
    }

    public void setTestingLabels(float[] testingLabels)
    {
        this.testingLabels = testingLabels;
    }

    public Mat getTrainingLabelsMat()
    {
        return trainingLabelsMat;
    }

    public void setTrainingLabelsMat(Mat trainingLabelsMat)
    {
        this.trainingLabelsMat = trainingLabelsMat;
    }

    public Mat getTrainingDataMat()
    {
        return trainingDataMat;
    }

    public void setTrainingDataMat(Mat trainingDataMat)
    {
        this.trainingDataMat = trainingDataMat;
    }

    public Mat getTestingDataMat()
    {
        return testingDataMat;
    }

    public void setTestingDataMat(Mat testingDataMat)
    {
        this.testingDataMat = testingDataMat;
    }

    public Mat getTestSample0FrTestMat()
    {
        return testSample0FrTestMat;
    }

    public Mat getTestSample0FrTrainMat()
    {
        return testSample0FrTrainMat;
    }

    public Mat getTestSample1FrTestMat()
    {
        return testSample1FrTestMat;
    }

    public Mat getTestSample1FrTrainMat()
    {
        return testSample1FrTrainMat;
    }

    public Mat getTestSample2FrTestMat()
    {
        return testSample2FrTestMat;
    }

    public Mat getTestSample2FrTrainMat()
    {
        return testSample2FrTrainMat;
    }

    public Mat getTrainingLabelsMatForAnn()
    {
        return trainingLabelsMatForAnn;
    }

    //  public static void main(String[] args) {
    //      IrisDatabase iris=new IrisDatabase();
    //      System.out.println(iris.trainingLabelsMat.dump());
    //      System.out.println(iris.testingLabels.length);
    //
    //  }

}
