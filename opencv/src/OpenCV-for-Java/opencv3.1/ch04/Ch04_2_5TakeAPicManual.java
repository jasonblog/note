package ch04;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JButton;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.videoio.VideoCapture;

public class Ch04_2_5TakeAPicManual
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }

    public static void main(String arg[]) throws InterruptedException {
        VideoCapture capture = new VideoCapture();
        capture.open(0);
        JFrame frame1 = new JFrame("show image");
        frame1.setTitle("從webcam讀取影像至Java Swing視窗");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(680, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        frame1.getContentPane().setLayout(null);
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);

        JButton btnNewButton = new JButton("拍 照");
        btnNewButton.setEnabled(false);
        btnNewButton.setBounds(680, 97, 87, 23);
        frame1.getContentPane().add(btnNewButton);

        if (!capture.isOpened())
        {
            System.out.println("Error");
        } else
        {
            final Mat webcam_image = new Mat();
            capture.read(webcam_image);

            frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);

            btnNewButton.setEnabled(true);
            btnNewButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent arg0) {
                    Imgcodecs.imwrite("c:\\opencv3.1\\samples\\TakeApicManual.jpg", webcam_image);
                    System.out.println("手動觸發拍照");
                }
            });

            while (true) {
                capture.read(webcam_image);
                Thread.sleep(500);
                capture.read(webcam_image);
                Thread.sleep(500);
                capture.read(webcam_image);
                Thread.sleep(500);
                System.out.println("使用webcam攝影拍照");
                panel1.setimagewithMat(webcam_image);
                frame1.repaint();
            }
        }
    }
}
