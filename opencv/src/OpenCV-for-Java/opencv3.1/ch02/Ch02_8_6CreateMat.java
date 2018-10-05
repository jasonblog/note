package ch02;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;

public class Ch02_8_6CreateMat
{
    static{System.loadLibrary(Core.NATIVE_LIBRARY_NAME);}
    public static void main(String[] args)
    {

        Mat matrix = new Mat(4, 4, CvType.CV_32F, new Scalar(0));
        float[] data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        matrix.put(0, 0, data);
        System.out.println(matrix.dump());


    }

}
