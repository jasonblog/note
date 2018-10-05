package ch13;

import java.io.FileWriter;
import java.text.NumberFormat;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.imgcodecs.Imgcodecs;

public class Ch13_14_10GetFacePcaForLibsvm
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {

        NumberFormat nf = NumberFormat.getInstance();
        nf.setMaximumFractionDigits(18);


        Mat allDataMat = new Mat(126, 400, CvType.CV_32FC1); //14人x9照片
        Mat source = new Mat();
        Mat Mean = new Mat();
        Mat Vectors = new Mat();

        String fileNameTrain = "C:\\TrainFacePCA.txt";
        String fileNameTest = "C:\\TestFacePCA.txt";





        //人臉共14人,每人9張不同光源影像及不同表情,其中8張拿來做訓練,另一 張做測試使用!
        //i:14人,當j=1;j<=8;j++ 輸出訓練樣本前20個PCA特徵值
        //當j=9;j<=9;j++ 輸出測試/預測樣本前20個PCA特徵值

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

        Core.PCACompute(allDataMat.t(), Mean, Vectors, 20);
        Mat allPcaData = Vectors.t();
        //取0~99列做為train
        Mat subMatTrainPcaFace = allPcaData.submat(0, 112, 0, 20);

        //取100~110列做為test
        Mat subMatTestPcaFace = allPcaData.submat(112, 126, 0, 20);

        //System.out.println("size="+subMatTestPcaFace.size()+","+subMatTestPcaFace.cols()+","+subMatTestPcaFace.rows());

        int personId = 0;

        try {
            FileWriter writer = new FileWriter(fileNameTrain);

            for (int i = 0; i < subMatTrainPcaFace.rows(); i++) { //14*8個人

                personId = i / 8; //每人8張圖

                //System.out.print((i+1)+" ");
                writer.write((personId + 1) + " ");

                for (int j = 0; j < subMatTrainPcaFace.cols(); j++) { //20個特徵
                    //System.out.print((j+1)+":"+nf.format(subMatTestPcaFace.t().get(j, i)[0])+" ");
                    writer.write((j + 1) + ":" + nf.format(subMatTrainPcaFace.t().get(j,
                                                           i)[0]) + " ");
                }

                //System.out.println("");
                writer.write("\n");
            }

            writer.close();

            //////////////////////////////////////



            writer = new FileWriter(fileNameTest);

            for (int i = 0; i < subMatTestPcaFace.rows(); i++) { //14個人

                //System.out.print((i+1)+" ");
                writer.write((i + 1) + " ");

                for (int j = 0; j < subMatTestPcaFace.cols(); j++) { //20個特徵
                    //System.out.print((j+1)+":"+nf.format(subMatTestPcaFace.t().get(j, i)[0])+" ");
                    writer.write((j + 1) + ":" + nf.format(subMatTestPcaFace.t().get(j,
                                                           i)[0]) + " ");
                }

                //System.out.println("");
                writer.write("\n");
            }

            writer.close();


        } catch (Exception e) {
            e.printStackTrace();
        }

    }


}
