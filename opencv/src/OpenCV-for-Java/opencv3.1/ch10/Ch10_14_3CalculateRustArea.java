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
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch10_14_3CalculateRustArea
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
                    Ch10_14_3CalculateRustArea window = new Ch10_14_3CalculateRustArea();
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
    public Ch10_14_3CalculateRustArea()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//DSC_0869.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("讀取影像至Java Swing視窗");
        frmjavaSwing.setBounds(100, 100, 732, 711);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showAlphaValue = new JLabel("2");
        showAlphaValue.setBounds(127, 20, 46, 15);
        frmjavaSwing.getContentPane().add(showAlphaValue);

        final JLabel showBetaValue = new JLabel("50");
        showBetaValue.setBounds(127, 43, 46, 15);
        frmjavaSwing.getContentPane().add(showBetaValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 669, 283);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_Hmax = new JSlider();
        slider_Hmax.setMaximum(256);
        slider_Hmax.setBounds(47, 33, 70, 25);
        frmjavaSwing.getContentPane().add(slider_Hmax);

        final JSlider slider_Hmin = new JSlider();
        slider_Hmin.setMaximum(255);
        slider_Hmin.setValue(2);

        slider_Hmin.setBounds(47, 10, 76, 25);
        frmjavaSwing.getContentPane().add(slider_Hmin);

        JLabel lblAlpha = new JLabel("Min");
        lblAlpha.setBounds(10, 20, 46, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("Max");
        lblBeta.setBounds(10, 45, 46, 15);
        frmjavaSwing.getContentPane().add(lblBeta);

        JLabel lblH = new JLabel("H");
        lblH.setBounds(10, -1, 46, 15);
        frmjavaSwing.getContentPane().add(lblH);

        JLabel lblS = new JLabel("S");
        lblS.setBounds(163, -1, 46, 15);
        frmjavaSwing.getContentPane().add(lblS);

        JLabel label_1 = new JLabel("Min");
        label_1.setBounds(163, 20, 46, 15);
        frmjavaSwing.getContentPane().add(label_1);

        JLabel label_2 = new JLabel("Max");
        label_2.setBounds(163, 45, 46, 15);
        frmjavaSwing.getContentPane().add(label_2);

        final JSlider slider_Smin = new JSlider();

        slider_Smin.setValue(0);
        slider_Smin.setMaximum(255);
        slider_Smin.setBounds(200, 10, 76, 25);
        frmjavaSwing.getContentPane().add(slider_Smin);

        final JSlider slider_Smax = new JSlider();

        slider_Smax.setValue(250);
        slider_Smax.setMaximum(256);
        slider_Smax.setBounds(200, 33, 70, 25);
        frmjavaSwing.getContentPane().add(slider_Smax);

        final JLabel label_Smin = new JLabel("0");
        label_Smin.setBounds(280, 20, 46, 15);
        frmjavaSwing.getContentPane().add(label_Smin);

        final JLabel label_Smax = new JLabel("256");
        label_Smax.setBounds(280, 43, 46, 15);
        frmjavaSwing.getContentPane().add(label_Smax);

        JLabel lblV = new JLabel("V");
        lblV.setBounds(329, -1, 46, 15);
        frmjavaSwing.getContentPane().add(lblV);

        JLabel label_6 = new JLabel("Min");
        label_6.setBounds(329, 20, 46, 15);
        frmjavaSwing.getContentPane().add(label_6);

        JLabel label_7 = new JLabel("Max");
        label_7.setBounds(329, 45, 46, 15);
        frmjavaSwing.getContentPane().add(label_7);

        final JSlider slider_Vmin = new JSlider();

        slider_Vmin.setValue(0);
        slider_Vmin.setMaximum(255);
        slider_Vmin.setBounds(366, 10, 76, 25);
        frmjavaSwing.getContentPane().add(slider_Vmin);

        final JSlider slider_Vmax = new JSlider();

        slider_Vmax.setValue(256);
        slider_Vmax.setMaximum(256);
        slider_Vmax.setBounds(366, 33, 70, 25);
        frmjavaSwing.getContentPane().add(slider_Vmax);

        final JLabel label_Vmin = new JLabel("0");
        label_Vmin.setBounds(446, 20, 46, 15);
        frmjavaSwing.getContentPane().add(label_Vmin);

        final JLabel label_Vmax = new JLabel("256");
        label_Vmax.setBounds(446, 43, 46, 15);
        frmjavaSwing.getContentPane().add(label_Vmax);

        JLabel label = new JLabel("");
        label.setBounds(10, 391, 669, 283);
        label.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(label);
        slider_Hmin.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showAlphaValue.setText(slider_Hmin.getValue() + "");
                BufferedImage newImage = matToBufferedImage(matConvertTo(slider_Hmin.getValue(),
                                         slider_Hmax.getValue(), slider_Smin.getValue(), slider_Smax.getValue(),
                                         slider_Vmin.getValue(), slider_Vmax.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_Hmax.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showBetaValue.setText(slider_Hmax.getValue() + "");
                BufferedImage newImage = matToBufferedImage(matConvertTo(slider_Hmin.getValue(),
                                         slider_Hmax.getValue(), slider_Smin.getValue(), slider_Smax.getValue(),
                                         slider_Vmin.getValue(), slider_Vmax.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_Smin.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                label_Smin.setText(slider_Smin.getValue() + "");
                BufferedImage newImage = matToBufferedImage(matConvertTo(slider_Hmin.getValue(),
                                         slider_Hmax.getValue(), slider_Smin.getValue(), slider_Smax.getValue(),
                                         slider_Vmin.getValue(), slider_Vmax.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_Smax.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                label_Smax.setText(slider_Smax.getValue() + "");
                BufferedImage newImage = matToBufferedImage(matConvertTo(slider_Hmin.getValue(),
                                         slider_Hmax.getValue(), slider_Smin.getValue(), slider_Smax.getValue(),
                                         slider_Vmin.getValue(), slider_Vmax.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_Vmin.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                label_Vmin.setText(slider_Vmin.getValue() + "");
                BufferedImage newImage = matToBufferedImage(matConvertTo(slider_Hmin.getValue(),
                                         slider_Hmax.getValue(), slider_Smin.getValue(), slider_Smax.getValue(),
                                         slider_Vmin.getValue(), slider_Vmax.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_Vmax.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                label_Vmax.setText(slider_Vmax.getValue() + "");
                BufferedImage newImage = matToBufferedImage(matConvertTo(slider_Hmin.getValue(),
                                         slider_Hmax.getValue(), slider_Smin.getValue(), slider_Smax.getValue(),
                                         slider_Vmin.getValue(), slider_Vmax.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat matConvertTo(double Hmin, double Hmax, double Smin, double Smax,
                            double Vmin, double Vmax)
    {
        Mat source2 = Imgcodecs.imread("C://opencv3.1//samples//DSC_0869.jpg");
        Mat hsv_image = new Mat();
        Imgproc.GaussianBlur(source2, source2, new Size(3, 3), 0, 0);
        Imgproc.cvtColor(source2, hsv_image, Imgproc.COLOR_BGR2HSV);
        Mat thresholded = new Mat();
        Core.inRange(hsv_image, new Scalar(Hmin, Smin, Vmin), new Scalar(Hmax, Smax,
                     Vmax), thresholded);

        int rust = 0;

        //gray
        for (int j = 0; j < thresholded.rows(); j++) {
            for (int i = 0; i < thresholded.cols(); i++) {
                double[] temp = thresholded.get(j, i);

                //System.out.println(temp[0]);
                if (temp[0] == 255) {
                    rust++;
                }

            }
        }

        System.out.println("生鏽面積大約:" + (float)(rust * 100) /
                           (thresholded.rows()*thresholded.cols()) + "%");

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
