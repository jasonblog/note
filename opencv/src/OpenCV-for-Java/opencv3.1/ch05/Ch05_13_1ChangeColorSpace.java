package ch05;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import javax.swing.JButton;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

public class Ch05_13_1ChangeColorSpace
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
                    Ch05_13_1ChangeColorSpace window = new Ch05_13_1ChangeColorSpace();
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
    public Ch05_13_1ChangeColorSpace()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv 顏色空間轉換練習");
        frmjavaSwing.setBounds(100, 100, 560, 620);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(5, 60, image.getHeight() + 10, image.getWidth() + 10);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        JButton btnRK0 = new JButton("RGB2HLS");
        btnRK0.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(colorSpaceConversion(source, 0));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnRK0.setBounds(5, 10, 110, 23);
        frmjavaSwing.getContentPane().add(btnRK0);

        JButton btnK1 = new JButton("RGB2BGR");
        btnK1.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(colorSpaceConversion(source, 1));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnK1.setBounds(5, 43, 110, 23);
        frmjavaSwing.getContentPane().add(btnK1);

        JButton btnK2 = new JButton("RGB2HSV");
        btnK2.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(colorSpaceConversion(source, 2));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnK2.setBounds(118, 10, 98, 23);
        frmjavaSwing.getContentPane().add(btnK2);

        JButton btnK3 = new JButton("RGB2Lab");
        btnK3.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(colorSpaceConversion(source, 3));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnK3.setBounds(118, 43, 98, 23);
        frmjavaSwing.getContentPane().add(btnK3);

        JButton btnK4 = new JButton("RGB2Luv");
        btnK4.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(colorSpaceConversion(source, 4));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnK4.setBounds(226, 10, 100, 23);
        frmjavaSwing.getContentPane().add(btnK4);

        JButton btnK5 = new JButton("RGB2YUV");
        btnK5.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(colorSpaceConversion(source, 5));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnK5.setBounds(226, 43, 100, 23);
        frmjavaSwing.getContentPane().add(btnK5);

        JButton btnK6 = new JButton("RGB2YCrCb");
        btnK6.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(colorSpaceConversion(source, 6));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnK6.setBounds(336, 10, 110, 23);
        frmjavaSwing.getContentPane().add(btnK6);

        JButton btnK7 = new JButton("RGB2XYZ");
        btnK7.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(colorSpaceConversion(source, 7));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnK7.setBounds(336, 43, 98, 23);
        frmjavaSwing.getContentPane().add(btnK7);
    }

    public Mat colorSpaceConversion(Mat source, int type)
    {

        Mat destination = new Mat(source.rows(), source.cols(), source.type());

        if (type == 0) {
            Imgproc.cvtColor(source, destination, Imgproc.COLOR_RGB2HLS);
        } else  if (type == 1) {
            Imgproc.cvtColor(source, destination, Imgproc.COLOR_RGB2BGR);
        } else  if (type == 2) {
            Imgproc.cvtColor(source, destination, Imgproc.COLOR_RGB2HSV);
        } else  if (type == 3) {
            Imgproc.cvtColor(source, destination, Imgproc.COLOR_RGB2Lab);
        } else  if (type == 4) {
            Imgproc.cvtColor(source, destination, Imgproc.COLOR_RGB2Luv);
        } else  if (type == 5) {
            Imgproc.cvtColor(source, destination, Imgproc.COLOR_RGB2YUV);
        } else  if (type == 6) {
            Imgproc.cvtColor(source, destination, Imgproc.COLOR_RGB2YCrCb);
        } else  if (type == 7) {
            Imgproc.cvtColor(source, destination, Imgproc.COLOR_RGB2XYZ);
        }

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


