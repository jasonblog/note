package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;

public class Ch13_14_1PCA
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {


        //第1組
        Mat data = new Mat(4, 4, CvType.CV_32F) {
            {
                put(0, 0, -1, 1, 0, 5);
                put(1, 0, 3, -4, 3, 0);
                put(2, 0, 2, 1, 0, 2);
                put(3, 0, 1, 2, 3, 2);
            }
        };
        System.out.println("data=" + data.dump());
        Mat mean = new Mat();
        Mat vectors = new Mat();

        Core.PCACompute(data, mean, vectors);

        System.out.println("mean" + mean.dump());
        System.out.println("vectors" + vectors.dump());
        System.out.println("////////////////////////////");

        //第2組
        Mat data2 = new Mat(2, 2, CvType.CV_32F) {
            {
                put(0, 0, 1, 0);
                put(1, 0, 0, 1);
            }
        };
        System.out.println("data2=" + data2.dump());
        Mat mean2 = new Mat();
        Mat vectors2 = new Mat();

        Core.PCACompute(data2, mean2, vectors2);

        System.out.println("mean2" + mean2.dump());
        System.out.println("vectors2" + vectors2.dump());



    }
}
