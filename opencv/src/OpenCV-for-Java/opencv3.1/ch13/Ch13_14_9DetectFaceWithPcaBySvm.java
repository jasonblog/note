package ch13;

import java.text.NumberFormat;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.core.TermCriteria;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.ml.Ml;
import org.opencv.ml.SVM;

public class Ch13_14_9DetectFaceWithPcaBySvm
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        NumberFormat nf = NumberFormat.getInstance();
        nf.setMaximumFractionDigits(18);

        float[] trainingLabels = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13}; //[112]
        float[] testingLabels = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

        Mat trainingLabelsMat = new Mat(112, 1, CvType.CV_32SC1);

        for (int i = 0; i < 112; i++) {

            trainingLabelsMat.put(i, 0, trainingLabels[i]);
        }

        Mat allDataMat = new Mat(126, 400, CvType.CV_32FC1); //14人x9照片
        Mat source = new Mat();
        Mat mean = new Mat();
        Mat vectors = new Mat();


        int start = 0;

        //先塞入每人8張照片
        for (int i = 1; i <= 14; i++) {
            for (int j = 1; j < 9; j++) {

                source = Imgcodecs.imread("C://opencv3.1//samples//yalefaces//" + i + "-" + j +
                                          ".png", Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
                // System.out.println(i+"-"+j);
                Mat temp = source.reshape(1, 400); //change image's col & row

                for (int k = 0; k < 400; k++) {
                    double[] data = new double[1];
                    data = temp.get(k, 0);
                    allDataMat.put(start, k, data);

                }

                start++;
            }
        }

        //先塞入每人第9張照片
        for (int i = 1; i <= 14; i++) {
            for (int j = 9; j < 10; j++) {

                source = Imgcodecs.imread("C://opencv3.1//samples//yalefaces//" + i + "-" + j +
                                          ".png", Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
                // System.out.println(i+"-"+j);
                Mat temp = source.reshape(1, 400); //change image's col & row

                for (int k = 0; k < 400; k++) {
                    double[] data = new double[1];
                    data = temp.get(k, 0);
                    allDataMat.put(start, k, data);
                }

                start++;
            }
        }




        //System.out.println(testingDataMat.dump());
        Mat Mean = new Mat();
        Mat Vectors = new Mat();

        //用30落到92%
        Core.PCACompute(allDataMat.t(), Mean, Vectors, 20);

        Mat allPcaData = Vectors.t();
        //取0~99列做為train
        Mat subMatTrainPcaFace = allPcaData.submat(0, 112, 0, 20);

        //取100~110列做為test
        Mat subMatTestPcaFace = allPcaData.submat(112, 126, 0, 20);
        //System.out.println(Vectors.dump());
        // System.out.println(trainingDataMat.dump()+",size="+trainingDataMat.size());

        //System.out.println(vectors.dump()+",size="+vectors.size());//112x20
        // System.out.println(i+" 1:"+nf.format(vectors.get(0, 0)[0])+" 2:"+nf.format(vectors.get(0, 1)[0])+" 3:"+nf.format(vectors.get(0, 2)[0])+" 4:"+nf.format(vectors.get(0, 3)[0])+" 5:"+nf.format(vectors.get(0, 4)[0])+" 6:"+nf.format(vectors.get(0, 5)[0])+" 7:"+nf.format(vectors.get(0, 6)[0])+" 8:"+nf.format(vectors.get(0, 7)[0])+" 9:"+nf.format(vectors.get(0, 8)[0])+" 10:"+nf.format(vectors.get(0, 9)[0])+" 11:"+nf.format(vectors.get(0, 10)[0])+" 12:"+nf.format(vectors.get(0, 11)[0])+" 13:"+nf.format(vectors.get(0, 12)[0])+" 14:"+nf.format(vectors.get(0, 13)[0])+" 15:"+nf.format(vectors.get(0, 14)[0])+" 16:"+nf.format(vectors.get(0, 15)[0])+" 17:"+nf.format(vectors.get(0, 16)[0])+" 18:"+nf.format(vectors.get(0, 17)[0])+" 19:"+nf.format(vectors.get(0, 18)[0])+" 20:"+nf.format(vectors.get(0, 19)[0])+" 21:"+nf.format(vectors.get(1, 0)[0])+" 22:"+nf.format(vectors.get(1, 1)[0])+" 23:"+nf.format(vectors.get(1, 2)[0])+" 24:"+nf.format(vectors.get(1, 3)[0])+" 25:"+nf.format(vectors.get(1, 4)[0])+" 26:"+nf.format(vectors.get(1, 5)[0])+" 27:"+nf.format(vectors.get(1, 6)[0])+" 28:"+nf.format(vectors.get(1, 7)[0])+" 29:"+nf.format(vectors.get(1, 8)[0])+" 30:"+nf.format(vectors.get(1, 9)[0]));

        //System.out.println(vectors.cols()+","+vectors.rows());

        SVM svm = SVM.create();
        TermCriteria criteria = new TermCriteria(TermCriteria.MAX_ITER, 100, 1e-6);
        //svm.setKernel(svm.LINEAR);
        svm.setType(svm.C_SVC);
        svm.setGamma(0.5);
        svm.setNu(0.5);
        //1-->97.77778%,2-->100%
        svm.setC(1);
        svm.setTermCriteria(criteria);

        boolean r = svm.train(subMatTrainPcaFace, Ml.ROW_SAMPLE, trainingLabelsMat);
        System.out.println("是否有訓練成功=" + r);

        float result;
        int right = 0;

        for (int i = 0; i < subMatTestPcaFace.rows(); i++) {
            result = svm.predict(subMatTestPcaFace.row(i));

            if (result == testingLabels[i]) {
                right++;
            } else {
                System.out.println("預測錯誤!支援向量機預測是" + result + ",正確是=" +
                                   testingLabels[i]);
            }
        }

        System.out.println("支援向量機測試精準值=" + ((float)right /
                           (float)subMatTestPcaFace.rows()) * 100 + "%");

    }


}
