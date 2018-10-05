package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import org.opencv.ml.KNearest;
import org.opencv.ml.Ml;

public class Ch13_7_5KnnFor2DMap
{

    public static void main(String[] args)
    {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        float[] labels = {0, 0, 0, 0, 0, 1, 1, 1, 1, 1};
        Mat labelsMat = new Mat(10, 1, CvType.CV_32FC1);

        for (int i = 0; i < labels.length; i++) {
            labelsMat.put(i, 0, labels[i]);
        }



        float[][] trainingData = new float[10][2];

        for (int i = 0; i < 5; i++) {
            trainingData[i][0] = (float)(Math.random() * 255 + 1); //產生0~255的隨機整數
            trainingData[i][1] = (float)(Math.random() * 255 + 1);;
            trainingData[i + 5][0] = (float)(Math.random() * 255 + 255);
            trainingData[i + 5][1] = (float)(Math.random() * 255 + 255);
        }

        //System.out.println(trainingData[3][1]);
        Mat trainingDataMat = new Mat(10, 2, CvType.CV_32FC1);

        for (int i = 0; i < trainingDataMat.rows(); i++) {
            for (int j = 0; j < trainingDataMat.cols(); j++) {
                trainingDataMat.put(i, j, trainingData[i][j]);
            }
        }

        //System.out.println("trainingDataMat="+trainingDataMat.dump());



        KNearest knn = KNearest.create();
        boolean b = knn.train(trainingDataMat, Ml.ROW_SAMPLE, labelsMat);


        // Data for visual representation
        Mat image = new Mat(512, 512, CvType.CV_8UC3);
        //image.zeros(512, 512, CvType.CV_8UC3);

        for (int i = 0; i < image.rows(); i++) {
            for (int j = 0; j < image.cols(); j++) {
                Mat sample = new Mat(1, 2, CvType.CV_32FC1);
                float[] smp = {
                    i, j
                };
                sample.put(0, 0, smp);
                Mat results = new Mat();
                Mat neighborResponses = new Mat();
                Mat dists = new Mat();
                float result = knn.findNearest(sample, 2, results, neighborResponses, dists);
                double[] data = new double[3];

                /**註解速度較快
                System.out.println("sample="+sample.dump());
                System.out.println("results="+results.dump());
                System.out.println("neighborResponses="+neighborResponses.dump());
                System.out.println("dists="+dists.dump());
                System.out.println("result="+result);
                **/
                if (result != 0.0) {
                    data[0] = 0;
                    data[1] = 255;
                    data[2] = 0;

                } else {
                    data[0] = 255;
                    data[1] = 0;
                    data[2] = 0;
                }

                image.put(i, j, data);

            }

            // Show the training data
            for (int ii = 0; ii < 5; ii++) {
                Imgproc.circle(image, new Point(trainingData[ii][0],  trainingData[ii][1]), 5,
                               new Scalar(0,   0,   0), 1);
                Imgproc.circle(image, new Point(trainingData[ii + 5][0],
                                                trainingData[ii + 5][1]), 5, new Scalar(255, 255, 255), 1);
            }

        }

        Imgcodecs.imwrite("C://opencv3.1//samples//knn-For2DMap.jpg", image);

    }

}
