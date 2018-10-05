package ch11;
import org.opencv.calib3d.Calib3d;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Size;
import org.opencv.core.TermCriteria;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

public class Ch11_8_1FindChessboardCorners
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        try {
            Mat source = Imgcodecs.imread("C://opencv3.1//samples//002.jpg",
                                          Imgcodecs.CV_LOAD_IMAGE_COLOR);
            Mat destination = source.clone();
            Imgproc.cvtColor(source, destination, Imgproc.COLOR_BGR2GRAY);
            Size boarderSize = new Size(9, 6);
            MatOfPoint2f imageCorners = new MatOfPoint2f();
            boolean found = Calib3d.findChessboardCorners(destination, boarderSize,
                            imageCorners, Calib3d.CALIB_CB_ADAPTIVE_THRESH +
                            Calib3d.CALIB_CB_NORMALIZE_IMAGE + Calib3d.CALIB_CB_FAST_CHECK);

            if (found) {
                Imgproc.cornerSubPix(destination, imageCorners, new Size(11, 11), new Size(-1,
                                     -1), new TermCriteria(TermCriteria.EPS | TermCriteria.MAX_ITER, 30, 0.1));
                Calib3d.drawChessboardCorners(source, boarderSize, imageCorners, found);
                Imgcodecs.imwrite("C://opencv3.1//samples///test-Calib3d-2.jpg", source);

            }

        } catch (Exception e) {
            System.out.println("error: " + e.getMessage());
        }
    }
}