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
import javax.swing.JButton;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch06_3_2LaplacianFilter
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
                    Ch06_3_2LaplacianFilter window = new Ch06_3_2LaplacianFilter();
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
    public Ch06_3_2LaplacianFilter()
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
        frmjavaSwing.setTitle("opencv Laplacian API½m²ß2");
        frmjavaSwing.setBounds(100, 100, 560, 620);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(5, 60, image.getHeight() + 10, image.getWidth() + 10);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        JLabel lblKsize = new JLabel("Ksize:");
        lblKsize.setBounds(10, 10, 46, 15);
        frmjavaSwing.getContentPane().add(lblKsize);

        final JSlider slider_Ksize = new JSlider();
        slider_Ksize.setMinimum(1);
        slider_Ksize.setMaximum(31);

        slider_Ksize.setValue(1);
        slider_Ksize.setBounds(50, 0, 200, 25);
        frmjavaSwing.getContentPane().add(slider_Ksize);

        final JLabel lblKsize_Val = new JLabel("1");
        lblKsize_Val.setBounds(253, 10, 46, 15);
        frmjavaSwing.getContentPane().add(lblKsize_Val);

        JLabel lblScale = new JLabel(" Scale:");
        lblScale.setBounds(5, 35, 42, 15);
        frmjavaSwing.getContentPane().add(lblScale);

        final JSlider slider_Scale = new JSlider();
        slider_Scale.setMinimum(1);

        slider_Scale.setValue(1);
        slider_Scale.setBounds(50, 35, 200, 25);
        frmjavaSwing.getContentPane().add(slider_Scale);

        final JLabel lblScale_Val = new JLabel("1");
        lblScale_Val.setBounds(253, 35, 46, 15);
        frmjavaSwing.getContentPane().add(lblScale_Val);

        JLabel lblNewLabel_1 = new JLabel("delta");
        lblNewLabel_1.setBounds(301, 10, 46, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_1);

        final JSlider slider_Delta = new JSlider();

        slider_Delta.setValue(0);
        slider_Delta.setBounds(283, 35, 180, 25);
        frmjavaSwing.getContentPane().add(slider_Delta);

        final JLabel lblDelta_Val = new JLabel("0");
        lblDelta_Val.setBounds(473, 35, 46, 15);
        frmjavaSwing.getContentPane().add(lblDelta_Val);

        slider_Ksize.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {

                if (slider_Ksize.getValue() % 2 == 0) {
                    slider_Ksize.setValue(slider_Ksize.getValue() + 1);
                }

                lblKsize_Val.setText(slider_Ksize.getValue() + "");
                BufferedImage newImage = matToBufferedImage(Laplacian(source,
                                         slider_Ksize.getValue(), slider_Scale.getValue(), slider_Delta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_Scale.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                lblScale_Val.setText(slider_Scale.getValue() + "");
                BufferedImage newImage = matToBufferedImage(Laplacian(source,
                                         slider_Ksize.getValue(), slider_Scale.getValue(), slider_Delta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));

            }
        });

        slider_Delta.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                lblDelta_Val.setText(slider_Delta.getValue() + "");
                BufferedImage newImage = matToBufferedImage(Laplacian(source,
                                         slider_Ksize.getValue(), slider_Scale.getValue(), slider_Delta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }

    public Mat Laplacian(Mat source, int ksize, double scale, double delta)
    {
        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Imgproc.Laplacian(source, destination, -1, ksize, scale, delta);
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
