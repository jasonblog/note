package ch11;

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
import org.opencv.core.MatOfPoint;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;

import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch11_2_1CornerHarrisStephens
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
                    Ch11_2_1CornerHarrisStephens window = new Ch11_2_1CornerHarrisStephens();
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
    public Ch11_2_1CornerHarrisStephens()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Highgui.imread("C://opencv3.1//samples//lena.jpg",
                                          Highgui.CV_LOAD_IMAGE_COLOR);

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("Harris-Stephens邊緣檢測方法");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showMCValue = new JLabel("1");
        showMCValue.setBounds(262, 10, 46, 15);
        frmjavaSwing.getContentPane().add(showMCValue);

        final JLabel showQLValue = new JLabel("1");
        showQLValue.setBounds(262, 43, 37, 15);
        frmjavaSwing.getContentPane().add(showQLValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_ks = new JSlider();
        slider_ks.setMaximum(31);
        slider_ks.setValue(1);
        slider_ks.setMinimum(1);
        slider_ks.setBounds(81, 35, 171, 25);
        frmjavaSwing.getContentPane().add(slider_ks);

        final JSlider slider_bs = new JSlider();
        slider_bs.setMinimum(1);
        slider_bs.setValue(2);

        slider_bs.setBounds(81, 10, 171, 25);
        frmjavaSwing.getContentPane().add(slider_bs);

        JLabel lblAlpha = new JLabel("blockSize");
        lblAlpha.setBounds(10, 20, 77, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("ksize");
        lblBeta.setBounds(10, 45, 77, 15);
        frmjavaSwing.getContentPane().add(lblBeta);

        JLabel lblMindistance = new JLabel("k");
        lblMindistance.setBounds(298, 18, 77, 15);
        frmjavaSwing.getContentPane().add(lblMindistance);

        final JSlider slider_k = new JSlider();
        slider_k.setMaximum(10);
        slider_k.setValue(1);
        slider_k.setMinimum(1);

        slider_k.setBounds(305, 35, 143, 25);
        frmjavaSwing.getContentPane().add(slider_k);

        final JLabel showMDValue = new JLabel("0.01");
        showMDValue.setBounds(456, 43, 46, 15);
        frmjavaSwing.getContentPane().add(showMDValue);


        slider_ks.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                if (slider_ks.getValue() % 2 == 0) {
                    slider_ks.setValue(slider_ks.getValue() + 1);
                }

                if (slider_ks.getValue() >= 32) {
                    slider_ks.setValue(31);
                }

                showQLValue.setText(slider_ks.getValue() + "");
                BufferedImage newImage = matToBufferedImage(Harris(slider_bs.getValue(),
                                         slider_ks.getValue(), (float)slider_k.getValue() / 100));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_k.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showMDValue.setText((float)slider_k.getValue() / 100 + "");
                BufferedImage newImage = matToBufferedImage(Harris(slider_bs.getValue(),
                                         slider_ks.getValue(), (float)slider_k.getValue() / 100));
                lblNewLabel.setIcon(new ImageIcon(newImage));

            }
        });
        slider_bs.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showMCValue.setText(slider_bs.getValue() + "");
                BufferedImage newImage = matToBufferedImage(Harris(slider_bs.getValue(),
                                         slider_ks.getValue(), (float)slider_k.getValue() / 100));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat Harris(int blockSize, int ksize, double k)
    {
        Mat source = Highgui.imread("C://opencv3.1//samples//lena.jpg",
                                    Highgui.CV_LOAD_IMAGE_COLOR);
        Mat destination = source.clone();
        Imgproc.cvtColor(destination, destination, Imgproc.COLOR_BGR2GRAY);
        Imgproc.Canny(destination, destination, 200, 200);
        Imgproc.cornerHarris(destination, destination, blockSize, ksize, k, 1);
        Core.normalize(destination, destination, 0, 255, Core.NORM_MINMAX,
                       CvType.CV_32FC1, new Mat());
        Core.convertScaleAbs(destination, destination);
        int thresh = 190; //180更多點

        for (int y = 0; y < destination.height(); y++) {

            for (int x = 0; x < destination.width(); x++) {
                destination.get(y, x);

                if (destination.get(y, x)[0] > thresh) {
                    Core.circle(source, new Point(x, y), 6, new Scalar(0, 0, 255), 1, 8, 0);
                }
            }
        }

        return source;
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
