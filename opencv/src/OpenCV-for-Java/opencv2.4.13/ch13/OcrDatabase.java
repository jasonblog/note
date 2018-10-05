package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.highgui.Highgui;

public class OcrDatabase
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    /*
     * 因圖檔命名關係,將資料分10:1,10即訓練用,1即測試用.
     * 手寫圖檔命名原則:00~09.jpg-->是0,10~19-->是1,以此類推
     *
     */

    Mat trainingLabelsMat = new Mat(100, 1,
                                    CvType.CV_32SC1); //CV_32FC1會Exception in thread "main" java.lang.Exception: unknown exception
    Mat trainingDataMat = new Mat(100, 144, CvType.CV_32FC1);
    Mat testingDataMat = new Mat(10, 144, CvType.CV_32FC1);
    Mat trainingLabelsMatForAnn = new Mat(100, 10,
                                          CvType.CV_32FC1); //ann採用1:{1,0,0},2:{0,1,0}


    float[] trainingLabels = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9}; //[100]
    float[] testingLabels = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; //[10]

    //測試用,從test隨機抓1組lable0的資料
    Mat testSample0FrTestMat = new Mat(1, 144, CvType.CV_32FC1);

    //驗證測試用,從train隨機抓1組lable0的資料
    Mat testSample0FrTrainMat = new Mat(1, 144, CvType.CV_32FC1);

    //測試用,從test隨機抓1組lable1的資料
    Mat testSample1FrTestMat = new Mat(1, 144, CvType.CV_32FC1);

    //驗證測試用,從train隨機抓1組lable1的資料
    Mat testSample1FrTrainMat = new Mat(1, 144, CvType.CV_32FC1);


    //測試用,從test隨機抓1組lable2的資料
    Mat testSample2FrTestMat = new Mat(1, 144, CvType.CV_32FC1);

    //驗證測試用,從train隨機抓1組lable2的資料
    Mat testSample2FrTrainMat = new Mat(1, 144, CvType.CV_32FC1);



    Mat sample7 = new Mat(1, 144, CvType.CV_32FC1);

    OcrDatabase()
    {

        Mat source;

        //assign training Mat
        for (int i = 0; i < 70; i++) {
            if (i < 10) {
                source = Highgui.imread("C://opencv3.1//samples//ocr//0" + i + ".jpg",
                                        Highgui.CV_LOAD_IMAGE_GRAYSCALE);
            } else {
                source = Highgui.imread("C://opencv3.1//samples//ocr//" + i + ".jpg",
                                        Highgui.CV_LOAD_IMAGE_GRAYSCALE);
            }

            //assign ocr's image data into Mat
            Mat temp = source.reshape(1, 144); //change image's col & row

            for (int j = 0; j < 144; j++) {
                double[] data = new double[1];
                data = temp.get(j, 0);
                trainingDataMat.put(i, j, data);
            }


            //assign label data
            trainingLabelsMat.put(i, 0, trainingLabels[i]);

            //assign label data for ANN
            if (trainingLabels[i] == 0) {
                trainingLabelsMatForAnn.put(i, 0, new float[] {1, 0, 0, 0, 0, 0, 0, 0, 0, 0});
            } else if (trainingLabels[i] == 1) {
                trainingLabelsMatForAnn.put(i, 0, new float[] {0, 1, 0, 0, 0, 0, 0, 0, 0, 0});
            } else if (trainingLabels[i] == 2) {
                trainingLabelsMatForAnn.put(i, 0, new float[] {0, 0, 1, 0, 0, 0, 0, 0, 0, 0});
            } else if (trainingLabels[i] == 3) {
                trainingLabelsMatForAnn.put(i, 0, new float[] {0, 0, 0, 1, 0, 0, 0, 0, 0, 0});
            } else if (trainingLabels[i] == 4) {
                trainingLabelsMatForAnn.put(i, 0, new float[] {0, 0, 0, 0, 1, 0, 0, 0, 0, 0});
            } else if (trainingLabels[i] == 5) {
                trainingLabelsMatForAnn.put(i, 0, new float[] {0, 0, 0, 0, 0, 1, 0, 0, 0, 0});
            } else if (trainingLabels[i] == 6) {
                trainingLabelsMatForAnn.put(i, 0, new float[] {0, 0, 0, 0, 0, 0, 1, 0, 0, 0});
            } else if (trainingLabels[i] == 7) {
                trainingLabelsMatForAnn.put(i, 0, new float[] {0, 0, 0, 0, 0, 0, 0, 1, 0, 0});
            } else if (trainingLabels[i] == 8) {
                trainingLabelsMatForAnn.put(i, 0, new float[] {0, 0, 0, 0, 0, 0, 0, 0, 1, 0});
            } else if (trainingLabels[i] == 9) {
                trainingLabelsMatForAnn.put(i, 0, new float[] {0, 0, 0, 0, 0, 0, 0, 0, 0, 1});
            }
        }


        //assign testing Mat
        for (int i = 0; i < 10; i++) {
            source = Highgui.imread("C://opencv3.1//samples//ocr//number" + i + ".jpg",
                                    Highgui.CV_LOAD_IMAGE_GRAYSCALE);

            Mat temp = source.reshape(1, 144); //change image's col & row

            for (int j = 0; j < 144; j++) {
                double[] data = new double[1];
                data = temp.get(j, 0);
                testingDataMat.put(i, j, data);
            }
        }

        testSample0FrTestMat = testingDataMat.row(0);
        testSample1FrTestMat = testingDataMat.row(1);
        testSample2FrTestMat = testingDataMat.row(2);

        Mat sample = Highgui.imread("C://opencv3.1//samples//ocr//number71.jpg",
                                    Highgui.CV_LOAD_IMAGE_GRAYSCALE);
        Mat tempSample7 = sample.reshape(1, 144);

        for (int j = 0; j < 144; j++) {
            double[] data = new double[1];
            data = tempSample7.get(j, 0);
            sample7.put(0, j, data);
        }

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

    public Mat handleForML(Mat src)
    {
        Mat handle = new Mat(1, 144, CvType.CV_32FC1);
        Mat tempSample = src.reshape(1, 144);

        for (int j = 0; j < 144; j++) {
            double[] data = new double[1];
            data = tempSample.get(j, 0);
            handle.put(0, j, data);
        }

        return handle;
    }

    public Mat getTrainingLabelsMatForAnn()
    {
        return trainingLabelsMatForAnn;
    }

    //  public static void main(String[] args) {
    //      OcrDatabase ocr=new OcrDatabase();
    //      System.out.println(ocr.trainingLabels.length);
    //      System.out.println(ocr.testingLabels.length);
    //      System.out.println(ocr.sample7.dump());
    //  }


}
