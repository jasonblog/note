package ch12;
import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import javax.swing.JFrame;
import javax.swing.JPanel;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfByte;
import org.opencv.core.MatOfFloat;
import org.opencv.core.MatOfInt;
import org.opencv.core.MatOfInt4;
import org.opencv.core.MatOfPoint;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Point;
import org.opencv.core.Range;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;
import org.opencv.video.KalmanFilter;
import org.opencv.video.Video;
import org.opencv.videoio.VideoCapture;
import org.opencv.core.CvType;
import javax.sound.midi.MidiSystem;
import javax.sound.midi.Synthesizer;
import javax.sound.midi.MidiChannel;


public class Ch12_7_1KalmanRandomRunning
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

        //-- 2. Read the video stream
        VideoCapture capture = new VideoCapture(); //0表第1支CCD,1是第2支
        capture.open(0);
        Mat webcam_image = new Mat();
        KalmanFilter kalman;
        Mat measurement;
        Point LastResult;
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

            while (true) {
                //Thread.sleep(200);



                capture.read(webcam_image);

                if (!webcam_image.empty()) {
                    mRgba = webcam_image.clone();
                    Point statePt = new Point(kalman.get_statePost().get(0, 0)[0],
                                              kalman.get_statePost().get(1, 0)[0]);

                    //2.kalman prediction
                    Mat prediction = kalman.predict();
                    Point predictPt = new Point(prediction.get(0, 0)[0], prediction.get(1, 0)[0]);

                    if (PredictPath.size() < 5) {
                        PredictPath.add(new Point(prediction.get(0, 0)[0], prediction.get(1, 0)[0]));
                    } else if (PredictPath.size() == 5) {
                        PredictPath.remove(0);
                        PredictPath.add(new Point(prediction.get(0, 0)[0], prediction.get(1, 0)[0]));
                    }



                    //3.update measurement
                    double newVL[] = new double[2];
                    newVL[0] = Math.random() * 300 + 12;
                    newVL[1] = Math.random() * 300 + 11;
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
                    kalman.correct(measurement);

                    Imgproc.circle(webcam_image, statePt, 5, new Scalar(255, 0, 0),
                                   2); //former point,Blue
                    Imgproc.circle(webcam_image, predictPt, 5, new Scalar(0, 255, 0),
                                   2); //predict point,Green
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
