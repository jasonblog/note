package ch10;
import java.awt.image.BufferedImage;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JPanel;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;
import org.opencv.videoio.VideoCapture;

import javax.swing.JLabel;
import javax.swing.JTextField;

public class Ch10_14_1CheckHSVfromWebCam
{
    private static JTextField textField_H;
    private static JTextField textField_S;
    private static JTextField textField_V;
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }

    public static void main(String arg[])
    {
        VideoCapture capture = new VideoCapture();
        capture.open(0);
        JFrame frame1 = new JFrame("show image");
        frame1.setTitle("從webcam讀取影像及判斷HSV顏色值");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(557, 529);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        JPanel panel1 = new JPanel();
        frame1.setContentPane(panel1);
        panel1.setLayout(null);

        JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(21, 23, 384, 222);
        panel1.add(lblNewLabel);

        JLabel lblNewLabel_1 = new JLabel("");
        lblNewLabel_1.setBounds(21, 255, 384, 222);
        panel1.add(lblNewLabel_1);

        JLabel lblNewLabel_2 = new JLabel("HSV顏色空間");
        lblNewLabel_2.setBounds(430, 268, 105, 15);
        panel1.add(lblNewLabel_2);

        JLabel lblNewLabel_3 = new JLabel("圓圈內的顏色是:");
        lblNewLabel_3.setBounds(415, 303, 97, 15);
        panel1.add(lblNewLabel_3);

        textField_H = new JTextField();
        textField_H.setBounds(439, 335, 96, 21);
        panel1.add(textField_H);
        textField_H.setColumns(10);

        textField_S = new JTextField();
        textField_S.setBounds(439, 366, 96, 21);
        panel1.add(textField_S);
        textField_S.setColumns(10);

        textField_V = new JTextField();
        textField_V.setBounds(439, 397, 96, 21);
        panel1.add(textField_V);
        textField_V.setColumns(10);

        JLabel lblNewLabel_4 = new JLabel("H:");
        lblNewLabel_4.setBounds(415, 338, 20, 15);
        panel1.add(lblNewLabel_4);

        JLabel lblS = new JLabel("S:");
        lblS.setBounds(415, 369, 20, 15);
        panel1.add(lblS);

        JLabel lblV = new JLabel("V:");
        lblV.setBounds(415, 400, 20, 15);
        panel1.add(lblV);
        frame1.setVisible(true);

        if (!capture.isOpened()) {
            System.out.println("Error");
        } else {
            Mat webcam_image = new Mat();
            Mat hsv_image = new Mat();
            double[] pointScaleData = new double[3];
            capture.read(webcam_image);

            frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);

            while (true) {
                capture.read(webcam_image);
                BufferedImage newImage = matToBufferedImage(webcam_image);
                lblNewLabel.setIcon(new ImageIcon(newImage));
                Imgproc.cvtColor(webcam_image, hsv_image, Imgproc.COLOR_BGR2HSV);
                pointScaleData = hsv_image.get(hsv_image.rows() / 2, hsv_image.cols() / 2);
                Imgproc.circle(hsv_image, new Point(hsv_image.rows() / 2, hsv_image.cols() / 2),
                               10, new Scalar(0, 0, 0), 3);
                Imgproc.line(hsv_image, new Point(hsv_image.rows() / 2,
                                                  hsv_image.cols() / 2 - 10), new Point(hsv_image.rows() / 2,
                                                          hsv_image.cols() / 2 - 70), new Scalar(0, 0, 0), 3);
                Imgproc.line(hsv_image, new Point(hsv_image.rows() / 2,
                                                  hsv_image.cols() / 2 - 70), new Point(hsv_image.rows() / 2 + 150,
                                                          hsv_image.cols() / 2 - 70), new Scalar(0, 0, 0), 3);
                textField_H.setText(String.valueOf(pointScaleData[0]));
                textField_S.setText(String.valueOf(pointScaleData[1]));
                textField_V.setText(String.valueOf(pointScaleData[2]));

                BufferedImage hsvImage = matToBufferedImage(hsv_image);
                lblNewLabel_1.setIcon(new ImageIcon(hsvImage));
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
