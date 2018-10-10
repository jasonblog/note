package ch11;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.highgui.VideoCapture;
import org.opencv.imgproc.Imgproc;
import org.opencv.core.CvType;

public class Ch11_4_2FindParticle
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

        JFrame frame2 = new JFrame("Threshold");
        frame2.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame2.setSize(640, 480);
        frame2.setBounds(300, 100, frame1.getWidth() + 50, 50 + frame1.getHeight());
        Panel panel2 = new Panel();
        frame2.setContentPane(panel2);
        frame2.setVisible(true);
        //-- 2. Read the video stream
        VideoCapture capture = new VideoCapture(1); //0表第1支CCD,1是第2支
        Mat webcam_image = new Mat();
        capture.read(webcam_image);
        frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);
        frame2.setSize(webcam_image.width() + 40, webcam_image.height() + 60);

        if (capture.isOpened())
        {
            while (true) {
                Thread.sleep(200);


                capture.read(webcam_image);

                if (!webcam_image.empty()) {
                    Mat destination = webcam_image.clone();
                    Imgproc.cvtColor(destination, destination, Imgproc.COLOR_BGR2GRAY);
                    //Imgproc.threshold(destination, destination, 85, 255, Imgproc.THRESH_BINARY);
                    Imgproc.Canny(destination, destination, 1, 50);
                    MatOfPoint corners = new MatOfPoint();

                    Imgproc.goodFeaturesToTrack(destination, corners, 23, 0.51,
                                                10);//destination, corners,
                    MatOfPoint2f Pts = new MatOfPoint2f();
                    corners.convertTo(Pts, CvType.CV_32FC2);
                    Point[] AllPoint = Pts.toArray();

                    //for(int y = 0; y < corners.size().height; y++){
                    for (Point px : AllPoint) {
                        Core.circle(webcam_image, px, 6, new Scalar(0, 0, 255));
                        //Core.circle(destination, new Point(corners[y],y), 4, new Scalar(0,0,255),2,8,0);
                    }

                    panel1.setimagewithMat(webcam_image);
                    panel2.setimagewithMat(destination);  //
                    frame1.repaint();
                    frame2.repaint();
                } else {
                    System.out.println("無補抓任何畫面!");
                    break;
                }
            }
        }

        return;
    }




}
