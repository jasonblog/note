package ch12;
import java.util.ArrayList;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;
import org.opencv.video.KalmanFilter;
import org.opencv.videoio.VideoCapture;
import org.opencv.core.CvType;


public class Ch12_7_2KalmanTrackingBuleBall
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }


    public static void main(String arg[]) throws Exception{
        JFrame frame1 = new JFrame("Camera");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(640, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);


        Mat hsv_image = new Mat();
        Mat thresholded = new Mat();
        Mat circles = new Mat();
        //-- 2. Read the video stream
        VideoCapture capture = new VideoCapture(); //0表第1支CCD,1是第2支
        capture.open(0);
        Mat webcam_image = new Mat();
        KalmanFilter kalman;
        Mat measurement;
        Point mousePoint = new Point(200, 200);
        //記錄行徑路徑
        ArrayList<Point> CorrectPath = new ArrayList<Point>(5);
        ArrayList<Point> PredictPath = new ArrayList<Point>(5);

        CorrectPath.add(mousePoint);
        Mat mRgba;


        capture.read(webcam_image);
        frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);

        if (capture.isOpened())
        {

            kalman = new KalmanFilter(4, 2, 0, CvType.CV_32F);

            Mat transitionMatrix = new Mat(4, 4, CvType.CV_32F, new Scalar(0));
            float[] tM = {
                1, 0, 1, 0,
                0, 1, 0, 1,
                0, 0, 1, 0,
                0, 0, 0, 1
            } ;
            transitionMatrix.put(0, 0, tM);
            kalman.set_transitionMatrix(transitionMatrix);
            Mat state = new Mat(4, 1, CvType.CV_32FC1);
            measurement = new Mat(2, 1, CvType.CV_32F, new Scalar(0));

            Mat processNoise = new Mat(4, 1, CvType.CV_32F);
            kalman.set_measurementMatrix(Mat.eye(2, 4, CvType.CV_32F));

            Mat processNoiseCov = Mat.eye(4, 4, CvType.CV_32F);
            processNoiseCov = processNoiseCov.mul(processNoiseCov, 1e-5);
            kalman.set_processNoiseCov(processNoiseCov);

            Mat id1 = Mat.eye(2, 2, CvType.CV_32F);
            id1 = id1.mul(id1, 1e-1);
            kalman.set_measurementNoiseCov(id1);

            Mat id2 = Mat.eye(4, 4, CvType.CV_32F);
            //id2 = id2.mul(id2,0.1);
            kalman.set_errorCovPost(id2);

            //Core.randn(kalman.get_statePost(), 0, 0.1);


            Point oldP = new Point();
            Point currentP = new Point();

            //抓到藍色ok
            Scalar hsv_min = new Scalar(100, 90, 90, 0);
            Scalar hsv_max = new Scalar(140, 255, 255, 0);
            Point center = new Point();

            while (true) {
                //Thread.sleep(200);



                capture.read(webcam_image);

                if (!webcam_image.empty()) {
                    mRgba = webcam_image.clone();

                    // 轉換
                    Imgproc.cvtColor(webcam_image, hsv_image, Imgproc.COLOR_BGR2HSV);
                    Core.inRange(hsv_image, hsv_min, hsv_max, thresholded);
                    // 除噪
                    Imgproc.GaussianBlur(thresholded, thresholded, new Size(9, 9), 0, 0);
                    //霍夫曼圓變換
                    Imgproc.HoughCircles(thresholded, circles, Imgproc.CV_HOUGH_GRADIENT, 2,
                                         thresholded.height() / 4, 500, 50, 0, 0);
                    // 符合圓且是藍繪出外框
                    int rows = circles.rows();
                    int elemSize = (int)circles.elemSize(); // Returns 12 (3 * 4bytes in a float)
                    float[] data2 = new float[rows * elemSize / 4];

                    if (data2.length > 0) {
                        circles.get(0, 0, data2);

                        for (int i = 0; i < data2.length; i = i + 3) {
                            center = new Point(data2[i], data2[i + 1]);
                            //Core.ellipse( this, center, new Size( rect.width*0.5, rect.height*0.5), 0, 0, 360, new Scalar( 255, 0, 255 ), 4, 8, 0 );
                            Imgproc.ellipse(webcam_image, center, new Size((double)data2[i + 2],
                                            (double)data2[i + 2]), 0, 0, 360, new Scalar(255, 0, 255), 4, 8, 0);
                        }
                    }




                    //Point statePt = new Point( kalman.get_statePost().get(0, 0)[0], kalman.get_statePost().get(1, 0)[0]);
                    //Point statePt = new Point( kalman.get_statePre().get(0, 0)[0], kalman.get_statePre().get(1, 0)[0]);
                    Point statePt = new Point(kalman.correct(measurement).get(0, 0)[0],
                                              kalman.correct(measurement).get(1, 0)[0]);

                    //2.kalman prediction
                    Mat prediction = kalman.predict();
                    //Mat prediction = kalman.get_statePost();
                    //System.out.println("prediction="+prediction.dump());
                    Point predictPt = new Point(prediction.get(0, 0)[0], prediction.get(1, 0)[0]);
                    Point NextPt = new Point(prediction.get(0, 0)[0] + prediction.get(2, 0)[0] * 10,
                                             prediction.get(1, 0)[0] + prediction.get(3, 0)[0] * 10);

                    //System.out.println("prediction1="+prediction.get(2, 0)[0]);
                    //kalman.set_statePost(kalman.get_statePre().clone());
                    //kalman.set_errorCovPost(kalman.get_errorCovPost());
                    if (PredictPath.size() < 5) {
                        PredictPath.add(new Point(prediction.get(0, 0)[0], prediction.get(1, 0)[0]));
                    } else if (PredictPath.size() == 5) {
                        PredictPath.remove(0);
                        PredictPath.add(new Point(prediction.get(0, 0)[0], prediction.get(1, 0)[0]));
                    }



                    //3.update measurement
                    double newVL[] = new double[2];
                    newVL[0] = center.x;
                    newVL[1] = center.y;
                    mousePoint.set(newVL);

                    if (CorrectPath.size() < 5) {
                        CorrectPath.add(new Point(newVL[0], newVL[1]));
                    } else if (CorrectPath.size() == 5) {
                        CorrectPath.remove(0);
                        CorrectPath.add(new Point(newVL[0], newVL[1]));
                    }




                    measurement.put(0, 0, mousePoint.x);
                    measurement.put(1, 0, mousePoint.y);



                    //System.out.println((int)mousePoint.x+","+(int)mousePoint.y+","+Math.random()*300+13);
                    //4.update
                    Mat estimate = kalman.correct(measurement);
                    //System.out.println("estimate="+estimate.dump());
                    Point estimatePt = new Point(estimate.get(0, 0)[0], estimate.get(1, 0)[0]);
                    Imgproc.circle(webcam_image, NextPt, 5, new Scalar(255, 0, 0),
                                   2); //former point,Blue
                    Imgproc.circle(webcam_image, predictPt, 15, new Scalar(0, 255, 0),
                                   6); //predict point,Green
                    Imgproc.circle(webcam_image, mousePoint, 5, new Scalar(0, 0, 255),
                                   2); //ture point,Red


                    //print tract
                    //System.out.println("CorrectPath.size()="+CorrectPath.size());
                    for (int i = 1; i < CorrectPath.size(); i++) {
                        oldP = CorrectPath.get(i - 1);
                        //System.out.println("oldP="+oldP.x+","+oldP.y);
                        currentP = CorrectPath.get(i);
                        //System.out.println("currentP="+currentP.x+","+currentP.y);
                        Imgproc.line(webcam_image, oldP, currentP, new Scalar(0, 0, 255), 2);
                    }

                    for (int i = 1; i < PredictPath.size(); i++) {
                        oldP = PredictPath.get(i - 1);
                        //System.out.println("oldP="+oldP.x+","+oldP.y);
                        currentP = PredictPath.get(i);
                        //System.out.println("currentP="+currentP.x+","+currentP.y);
                        Imgproc.line(webcam_image, oldP, currentP, new Scalar(0, 255, 0), 2);
                    }


                    //System.out.println("=============");




                    panel1.setimagewithMat(webcam_image);
                    frame1.repaint();
                } else {
                    System.out.println("無補抓任何畫面!");
                    break;
                }
            }
        }

        return;
    }

}
