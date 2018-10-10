package ch13;
import java.util.Arrays;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;
import org.opencv.ml.CvKNearest;

public class KnnExample
{

    public static void main(String[] args)
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        float[] labels = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};

        //System.out.println("labels-len="+labels.length);
        Mat labelsMat = new Mat(40, 1, CvType.CV_32FC1);

        for (int i = 0; i < labels.length; i++) {
            labelsMat.put(i, 0, labels[i]);
        }

        //System.out.println(labelsMat.dump());



        //System.out.println(trainingData[3][1]);
        Mat trainingDataMat = new Mat(40, 144, CvType.CV_32FC1);
        //System.out.println("trainingDataMat.rows()="+trainingDataMat.rows());
        Mat source;

        //      Mat temp=source.reshape(1, 144);
        //      System.out.println(temp.size());
        //      System.out.println(temp.get(0,3));//null
        //      System.out.println(temp.get(3,0)[0]);//¦³­È
        for (int i = 0; i < 40; i++) {
            String tempFileNamw = "";

            if (i < 10) {
                source = Highgui.imread("C://opencv3.1//samples//ocr//0" + i + ".jpg",
                                        Highgui.CV_LOAD_IMAGE_GRAYSCALE);
            } else {
                source = Highgui.imread("C://opencv3.1//samples//ocr//" + i + ".jpg",
                                        Highgui.CV_LOAD_IMAGE_GRAYSCALE);
            }

            Mat temp = source.reshape(1, 144);

            for (int j = 0; j < 144; j++) {
                double[] data = new double[1];
                data = temp.get(j, 0);
                trainingDataMat.put(i, j, data);

            }

        }

        Mat sampleIdx = new Mat();
        CvKNearest knn = new CvKNearest();
        knn.train(trainingDataMat, labelsMat, sampleIdx, false, 4, false);
        System.out.println("trainingDataMat=" + trainingDataMat.size());
        //System.out.println(trainingDataMat.get(23,23)[0]);//¦³­È
        System.out.println("sampleIdx=" + sampleIdx.dump());

        Mat sample = Highgui.imread("C://opencv3.1//samples//ocr//number0.jpg",
                                    Highgui.CV_LOAD_IMAGE_GRAYSCALE);
        Mat results = new Mat();
        Mat neighborResponses = new Mat();
        Mat dists = new Mat();

        //sample.convertTo(sample, CvType.CV_32FC1);
        //testSample=new Mat(1,144,CvType.CV_32FC1);
        // Mat testSample=sample.reshape(1, 144);
        Mat samples32f = new Mat(1, 144, CvType.CV_32FC1);

        Mat tempSample = sample.reshape(1, 144);


        for (int k = 0; k < samples32f.cols(); k++) {
            double[] data = new double[1];
            data = tempSample.get(k, 0);
            //System.out.println(data[0]);
            samples32f.put(0, k, data);

        }


        // testSample.convertTo(samples32f, CvType.CV_32FC1);
        //System.out.println("tempSample="+sample.type());
        System.out.println("Sample0,67=" + samples32f.get(0, 67)[0]);

        //tempSample.convertTo(tempSample, CvType.CV_32FC1);
        //System.out.println("sample="+sample.dims());
        //System.out.println("testSample="+testSample.dims()+","+testSample.size());
        System.out.println("samples32f=" + samples32f.rows() + "," + samples32f.size());
        float result = knn.find_nearest(samples32f, 4, results, neighborResponses,
                                        dists);

        //System.out.println("testSample="+testSample.dump());
        System.out.println("results=" + results.dump());
        System.out.println("neighborResponses=" + neighborResponses.dump());
        System.out.println("dists=" + dists.dump());
        System.out.println("result=" + result);


    }

}
