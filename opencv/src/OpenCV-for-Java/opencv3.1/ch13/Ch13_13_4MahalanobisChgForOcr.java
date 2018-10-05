package ch13;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.imgcodecs.Imgcodecs;


//瞦も糶计,10计癬matrix,箇代计玥礚
public class Ch13_13_4MahalanobisChgForOcr
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {

        Mat trainingDataNumberMat = new Mat(11, 144, CvType.CV_32FC1);
        Mat source;

        for (int i = 0; i < 10; i++) {

            source = Imgcodecs.imread("C://opencv3.1//samples//ocr//" + i + "0.jpg",
                                      Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);


            Mat temp = source.reshape(1, 144);

            for (int j = 0; j < 144; j++) {
                double[] data = new double[1];
                data = temp.get(j, 0);
                trainingDataNumberMat.put(i, j, data);

            }

        }

        Mat temp0 = Imgcodecs.imread("C://opencv3.1//samples//ocr//number3.jpg",
                                     Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
        Mat temp = temp0.reshape(1, 144);

        for (int j = 0; j < 144; j++) {
            double[] data = new double[1];
            data = temp.get(j, 0);
            trainingDataNumberMat.put(10, j, data);

        }

        //System.out.println("temp1="+temp1.dump());
        Mat covar = new Mat(144, 144, CvType.CV_32F);

        Mat mean = new Mat(1, 144, CvType.CV_32F);

        Core.calcCovarMatrix(trainingDataNumberMat, covar, mean,
                             Core.COVAR_ROWS | Core.COVAR_NORMAL, CvType.CV_32F);
        //       Core.invert(covar, covar, Core.DECOMP_SVD);

        //2菠,タ絋е
        //          System.out.println("covar="+covar.dump());
        //          System.out.println("mean="+mean.dump());
        Mat line0 = trainingDataNumberMat.row(0);
        Mat line1 = trainingDataNumberMat.row(1);
        Mat line2 = trainingDataNumberMat.row(2);
        Mat line3 = trainingDataNumberMat.row(3);
        Mat line4 = trainingDataNumberMat.row(4);
        Mat line5 = trainingDataNumberMat.row(5);
        Mat line6 = trainingDataNumberMat.row(6);
        Mat line7 = trainingDataNumberMat.row(7);
        Mat line8 = trainingDataNumberMat.row(8);
        Mat line9 = trainingDataNumberMat.row(9);
        Mat line10 = trainingDataNumberMat.row(10);
        //System.out.println("line3="+line3.dump());

        Map findMinium = new HashMap<String, Float>();
        Map findMiniumInv = new HashMap<String, Float>();

        double d = Core.Mahalanobis(line10, line0, covar);
        System.out.println("line10(计3)籔line0Mahalanobis禯瞒=" + d);
        findMinium.put("3,0", d);
        findMiniumInv.put(d, "3,0");
        d = Core.Mahalanobis(line10, line1, covar);
        findMinium.put("3,1", d);
        findMiniumInv.put(d, "3,1");
        System.out.println("line10(计3)籔line1Mahalanobis禯瞒=" + d); //NaN:ぃ琌计
        d = Core.Mahalanobis(line10, line2, covar);
        findMinium.put("3,2", d);
        findMiniumInv.put(d, "3,2");
        System.out.println("line10(计3)籔line2Mahalanobis禯瞒=" + d); //NaN:ぃ琌计
        d = Core.Mahalanobis(line10, line3, covar);
        findMinium.put("3,3", d);
        findMiniumInv.put(d, "3,3");
        System.out.println("line10(计3)籔line3Mahalanobis禯瞒=" + d); //NaN:ぃ琌计
        d = Core.Mahalanobis(line10, line4, covar);
        findMinium.put("3,4", d);
        findMiniumInv.put(d, "3,4");
        System.out.println("line10(计3)籔line4Mahalanobis禯瞒=" + d); //NaN:ぃ琌计
        d = Core.Mahalanobis(line10, line5, covar);
        findMinium.put("3,5", d);
        findMiniumInv.put(d, "3,5");
        System.out.println("line10(计3)籔line5Mahalanobis禯瞒=" + d); //NaN:ぃ琌计
        d = Core.Mahalanobis(line10, line6, covar);
        findMinium.put("3,6", d);
        findMiniumInv.put(d, "3,6");
        System.out.println("line10(计3)籔line6Mahalanobis禯瞒=" + d); //NaN:ぃ琌计
        d = Core.Mahalanobis(line10, line7, covar);
        findMinium.put("3,7", d);
        findMiniumInv.put(d, "3,7");
        System.out.println("line10(计3)籔line7Mahalanobis禯瞒=" + d); //NaN:ぃ琌计
        d = Core.Mahalanobis(line10, line8, covar);
        findMinium.put("3,8", d);
        findMiniumInv.put(d, "3,8");
        System.out.println("line10(计3)籔line8Mahalanobis禯瞒=" + d); //NaN:ぃ琌计
        d = Core.Mahalanobis(line10, line9, covar);
        findMinium.put("3,9", d);
        findMiniumInv.put(d, "3,9");
        System.out.println("line10(计3)籔line9Mahalanobis禯瞒=" + d); //NaN:ぃ琌计
        double min = (double) Collections.min(findMinium.values());

        System.out.println("程=" + min);
        System.out.println("舱琌=" + findMiniumInv.get(min));


    }

}
