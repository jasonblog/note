package ch13;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.TermCriteria;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

public class Ch13_14_5PCAForOcrClusterByKmean
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        OcrDatabase ocr = new OcrDatabase();

        Mat mean = new Mat();
        Mat vectors = new Mat();
        Mat ocrData = ocr.getTrainingDataMat().t();
        //System.out.println(allIrisData.dump());
        //System.out.println(allIrisData.size());


        Core.PCACompute(ocrData, mean, vectors, 2);

        //System.out.println(vectors.dump());
        //System.out.println(vectors.size());

        Mat labels = new Mat();


        Mat pcaOcr = vectors.t();
        TermCriteria criteria = new TermCriteria(TermCriteria.COUNT, 10, 1);
        Core.kmeans(pcaOcr, 10, labels, criteria, 12, Core.KMEANS_PP_CENTERS);
        System.out.println("labels=" + labels.dump());
    }
}
