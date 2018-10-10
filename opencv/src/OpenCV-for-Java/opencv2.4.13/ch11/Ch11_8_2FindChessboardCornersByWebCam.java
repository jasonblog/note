package ch11;
import javax.swing.JFrame;
import org.opencv.calib3d.Calib3d;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Size;
import org.opencv.core.TermCriteria;
import org.opencv.highgui.VideoCapture;
import org.opencv.imgproc.Imgproc;

public class Ch11_8_2FindChessboardCornersByWebCam
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String arg[]) throws Exception{
        // It is better to group all frames together so cut and paste to
        // create more frames is easier
        JFrame frame1 = new JFrame("尋找出棋盤格Live版");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(640, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);

        //-- 2. Read the video stream
        VideoCapture capture = new VideoCapture(0);
        Mat webcam_image = new Mat();

        capture.read(webcam_image);
        frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);

        if (capture.isOpened())
        {
            while (true) {
                Thread.sleep(200);


                capture.read(webcam_image);

                if (!webcam_image.empty()) {

                    Mat destination = webcam_image.clone();
                    Imgproc.cvtColor(destination, destination, Imgproc.COLOR_BGR2GRAY);
                    Size boarderSize = new Size(9, 6);
                    MatOfPoint2f imageCorners = new MatOfPoint2f();
                    boolean found = Calib3d.findChessboardCorners(destination, boarderSize,
                            imageCorners, Calib3d.CALIB_CB_ADAPTIVE_THRESH +
                            Calib3d.CALIB_CB_NORMALIZE_IMAGE + Calib3d.CALIB_CB_FAST_CHECK);

                    if (found) {
                        Imgproc.cornerSubPix(destination, imageCorners, new Size(11, 11), new Size(-1,
                                             -1), new TermCriteria(TermCriteria.EPS | TermCriteria.MAX_ITER, 30, 0.1));
                        Calib3d.drawChessboardCorners(webcam_image, boarderSize, imageCorners, found);
                    }

                    panel1.setimagewithMat(webcam_image);
                    frame1.repaint();
                } else {
                    System.out.println(" 無補抓任何畫面!");
                    break;
                }
            }
        }

        return;
    }




}
