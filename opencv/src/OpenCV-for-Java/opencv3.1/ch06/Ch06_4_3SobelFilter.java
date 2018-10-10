package ch06;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch06_4_3SobelFilter
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
                    Ch06_4_3SobelFilter window = new Ch06_4_3SobelFilter();
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
    public Ch06_4_3SobelFilter()
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
        frmjavaSwing.setTitle("opencv Sobel API½m²ß2");
        frmjavaSwing.setBounds(100, 100, 560, 620);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(5, 60, image.getHeight() + 10, image.getWidth() + 10);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        JLabel lblKsize = new JLabel("Ksize:");
        lblKsize.setBounds(179, 10, 46, 15);
        frmjavaSwing.getContentPane().add(lblKsize);

        final JSlider slider_Ksize = new JSlider();
        slider_Ksize.setMinimum(1);
        slider_Ksize.setMaximum(31);

        slider_Ksize.setValue(1);
        slider_Ksize.setBounds(224, 0, 122, 25);
        frmjavaSwing.getContentPane().add(slider_Ksize);

        final JLabel lblKsize_Val = new JLabel("1");
        lblKsize_Val.setBounds(345, 10, 46, 15);
        frmjavaSwing.getContentPane().add(lblKsize_Val);

        JLabel lblScale = new JLabel(" Scale:");
        lblScale.setBounds(179, 35, 42, 15);
        frmjavaSwing.getContentPane().add(lblScale);

        final JSlider slider_Scale = new JSlider();
        slider_Scale.setMinimum(1);

        slider_Scale.setValue(1);
        slider_Scale.setBounds(224, 31, 121, 25);
        frmjavaSwing.getContentPane().add(slider_Scale);

        final JLabel lblScale_Val = new JLabel("1");
        lblScale_Val.setBounds(345, 35, 46, 15);
        frmjavaSwing.getContentPane().add(lblScale_Val);

        JLabel lblNewLabel_1 = new JLabel("delta");
        lblNewLabel_1.setBounds(385, 35, 31, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_1);

        final JSlider slider_Delta = new JSlider();

        slider_Delta.setValue(0);
        slider_Delta.setBounds(426, 35, 87, 25);
        frmjavaSwing.getContentPane().add(slider_Delta);

        final JLabel lblDelta_Val = new JLabel("0");
        lblDelta_Val.setBounds(455, 20, 19, 15);
        frmjavaSwing.getContentPane().add(lblDelta_Val);

        JLabel lblNewLabel_2 = new JLabel("dx");
        lblNewLabel_2.setBounds(5, 10, 17, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_2);

        JLabel lblNewLabel_3 = new JLabel("dy");
        lblNewLabel_3.setBounds(5, 35, 24, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_3);

        final JSlider slider_dx = new JSlider();
        slider_dx.setValue(1);
        slider_dx.setMaximum(2);

        slider_dx.setBounds(26, 0, 87, 25);
        frmjavaSwing.getContentPane().add(slider_dx);

        final JSlider slider_dy = new JSlider();
        slider_dy.setMaximum(2);
        slider_dy.setValue(1);

        slider_dy.setBounds(26, 35, 87, 25);
        frmjavaSwing.getContentPane().add(slider_dy);

        final JLabel lblNewLabel_dx = new JLabel("1");
        lblNewLabel_dx.setBounds(119, 10, 31, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_dx);

        final JLabel lblNewLabel_dy = new JLabel("1");
        lblNewLabel_dy.setBounds(119, 35, 31, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_dy);

        slider_Ksize.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {

                if (slider_Ksize.getValue() % 2 == 0) {
                    slider_Ksize.setValue(slider_Ksize.getValue() + 1);
                }

                lblKsize_Val.setText(slider_Ksize.getValue() + "");
                BufferedImage newImage = matToBufferedImage(Sobel(source, slider_dx.getValue(),
                                         slider_dy.getValue(), slider_Ksize.getValue(), slider_Scale.getValue(),
                                         slider_Delta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_Scale.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                lblScale_Val.setText(slider_Scale.getValue() + "");
                BufferedImage newImage = matToBufferedImage(Sobel(source, slider_dx.getValue(),
                                         slider_dy.getValue(), slider_Ksize.getValue(), slider_Scale.getValue(),
                                         slider_Delta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));

            }
        });

        slider_Delta.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                lblDelta_Val.setText(slider_Delta.getValue() + "");
                BufferedImage newImage = matToBufferedImage(Sobel(source, slider_dx.getValue(),
                                         slider_dy.getValue(), slider_Ksize.getValue(), slider_Scale.getValue(),
                                         slider_Delta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_dx.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                lblNewLabel_dx.setText(slider_dx.getValue() + "");
                BufferedImage newImage = matToBufferedImage(Sobel(source, slider_dx.getValue(),
                                         slider_dy.getValue(), slider_Ksize.getValue(), slider_Scale.getValue(),
                                         slider_Delta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_dy.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                lblNewLabel_dy.setText(slider_dy.getValue() + "");
                BufferedImage newImage = matToBufferedImage(Sobel(source, slider_dx.getValue(),
                                         slider_dy.getValue(), slider_Ksize.getValue(), slider_Scale.getValue(),
                                         slider_Delta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

    }

    public Mat Sobel(Mat source, int dx, int dy, int ksize, double scale,
                     double delta)
    {
        if ((dx == 0) && (dy == 0)) {
            dx = 1;
        }

        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Imgproc.Sobel(source, destination, -1, dx, dy, ksize, scale, delta);
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
