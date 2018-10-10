package ch05;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.highgui.Highgui;

public class Ch05_1_1BrightnessUseConvertTo
{

    //變暗
    //      static double alpha = 0.3;
    //      static double beta = -50;

    static double alpha = 2;
    static double beta = 50;
    public static void main(String[] args)
    {
        try {
            System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
            Mat source = Highgui.imread("C://opencv3.1//samples//lena.jpg",
                                        Highgui.CV_LOAD_IMAGE_COLOR);
            Mat destination = new Mat(source.rows(), source.cols(),
                                      source.type());
            source.convertTo(destination, -1, alpha, beta);
            Highgui.imwrite("C://opencv3.1//samples//lena-brightWithAlpha2Beta50.jpg",
                            destination);
        } catch (Exception e) {
            System.out.println("error: " + e.getMessage());
        }
    }
}
