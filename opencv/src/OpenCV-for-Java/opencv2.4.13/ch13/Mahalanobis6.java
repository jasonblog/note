package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.highgui.Highgui;


//猜手寫數字,倆倆比較
public class Mahalanobis6
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {

        Mat trainingDataNumberMat = new Mat(2, 144, CvType.CV_32FC1);
        Mat source;

        Mat temp0 = Highgui.imread("C://opencv3.1//samples//ocr//number4.jpg",
                                   Highgui.CV_LOAD_IMAGE_GRAYSCALE);
        Mat temp2 = temp0.reshape(1, 144);
        Mat temp1 = new Mat(1, 144, CvType.CV_32F);

        for (int j = 0; j < 144; j++) {
            double[] data = new double[1];
            data = temp2.get(j, 0);
            temp1.put(0, j, data);
            trainingDataNumberMat.put(1, j, data);
        }


        for (int i = 0; i < 10; i++) {
            String tempFileNamw = "";

            source = Highgui.imread("C://opencv3.1//samples//ocr//" + i + "0.jpg",
                                    Highgui.CV_LOAD_IMAGE_GRAYSCALE);


            Mat temp = source.reshape(1, 144);

            for (int j = 0; j < 144; j++) {
                double[] data = new double[1];
                double[] data1 = new double[1];
                data = temp.get(j, 0);
                trainingDataNumberMat.put(0, j, data);

            }

            //               System.out.println("trainingDataNumberMat="+trainingDataNumberMat.dump());
            //               System.out.println("temp1="+temp1.dump());
            //               System.out.println("===========");

            Mat covar = new Mat(144, 144, CvType.CV_32F);
            Mat mean = new Mat(1, 144, CvType.CV_32F);
            Core.calcCovarMatrix(trainingDataNumberMat, covar, mean,
                                 Core.COVAR_ROWS | Core.COVAR_NORMAL, CvType.CV_32F);
            //               Core.divide(covar, Scalar.all(trainingDataNumberMat.rows()-1), covar);//covar=covar/(trainingDataMat.rows()-1)
            Core.invert(covar, covar, Core.DECOMP_SVD);
            //上2個省略,正確又很快
            //System.out.println("covar="+covar.dump());
            //System.out.println("mean="+mean.dump());
            Mat line0 = trainingDataNumberMat.row(0);
            //Mat line1 = trainingDataNumberMat.row(1);
            double d = Core.Mahalanobis(temp1, line0, covar);
            System.out.println("d2=" + d); //NaN:不是數字
        }



    }

}
