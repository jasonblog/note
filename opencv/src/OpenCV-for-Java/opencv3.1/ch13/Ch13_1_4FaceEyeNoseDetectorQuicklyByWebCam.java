package ch13;
import java.util.ArrayList;
import java.util.List;

import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfFloat6;
import org.opencv.core.MatOfInt;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.MatOfRect;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;
import org.opencv.imgproc.Subdiv2D;
import org.opencv.objdetect.CascadeClassifier;
import org.opencv.videoio.VideoCapture;


//handle the mouth
public class Ch13_1_4FaceEyeNoseDetectorQuicklyByWebCam
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }
    public static void main(String arg[])
    {
        VideoCapture capture = new VideoCapture();
        capture.open(0);
        JFrame frame1 = new JFrame("show image");
        frame1.setTitle("人臉+眼睛+鼻子檢測");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(640, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);


        if (!capture.isOpened()) {
            System.out.println("Error");
        } else {
            Mat webcam_image = new Mat();
            capture.read(webcam_image);
            frame1.setSize(webcam_image.width() + 10, webcam_image.height() + 20);

            CascadeClassifier faceDetector = new
            CascadeClassifier("C://opencv3.1//build//etc//haarcascades//haarcascade_frontalface_alt.xml");
            MatOfRect faceDetections = new MatOfRect();

            CascadeClassifier eyeDetector = new
            CascadeClassifier("C://opencv3.1//sources//data//haarcascades//haarcascade_mcs_eyepair_big.xml");
            CascadeClassifier noseDetector = new
            CascadeClassifier("C://opencv3.1//sources//data//haarcascades//haarcascade_mcs_nose.xml");
            MatOfRect eyeDetection = new MatOfRect();
            MatOfRect noseDetection = new MatOfRect();


            while (true) {
                capture.read(webcam_image);

                if (!webcam_image.empty()) {
                    faceDetector.detectMultiScale(webcam_image, faceDetections);

                    for (Rect rect : faceDetections.toArray()) {
                        Imgproc.rectangle(webcam_image, new Point(rect.x, rect.y),
                                          new Point(rect.x + rect.width, rect.y + rect.height),
                                          new Scalar(0, 255, 0), 3);


                        //眼睛於臉2/3處上方
                        int eyeStartX = rect.x + 10;
                        int eyeStartY = rect.y;

                        int eyeEndX = rect.x + rect.width - 10;
                        int eyeEndY = rect.y + rect.height * 2 / 3;


                        //鼻子於臉1/4處下方,位置窄於眼睛
                        int noseStartX = rect.x + 20;
                        int noseStartY = rect.y + rect.height * 1 / 4;
                        //int noseStartY=rect.y+20;

                        int noseEndX = rect.x + rect.width - 20;
                        int noseEndY = rect.height - 10;


                        Rect eyeRect = new Rect(new Point(eyeStartX, eyeStartY), new Point(eyeEndX,
                                                eyeEndY));
                        //Imgproc.rectangle(webcam_image, new Point(eyeStartX,eyeStartY),new Point(eyeEndX,eyeEndY),new Scalar(0, 25, 30),1);
                        Rect noseRect = new Rect(new Point(noseStartX, noseStartY), new Point(noseEndX,
                                                 noseEndY));

                        try {
                            Mat subWebcamForEye = webcam_image.submat(eyeRect);
                            eyeDetector.detectMultiScale(subWebcamForEye, eyeDetection);
                            Rect subRealEyeRect = eyeDetection.toArray()[0];
                            Imgproc.rectangle(subWebcamForEye, new Point(subRealEyeRect.x,
                                              subRealEyeRect.y), new Point(subRealEyeRect.x + subRealEyeRect.width,
                                                      subRealEyeRect.y + subRealEyeRect.height),
                                              new Scalar(255, 0, 0), 2);

                            Mat subWebcamForNose = webcam_image.submat(noseRect);
                            noseDetector.detectMultiScale(subWebcamForNose, noseDetection);
                            Rect subRealNoseRect = noseDetection.toArray()[0];
                            Imgproc.rectangle(subWebcamForNose, new Point(subRealNoseRect.x,
                                              subRealNoseRect.y), new Point(subRealNoseRect.x + subRealNoseRect.width,
                                                      subRealNoseRect.y + subRealNoseRect.height),
                                              new Scalar(0, 0, 255), 2);



                        } catch (Exception e) {
                            System.out.println(e);
                        }



                    }

                    System.out.println(String.format("Detected %s faces",
                                                     faceDetections.toArray().length));




                    panel1.setimagewithMat(webcam_image);
                    frame1.repaint();



                } else {
                    capture.release();
                    break;
                }
            }
        }
    }


}
