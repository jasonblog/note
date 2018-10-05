package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.highgui.Highgui;


//瞦も糶计,10计癬matrix,箇代计玥礚
public class Mahalanobis5
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {

        Mat trainingDataNumberMat = new Mat(10, 144, CvType.CV_32FC1);
        Mat source;

        for (int i = 0; i < 10; i++) {
            String tempFileNamw = "";

            source = Highgui.imread("C://opencv3.1//samples//ocr//" + i + "2.jpg",
                                    Highgui.CV_LOAD_IMAGE_GRAYSCALE);


            Mat temp = source.reshape(1, 144);

            for (int j = 0; j < 144; j++) {
                double[] data = new double[1];
                data = temp.get(j, 0);
                trainingDataNumberMat.put(i, j, data);

            }

        }

        Mat temp0 = Highgui.imread("C://opencv3.1//samples//ocr//number3.jpg",
                                   Highgui.CV_LOAD_IMAGE_GRAYSCALE);
        Mat temp = temp0.reshape(1, 144);
        Mat temp1 = new Mat(1, 144, CvType.CV_32F);

        for (int j = 0; j < 144; j++) {
            double[] data = new double[1];
            data = temp.get(j, 0);
            temp1.put(0, j, data);

        }

        System.out.println("temp1=" + temp1.dump());
        Mat covar = new Mat(144, 144, CvType.CV_32F);

        Mat mean = new Mat(1, 144, CvType.CV_32F);

        Core.calcCovarMatrix(trainingDataNumberMat, covar, mean,
                             Core.COVAR_ROWS | Core.COVAR_NORMAL, CvType.CV_32F);
        //Core.divide(covar, Scalar.all(trainingDataNumberMat.rows()-1), covar);//covar=covar/(trainingDataMat.rows()-1)
        //Core.invert(covar, covar, Core.DECOMP_SVD);
        //2菠,タ絋е
        //          System.out.println("covar="+covar.dump());
        //          System.out.println("mean="+mean.dump());
        Mat line0 = trainingDataNumberMat.row(0);

        Mat line1 = trainingDataNumberMat.row(1);
        System.out.println("line1=" + line1.dump());
        Mat line2 = trainingDataNumberMat.row(2);
        Mat line3 = trainingDataNumberMat.row(3);
        Mat line4 = trainingDataNumberMat.row(4);
        Mat line5 = trainingDataNumberMat.row(5);
        Mat line6 = trainingDataNumberMat.row(6);
        Mat line7 = trainingDataNumberMat.row(7);
        Mat line8 = trainingDataNumberMat.row(8);
        Mat line9 = trainingDataNumberMat.row(9);
        //Mat line10 = trainingDataNumberMat.row(10);
        //System.out.println("line3="+line3.dump());
        double d = Core.Mahalanobis(temp1, line0, covar);
        System.out.println("d2=" + d);
        d = Core.Mahalanobis(temp1, line1, covar);
        System.out.println("d2=" + d); //NaN:ぃ琌计
        d = Core.Mahalanobis(temp1, line2, covar);
        System.out.println("d2=" + d); //NaN:ぃ琌计
        d = Core.Mahalanobis(temp1, line3, covar);
        System.out.println("d2=" + d); //NaN:ぃ琌计
        d = Core.Mahalanobis(temp1, line4, covar);
        System.out.println("d2=" + d); //NaN:ぃ琌计
        d = Core.Mahalanobis(temp1, line5, covar);
        System.out.println("d2=" + d); //NaN:ぃ琌计
        d = Core.Mahalanobis(temp1, line6, covar);
        System.out.println("d2=" + d); //NaN:ぃ琌计
        d = Core.Mahalanobis(temp1, line7, covar);
        System.out.println("d2=" + d); //NaN:ぃ琌计
        d = Core.Mahalanobis(temp1, line8, covar);
        System.out.println("d2=" + d); //NaN:ぃ琌计
        d = Core.Mahalanobis(temp1, line9, covar);
        System.out.println("d2=" + d); //NaN:ぃ琌计
    }

}
