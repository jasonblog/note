package ch05;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch05_1_4BrightnessChgEveryPixel
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
                    Ch05_1_4BrightnessChgEveryPixel window = new Ch05_1_4BrightnessChgEveryPixel();
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
    public Ch05_1_4BrightnessChgEveryPixel()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg");

        BufferedImage image = matToBufferedImage(matConvertTo(source, alpha, beta));

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("讀取影像至Java Swing視窗");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showAlphaValue = new JLabel("2");
        showAlphaValue.setBounds(250, 10, 46, 15);
        frmjavaSwing.getContentPane().add(showAlphaValue);

        final JLabel showBetaValue = new JLabel("50");
        showBetaValue.setBounds(250, 35, 46, 15);
        frmjavaSwing.getContentPane().add(showBetaValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_beta = new JSlider();
        slider_beta.setMaximum(300);
        slider_beta.setBounds(47, 33, 200, 25);
        frmjavaSwing.getContentPane().add(slider_beta);

        final JSlider slider_alpha = new JSlider();
        slider_alpha.setValue(2);
        slider_alpha.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showAlphaValue.setText(slider_alpha.getValue() + "");
                BufferedImage newImage = matToBufferedImage(matConvertTo(source,
                                         slider_alpha.getValue(), slider_beta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_alpha.setBounds(47, 10, 200, 25);
        frmjavaSwing.getContentPane().add(slider_alpha);

        JLabel lblAlpha = new JLabel("Alpha");
        lblAlpha.setBounds(10, 20, 46, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("beta");
        lblBeta.setBounds(10, 45, 46, 15);
        frmjavaSwing.getContentPane().add(lblBeta);


        slider_beta.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showBetaValue.setText(slider_beta.getValue() + "");
                BufferedImage newImage = matToBufferedImage(matConvertTo(source,
                                         slider_alpha.getValue(), slider_beta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });





    }
    public Mat matConvertTo(Mat source, double alpha, double beta)
    {

        Mat destination = new Mat(source.rows(), source.cols(), source.type());

        for (int j = 0; j < source.rows(); j++) {
            for (int i = 0; i < source.cols(); i++) {
                double[] temp = source.get(j, i);
                temp[0] = temp[0] * alpha + beta;
                temp[1] = temp[1] * alpha + beta;
                temp[2] = temp[2] * alpha + beta;
                destination.put(j, i, temp);
            }
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
