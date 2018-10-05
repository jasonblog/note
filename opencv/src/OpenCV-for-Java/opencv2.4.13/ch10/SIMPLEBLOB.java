package ch10;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfKeyPoint;
import org.opencv.features2d.FeatureDetector;
import org.opencv.features2d.KeyPoint;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;

import java.util.List;

public class SIMPLEBLOB
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {

        String sourcePath = "C:\\path_to_image\\05.png";

        Mat srcImgMat = Highgui.imread("C://opencv249//sources//samples//c//lena.jpg",
                                       Highgui.CV_LOAD_IMAGE_COLOR);

        if (srcImgMat == null) {
            System.out.println("Failed to load image at " + sourcePath);
            return;
        }

        System.out.println("Loaded image at " + sourcePath);

        MatOfKeyPoint matOfKeyPoints = new MatOfKeyPoint();

        FeatureDetector blobDetector = FeatureDetector.create(
                                           FeatureDetector.SIMPLEBLOB);
        blobDetector.detect(srcImgMat, matOfKeyPoints);

        System.out.println("Detected " + matOfKeyPoints.size() + " blobs in the image");

        List<KeyPoint> keyPoints = matOfKeyPoints.toList();

    }
}