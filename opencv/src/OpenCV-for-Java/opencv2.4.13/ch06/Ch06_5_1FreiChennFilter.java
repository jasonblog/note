package ch06;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;
import javax.swing.JButton;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

public class Ch06_5_1FreiChennFilter
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
                    Ch06_5_1FreiChennFilter window = new Ch06_5_1FreiChennFilter();
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
    public Ch06_5_1FreiChennFilter()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Highgui.imread("C://opencv3.1//samples//lena.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv Frei Chenn filter練習");
        frmjavaSwing.setBounds(100, 100, 560, 620);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(5, 60, image.getHeight() + 10, image.getWidth() + 10);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        JButton btnX = new JButton("水平處理");
        btnX.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(Convolution(source, 1));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnX.setBounds(42, 10, 114, 23);
        frmjavaSwing.getContentPane().add(btnX);

        JButton btnY = new JButton("垂直處理");
        btnY.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(Convolution(source, 2));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnY.setBounds(211, 10, 102, 23);
        frmjavaSwing.getContentPane().add(btnY);
    }

    public Mat Convolution(Mat source, int type)
    {
        Mat kernel = new Mat(3, 3, CvType.CV_32F);

        if (type == 1) {

            kernel.put(0, 0, new float[] {-1, -1.414f, -1});
            kernel.put(1, 0, new float[] {0, 0, 0});
            kernel.put(2, 0, new float[] {1, 1.414f, 1});

        } else if (type == 2) {
            kernel.put(0, 0, new float[] {-1, 0, 1});
            kernel.put(1, 0, new float[] {-1.414f, 0, 1.414f});
            kernel.put(2, 0, new float[] {-1, 0, 1});

        }

        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Imgproc.filter2D(source, destination, -1, kernel);
        return destination;

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
