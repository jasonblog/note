package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.TermCriteria;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import org.opencv.ml.KNearest;
import org.opencv.ml.Ml;
import org.opencv.ml.SVM;

public class Ch13_11_4SvmFor2DMap
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        //40個sample
        float[] labels = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
        Mat labelsMat = new Mat(40, 1, CvType.CV_32SC1);

        for (int i = 0; i < labels.length; i++) {
            labelsMat.put(i, 0, labels[i]);
        }


        //group1,使用常態分配
        Mat normalDistubitionMat = new Mat(40, 2, CvType.CV_32FC1);
        Core.randu(normalDistubitionMat, 210, 150);

        //group2
        Mat normalDistubitionMat2 = new Mat(40, 2, CvType.CV_32FC1);
        Core.randu(normalDistubitionMat2, 188, 42);

        for (int i = 20; i < 40; i++) {
            double x = normalDistubitionMat2.get(i, 0)[0];
            double y = normalDistubitionMat2.get(i, 1)[0];
            normalDistubitionMat.put(i, 0, x);
            normalDistubitionMat.put(i, 1, y);
        }

        SVM svm = SVM.create();
        TermCriteria criteria = new TermCriteria(TermCriteria.MAX_ITER, 100, 1e-6);
        svm.setKernel(svm.LINEAR);
        svm.setType(svm.C_SVC);
        svm.setGamma(0.5);
        svm.setNu(0.5);
        //1-->97.77778%,2-->100%
        svm.setC(1);
        svm.setTermCriteria(criteria);
        boolean b = svm.train(normalDistubitionMat, Ml.ROW_SAMPLE, labelsMat);


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
                float result = svm.predict(sample);
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
            for (int ii = 0; ii < 20; ii++) {
                //group1顯示白色
                Imgproc.circle(image, new Point(normalDistubitionMat.get(ii, 0)[0],
                                                normalDistubitionMat.get(ii, 1)[0]), 3, new Scalar(0,   0,   0), 2);
                //group2顯示黑色
                Imgproc.circle(image, new Point(normalDistubitionMat.get(ii + 20, 0)[0],
                                                normalDistubitionMat.get(ii + 20, 1)[0]), 3, new Scalar(255, 255, 255), 2);
            }

        }

        Imgcodecs.imwrite("C://opencv3.1//samples//svm-For2DMap.jpg", image);

    }

}
