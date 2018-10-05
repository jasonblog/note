package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.TermCriteria;
import org.opencv.highgui.Highgui;
import org.opencv.ml.CvANN_MLP;
import org.opencv.ml.CvSVM;
import org.opencv.ml.CvSVMParams;


public class SVM
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {

        //train data
        float[] labels = {1.0f, -1.0f, -1.0f, -1.0f}; //[4]
        Mat labelsMat = new Mat(4, 1, CvType.CV_32FC1);
        labelsMat.put(0, 0, labels[0]);
        labelsMat.put(1, 0, labels[1]);
        labelsMat.put(2, 0, labels[2]);
        labelsMat.put(3, 0, labels[3]);

        //System.out.println(labelsMat.dump());

        float[][] trainingData = {{501, 10}, {255, 10}, {501, 255}, {10, 501}}; //[4][2]
        Mat trainingDataMat = new Mat(4, 2, CvType.CV_32FC1);
        trainingDataMat.put(0, 0, trainingData[0]);
        trainingDataMat.put(1, 0, trainingData[1]);
        trainingDataMat.put(2, 0, trainingData[2]);
        trainingDataMat.put(3, 0, trainingData[3]);
        //System.out.println(trainingDataMat.dump());

        CvSVM svm = new CvSVM();
        CvSVMParams Params = new CvSVMParams();
        Params.set_svm_type(svm.C_SVC);
        Params.set_kernel_type(svm.LINEAR);
        Params.set_term_crit(new TermCriteria(TermCriteria.MAX_ITER, 100, 1E-6));

        Mat responses = new Mat(4, 1, CvType.CV_32FC1);
        //svm.train(trainingDataMat, labelsMat);//幾乎沒有分類
        Mat varIdx = new Mat();
        Mat sampleIdx = new Mat();
        svm.train(trainingDataMat, labelsMat, varIdx, sampleIdx, Params);






        Mat image = new Mat(512, 512, CvType.CV_8UC3);

        for (int i = 0; i < image.rows(); i++) {
            for (int j = 0; j < image.cols(); j++) {
                Mat sample = new Mat(1, 2, CvType.CV_32FC1);
                float[] smp = {
                    i, j
                };
                sample.put(0, 0, smp);

                Mat outputs = new Mat();
                //float result=ann.predict(sample, results, flags);
                float result = svm.predict(sample);
                //System.out.println("result="+result+",("+i+","+j+")");
                //System.out.println("result="+result);
                double[] data = new double[3];

                if (result == 1) {
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

            int c = svm.get_support_vector_count();

            //System.out.println("c="+c);

        }





        Highgui.imwrite("C://opencv3.1//svm-test1.jpg", image);

    }
}
