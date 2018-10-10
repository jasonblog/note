package ch13;

import java.util.ArrayList;
import java.util.List;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.TermCriteria;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.utils.Converters;

public class Ch13_12_5KmeanClassificationForIris
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }

    public static void main(String[] args)
    {
        IrisDatabase iris = new IrisDatabase();

        Mat labels = new Mat();
        TermCriteria criteria = new TermCriteria(TermCriteria.COUNT, 10, 1);
        Core.kmeans(iris.getAllDataMat(), 3, labels, criteria, 12,
                    Core.KMEANS_PP_CENTERS);
        System.out.println("labels=" + labels.dump());

    }
}
