package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.ml.DTrees;
import org.opencv.ml.LogisticRegression;
import org.opencv.ml.Ml;


public class Ch13_6_2LogisticRegressionForOcr
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }

    public static void main(String[] args)
    {
        Mat trainingLabelsMat = new Mat(20, 1,
                                        CvType.CV_32FC1); //CV_32FC1會Exception in thread "main" java.lang.Exception: unknown exception
        Mat trainingDataMat = new Mat(20, 144, CvType.CV_32FC1);

        float[] trainingLabels = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; //[20]


        Mat source;

        //assign training Mat
        //測試發現Mat 元素太大會出現OpenCV Error: Bad argument (check training parameters. Invalid training classifier) in cv::ml::LogisticRegressionImpl
        //異常故每個元素/10
        for (int i = 0; i < 20; i++) { //抓到10個0及10個1
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
                data[0] = data[0] / 10;
                trainingDataMat.put(i, j, data);
            }

            //assign label data
            trainingLabelsMat.put(i, 0, trainingLabels[i]);
        }


        LogisticRegression LR = LogisticRegression.create();
        //LR.setLearningRate(0.001);//也ok
        LR.setLearningRate(0.01);//與元素大小有關
        LR.setIterations(10);
        LR.setRegularization(LogisticRegression.REG_L2);
        LR.setTrainMethod(LogisticRegression.BATCH);
        LR.setMiniBatchSize(1);

        //      System.out.println(trainingDataMat.size());
        //      System.out.println(trainingLabelsMat.size());
        //System.out.println(trainingDataMat.row(18).dump());
        //System.out.println(trainingLabelsMat.dump());
        boolean r = LR.train(trainingDataMat, Ml.ROW_SAMPLE, trainingLabelsMat);


        System.out.println("是否有訓練成功=" + r);

        //隨便找3組測試
        Mat results = new Mat();
        int flags = 0;
        Mat zero = handleForML(
                       Imgcodecs.imread("C://opencv3.1//samples//ocr//number0.jpg",
                                        Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE));
        Mat one = handleForML(
                      Imgcodecs.imread("C://opencv3.1//samples//ocr//number1.jpg",
                                       Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE));


        LR.predict(zero, results, flags);
        System.out.println("邏輯迴歸預測result0=" + results.get(0, 0)[0] + ",實際是0");

        LR.predict(one, results, flags);
        System.out.println("邏輯迴歸預測result1=" + results.get(0, 0)[0] + ",實際是1");

    }
    //測試發現Mat 元素太大會出現OpenCV Error: Bad argument (check training parameters. Invalid training classifier) in cv::ml::LogisticRegressionImpl
    //異常故每個元素/10
    public static Mat handleForML(Mat src)
    {
        Mat handle = new Mat(1, 144, CvType.CV_32FC1);
        Mat tempSample = src.reshape(1, 144);

        for (int j = 0; j < 144; j++) {
            double[] data = new double[1];
            data = tempSample.get(j, 0);
            data[0] = data[0] / 10;
            handle.put(0, j, data);
        }

        return handle;
    }
}
