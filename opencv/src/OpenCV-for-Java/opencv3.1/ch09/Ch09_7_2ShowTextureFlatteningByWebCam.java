package ch09;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;
import org.opencv.photo.Photo;
import org.opencv.videoio.VideoCapture;

public class Ch09_7_2ShowTextureFlatteningByWebCam
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }

    public static void main(String arg[])
    {
        VideoCapture capture = new VideoCapture();
        capture.open(0);
        JFrame frame1 = new JFrame("show image");
        frame1.setTitle("從webcam讀取影像至Java Swing視窗1");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(640, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);

        JFrame frame2 = new JFrame("show image");
        frame2.setTitle("從webcam讀取影像至Java Swing視窗2");
        frame2.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame2.setSize(640, 480);
        frame2.setBounds(160, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel2 = new Panel();
        frame2.setContentPane(panel2);
        frame2.setVisible(true);

        if (!capture.isOpened()) {
            System.out.println("Error");
        } else {
            Mat webcam_image = new Mat();

            capture.read(webcam_image);
            frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);
            frame2.setSize(webcam_image.width() + 50, webcam_image.height() + 70);

            while (true) {
                Mat destination = new Mat();
                Mat src_mask = new Mat(webcam_image.rows(), webcam_image.cols(),
                                       webcam_image.depth());

                capture.read(webcam_image);
                //System.out.println("從webcam讀取影像至Java Swing視窗");

                for (int i = 0; i < src_mask.rows(); i++) {
                    for (int j = 0; j < src_mask.cols(); j++) {
                        double[] data = new double[3];
                        data = src_mask.get(i, j);
                        data[0] = 255;
                        src_mask.put(i, j, data);
                    }
                }

                try {
                    Photo.textureFlattening(webcam_image, src_mask, destination, 5, 5, 5);

                    panel1.setimagewithMat(webcam_image);
                    panel2.setimagewithMat(destination);
                    frame1.repaint();
                    frame2.repaint();
                } catch (Exception e) {
                    // TODO: handle exception
                }

            }
        }
    }

}
