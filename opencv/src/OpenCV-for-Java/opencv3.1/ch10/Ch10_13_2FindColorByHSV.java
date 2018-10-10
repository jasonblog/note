package ch10;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import javax.swing.JButton;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

public class Ch10_13_2FindColorByHSV
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    private JFrame frmjavaSwing;
    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {

        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch10_13_2FindColorByHSV window = new Ch10_13_2FindColorByHSV();
                    window.frmjavaSwing.setVisible(true);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }

    /**
     * Create the application.
     */
    public Ch10_13_2FindColorByHSV()
    {
        initialize();

    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//color2.png");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv ÃC¦â¿ëÃÑ½m²ß");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(20, 81, 244, 164);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        JLabel lblAlpha = new JLabel("­ì¹Ï");
        lblAlpha.setBounds(20, 46, 28, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        final JLabel lblNewLabel_2 = new JLabel("");
        lblNewLabel_2.setBounds(21, 275, 243, 164);
        frmjavaSwing.getContentPane().add(lblNewLabel_2);

        JButton btnNewButton_red = new JButton("red(H:0~20)");
        btnNewButton_red.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent arg0) {
                BufferedImage newImage = matToBufferedImage(findColor(0, 20));
                lblNewLabel_2.setIcon(new ImageIcon(newImage));
            }
        });
        btnNewButton_red.setBounds(20, 10, 128, 23);
        frmjavaSwing.getContentPane().add(btnNewButton_red);

        JButton btnNewButton_blue = new JButton("blue(H:100~120)");
        btnNewButton_blue.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent arg0) {
                BufferedImage newImage = matToBufferedImage(findColor(100, 120));
                lblNewLabel_2.setIcon(new ImageIcon(newImage));
            }
        });
        btnNewButton_blue.setBounds(171, 10, 135, 23);
        frmjavaSwing.getContentPane().add(btnNewButton_blue);

        JButton btnNewButton_green = new JButton("green(H:60~80)");
        btnNewButton_green.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent arg0) {
                BufferedImage newImage = matToBufferedImage(findColor(60, 80));
                lblNewLabel_2.setIcon(new ImageIcon(newImage));
            }
        });
        btnNewButton_green.setBounds(316, 10, 128, 23);
        frmjavaSwing.getContentPane().add(btnNewButton_green);

        JButton btnYellow = new JButton("yellow(H:23~38)");
        btnYellow.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent arg0) {
                BufferedImage newImage = matToBufferedImage(findColor(23, 38));
                lblNewLabel_2.setIcon(new ImageIcon(newImage));
            }
        });
        btnYellow.setBounds(316, 57, 128, 23);
        frmjavaSwing.getContentPane().add(btnYellow);
    }
    public Mat findColor(int min, int max)
    {
        Mat source2 = Imgcodecs.imread("C://opencv3.1//samples//color2.png");
        Mat hsv_image = new Mat();
        Imgproc.GaussianBlur(source2, source2, new Size(3, 3), 0, 0);
        Imgproc.cvtColor(source2, hsv_image, Imgproc.COLOR_BGR2HSV);
        Mat thresholded = new Mat();
        Core.inRange(hsv_image, new Scalar(min, 90, 90), new Scalar(max, 255, 255),
                     thresholded);
        return thresholded;
    }

    public BufferedImage matToBufferedImage(Mat matrix)
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
