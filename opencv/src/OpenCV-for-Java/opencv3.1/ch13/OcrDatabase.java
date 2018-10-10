package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;

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
    Mat allDataMat = new Mat(110, 144, CvType.CV_32FC1); //all data for PCA



    Mat trainingLabelsFloatMat = new Mat(100, 10, CvType.CV_32FC1); //for ANN

    float[] trainingLabels = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9}; //[100]
    float[] testingLabels = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; //[10]
    float[][] trainingLabelsFloat = { //ANN專用
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
    };

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
        for (int i = 0; i < 100; i++) {
            if (i < 10) {
                source = Imgcodecs.imread("C://opencv3.1//samples//ocr//0" + i + ".jpg",
                                          Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
            } else {
                source = Imgcodecs.imread("C://opencv3.1//samples//ocr//" + i + ".jpg",
                                          Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
            }

            //assign ocr's image data into Mat
            Mat temp = source.reshape(1, 144); //change image's col & row

            for (int j = 0; j < 144; j++) {
                double[] data = new double[1];
                data = temp.get(j, 0);
                trainingDataMat.put(i, j, data);
                allDataMat.put(i, j, data);
            }


            //assign label data
            trainingLabelsMat.put(i, 0, trainingLabels[i]);
            //for ann label
            trainingLabelsFloatMat.put(i, 0, trainingLabelsFloat[i]);
        }


        //assign testing Mat
        for (int i = 0; i < 10; i++) {
            source = Imgcodecs.imread("C://opencv3.1//samples//ocr//number" + i + ".jpg",
                                      Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);

            Mat temp = source.reshape(1, 144); //change image's col & row

            for (int j = 0; j < 144; j++) {
                double[] data = new double[1];
                data = temp.get(j, 0);
                testingDataMat.put(i, j, data);
                allDataMat.put(i + 100, j, data);
            }
        }

        testSample0FrTestMat = testingDataMat.row(0);
        testSample1FrTestMat = testingDataMat.row(1);
        testSample2FrTestMat = testingDataMat.row(2);

        Mat sample = Imgcodecs.imread("C://opencv3.1//samples//ocr//number71.jpg",
                                      Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
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

    public Mat getTrainingLabelsFloatMat()
    {
        return trainingLabelsFloatMat;
    }

    public void setTrainingLabelsFloatMat(Mat trainingLabelsFloatMat)
    {
        this.trainingLabelsFloatMat = trainingLabelsFloatMat;
    }

    public Mat getAllDataMat()
    {
        return allDataMat;
    }

    public void setAllDataMat(Mat allDataMat)
    {
        this.allDataMat = allDataMat;
    }

    //  public static void main(String[] args) {
    //      OcrDatabase ocr=new OcrDatabase();
    //      System.out.println(ocr.trainingLabels.length);
    //      System.out.println(ocr.testingLabels.length);
    //      System.out.println(ocr.sample7.dump());
    //  }


}
