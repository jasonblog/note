package ch09;

import java.awt.EventQueue;
import java.awt.Graphics;
import java.awt.Panel;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.JLayeredPane;
import java.awt.Color;
import java.awt.image.BufferedImage;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.CLAHE;
import org.opencv.imgproc.Imgproc;

import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch09_13_1CLAHE
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    private JFrame frmjavaSwing;
    double alpha = 2;
    double beta = 50;
    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {

        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch09_13_1CLAHE window = new Ch09_13_1CLAHE();
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
    public Ch09_13_1CLAHE()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//scene_l.bmp",
                                            Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("讀取影像至Java Swing視窗");
        frmjavaSwing.setBounds(100, 100, 520, 452);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showClipLimitValue = new JLabel("0.1");
        showClipLimitValue.setBounds(287, 10, 46, 15);
        frmjavaSwing.getContentPane().add(showClipLimitValue);

        final JLabel showTileGridSizeValue = new JLabel("1");
        showTileGridSizeValue.setBounds(297, 35, 46, 15);
        frmjavaSwing.getContentPane().add(showTileGridSizeValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 340);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_beta = new JSlider();
        slider_beta.setValue(1);
        slider_beta.setMinimum(1);
        slider_beta.setBounds(85, 35, 200, 25);
        frmjavaSwing.getContentPane().add(slider_beta);

        final JSlider slider_alpha = new JSlider();
        slider_alpha.setMaximum(500);
        slider_alpha.setMinimum(1);
        slider_alpha.setValue(1);
        slider_alpha.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showClipLimitValue.setText((float)slider_alpha.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(CLAHE((float)slider_alpha.getValue()
                                         / 10, slider_beta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_alpha.setBounds(85, 10, 200, 25);
        frmjavaSwing.getContentPane().add(slider_alpha);

        JLabel lblAlpha = new JLabel("clipLimit:");
        lblAlpha.setBounds(10, 20, 65, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("tileGridSize:");
        lblBeta.setBounds(10, 45, 79, 15);
        frmjavaSwing.getContentPane().add(lblBeta);


        slider_beta.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showTileGridSizeValue.setText(slider_beta.getValue() + "");
                BufferedImage newImage = matToBufferedImage(CLAHE((float)slider_alpha.getValue()
                                         / 10, slider_beta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });





    }
    public Mat CLAHE(double clipLimit, double tileGridSize)
    {
        Mat source = Imgcodecs.imread("C://opencv3.1//samples//scene_l.bmp",
                                      Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
        Mat dst = new Mat();
        CLAHE clahe = Imgproc.createCLAHE(clipLimit, new Size(tileGridSize,
                                          tileGridSize));
        clahe.apply(source, dst);
        return dst;

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
