package ch04;
import java.awt.image.BufferedImage;
import org.opencv.videoio.VideoCapture;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import javax.swing.JLabel;

public class Ch04_3_2ReadAviNoPanel
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }

    public static void main(String arg[])
    {
        VideoCapture capture = new VideoCapture("C://opencv3.1//samples//tree.avi");
        JFrame frame1 = new JFrame("show image");
        frame1.setTitle("從webcam讀取影像至Java Swing視窗");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(672, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());


        JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(21, 23, 571, 413);
        frame1.add(lblNewLabel);
        frame1.setVisible(true);

        if (!capture.isOpened()) {
            System.out.println("Error");
        } else {
            Mat webcam_image = new Mat();
            capture.read(webcam_image);
            frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);

            while (true) {
                capture.read(webcam_image);

                if (!webcam_image.empty()) {
                    BufferedImage newImage = matToBufferedImage(webcam_image);
                    lblNewLabel.setIcon(new ImageIcon(newImage));
                    frame1.repaint();
                } else {
                    System.out.println("影片讀取結束!");
                    capture.release();
                    break;
                }

            }
        }
    }
    public static BufferedImage matToBufferedImage(Mat matrix)
    {
        int cols = matrix.cols();
        int rows = matrix.rows();
        int elemSize = (int)matrix.elemSize();
        byte[] data = new byte[cols * rows * elemSize];
        int type;
        matrix.get(0, 0, data);

        switch (matrix.channels()) {
        case 1:
            type = BufferedImage.TYPE_BYTE_GRAY;
            break;

        case 3:
            type = BufferedImage.TYPE_3BYTE_BGR;
            // bgr to rgb
            byte b;

            for (int i = 0; i < data.length; i = i + 3) {
                b = data[i];
                data[i] = data[i + 2];
                data[i + 2] = b;
            }

            break;

        default:
            return null;
        }

        BufferedImage image2 = new BufferedImage(cols, rows, type);
        image2.getRaster().setDataElements(0, 0, cols, rows, data);
        return image2;
    }
}
