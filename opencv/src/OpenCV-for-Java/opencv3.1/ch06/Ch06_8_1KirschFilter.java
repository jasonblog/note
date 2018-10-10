package ch06;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import javax.swing.JButton;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

public class Ch06_8_1KirschFilter
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
                    Ch06_8_1KirschFilter window = new Ch06_8_1KirschFilter();
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
    public Ch06_8_1KirschFilter()
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
        frmjavaSwing.setTitle("opencv Kirsch filter½m²ß");
        frmjavaSwing.setBounds(100, 100, 560, 620);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(5, 60, image.getHeight() + 10, image.getWidth() + 10);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        JButton btnRK0 = new JButton("K0");
        btnRK0.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(Convolution(source, 0));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnRK0.setBounds(5, 10, 63, 23);
        frmjavaSwing.getContentPane().add(btnRK0);

        JButton btnK1 = new JButton("K1");
        btnK1.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(Convolution(source, 1));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnK1.setBounds(72, 10, 63, 23);
        frmjavaSwing.getContentPane().add(btnK1);

        JButton btnK2 = new JButton("K2");
        btnK2.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(Convolution(source, 2));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnK2.setBounds(136, 10, 63, 23);
        frmjavaSwing.getContentPane().add(btnK2);

        JButton btnK3 = new JButton("K3");
        btnK3.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(Convolution(source, 3));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnK3.setBounds(202, 10, 63, 23);
        frmjavaSwing.getContentPane().add(btnK3);

        JButton btnK4 = new JButton("K4");
        btnK4.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(Convolution(source, 4));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnK4.setBounds(268, 10, 63, 23);
        frmjavaSwing.getContentPane().add(btnK4);

        JButton btnK5 = new JButton("K5");
        btnK5.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(Convolution(source, 5));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnK5.setBounds(336, 10, 63, 23);
        frmjavaSwing.getContentPane().add(btnK5);

        JButton btnK6 = new JButton("K6");
        btnK6.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(Convolution(source, 6));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnK6.setBounds(403, 10, 63, 23);
        frmjavaSwing.getContentPane().add(btnK6);

        JButton btnK7 = new JButton("K7");
        btnK7.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(Convolution(source, 7));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        btnK7.setBounds(476, 10, 63, 23);
        frmjavaSwing.getContentPane().add(btnK7);
    }

    public Mat Convolution(Mat source, int type)
    {
        Mat kernel = new Mat(3, 3, CvType.CV_32F);

        if (type == 0) {
            kernel.put(0, 0, new float[] {-3, -3, 5});
            kernel.put(1, 0, new float[] {-3, 0, 5});
            kernel.put(2, 0, new float[] {-3, -3, 5});


        } else if (type == 1) {
            kernel.put(0, 0, new float[] {-3, 5, 5});
            kernel.put(1, 0, new float[] {-3, 0, 5});
            kernel.put(2, 0, new float[] {-3, -3, -3});

        } else if (type == 2) {
            kernel.put(0, 0, new float[] {5, 5, 5});
            kernel.put(1, 0, new float[] {-3, 0, -3});
            kernel.put(2, 0, new float[] {-3, -3, -3});

        } else if (type == 3) {
            kernel.put(0, 0, new float[] {5, 5, -3});
            kernel.put(1, 0, new float[] {5, 0, -3});
            kernel.put(2, 0, new float[] {-3, -3, -3});

        } else if (type == 4) {
            kernel.put(0, 0, new float[] {5, -3, -3});
            kernel.put(1, 0, new float[] {5, 0, -3});
            kernel.put(2, 0, new float[] {5, -3, -3});


        } else if (type == 5) {
            kernel.put(0, 0, new float[] {-3, -3, -3});
            kernel.put(1, 0, new float[] {5, 0, -3});
            kernel.put(2, 0, new float[] {5, 5, -3});


        } else if (type == 6) {
            kernel.put(0, 0, new float[] {-3, -3, -3});
            kernel.put(1, 0, new float[] {-3, 0, -3});
            kernel.put(2, 0, new float[] {5, 5, 5});


        } else if (type == 7) {
            kernel.put(0, 0, new float[] {-3, -3, -3});
            kernel.put(1, 0, new float[] {-3, 0, 5});
            kernel.put(2, 0, new float[] {-3, 5, 5});

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


