package ch11;
import java.util.ArrayList;

import org.opencv.calib3d.Calib3d;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.MatOfPoint3f;
import org.opencv.core.Point3;
import org.opencv.core.Size;
import org.opencv.core.TermCriteria;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;

public class Ch11_9_1Undistort
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String[] args)
    {
        try {
            Mat source = Highgui.imread("C://opencv3.1//samples//005.jpg",
                                        Highgui.CV_LOAD_IMAGE_COLOR);
            Mat destination = source.clone();
            // Mat destination1=new Mat();
            Imgproc.cvtColor(source, destination, Imgproc.COLOR_BGR2GRAY);
            Size boarderSize = new Size(9, 6);
            MatOfPoint2f imageCorners = new MatOfPoint2f();
            boolean found = Calib3d.findChessboardCorners(destination, boarderSize,
                            imageCorners, Calib3d.CALIB_CB_ADAPTIVE_THRESH +
                            Calib3d.CALIB_CB_NORMALIZE_IMAGE + Calib3d.CALIB_CB_FAST_CHECK);

            if (found) {
                TermCriteria term = new TermCriteria(TermCriteria.EPS | TermCriteria.MAX_ITER,
                                                     30, 0.1);
                Imgproc.cornerSubPix(destination, imageCorners, new Size(11, 11), new Size(-1,
                                     -1), term);
                Calib3d.drawChessboardCorners(source, boarderSize, imageCorners, found);

            }

            ArrayList<Mat> rvecs = new ArrayList<Mat>();
            ArrayList<Mat> tvecs = new ArrayList<Mat>();
            ArrayList<Mat> objectPoints = new ArrayList<Mat>();
            ArrayList<Mat> imagePoints = new ArrayList<Mat>();
            MatOfPoint3f obj = new MatOfPoint3f();
            Mat cameraMatrix = new Mat(3, 3, CvType.CV_32FC1);
            cameraMatrix.put(0, 0, 1);
            cameraMatrix.put(1, 1, 1);
            Mat distCoeffs = new Mat();
            Mat undistort = new Mat(source.size(), source.type());

            for (int j = 0; j < 54; j++) {
                //obj.push_back(new MatOfPoint3f(new Point3(j/6,j%9,0.0f)));
                obj.push_back(new MatOfPoint3f(new Point3(j / 6, j % 9, 0.0f))); //º§º¬3,3
            }

            objectPoints.add(obj);
            imagePoints.add(imageCorners);

            Calib3d.calibrateCamera(objectPoints, imagePoints, destination.size(),
                                    cameraMatrix, distCoeffs, rvecs, tvecs);
            Imgproc.undistort(source, undistort, cameraMatrix, distCoeffs);
            Highgui.imwrite("C://opencv3.1//samples//Calib3d-undistort.jpg", undistort);



        } catch (Exception e) {
            System.out.println("error: " + e.getMessage());
        }
    }
}