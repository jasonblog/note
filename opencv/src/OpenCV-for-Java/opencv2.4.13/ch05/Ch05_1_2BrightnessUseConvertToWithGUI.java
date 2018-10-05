package ch05;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.highgui.Highgui;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;
import java.awt.Font;

public class Ch05_1_2BrightnessUseConvertToWithGUI
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
                    Ch05_1_2BrightnessUseConvertToWithGUI window = new
                    Ch05_1_2BrightnessUseConvertToWithGUI();
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
    public Ch05_1_2BrightnessUseConvertToWithGUI()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Highgui.imread("C://opencv3.1//samples//lena.jpg");
        //final Mat source = Highgui.imread("C://opencv3.1//samples//scene_l.bmp");

        BufferedImage image = matToBufferedImage(matConvertTo(source, alpha, beta));

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("讀取影像至Java Swing視窗");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showAlphaValue = new JLabel("2");
        showAlphaValue.setBounds(228, 10, 46, 15);
        frmjavaSwing.getContentPane().add(showAlphaValue);

        final JLabel showBetaValue = new JLabel("50");
        showBetaValue.setBounds(228, 50, 46, 15);
        frmjavaSwing.getContentPane().add(showBetaValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 75, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_beta = new JSlider();
        slider_beta.setMaximum(300);
        slider_beta.setBounds(47, 44, 180, 25);
        frmjavaSwing.getContentPane().add(slider_beta);

        final JSlider slider_alpha = new JSlider();
        slider_alpha.setMinimum(1);
        slider_alpha.setMaximum(20);
        slider_alpha.setValue(2);

        slider_alpha.setBounds(47, 10, 180, 25);
        frmjavaSwing.getContentPane().add(slider_alpha);

        JLabel lblAlpha = new JLabel("Alpha");
        lblAlpha.setBounds(10, 20, 46, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("Beta");
        lblBeta.setBounds(10, 45, 46, 15);
        frmjavaSwing.getContentPane().add(lblBeta);

        JLabel label = new JLabel("變亮");
        label.setFont(new Font("新細明體", Font.PLAIN, 10));
        label.setBounds(0, 0, 37, 15);
        frmjavaSwing.getContentPane().add(label);

        JLabel label_1 = new JLabel("變暗");
        label_1.setFont(new Font("新細明體", Font.PLAIN, 10));
        label_1.setBounds(274, 0, 37, 15);
        frmjavaSwing.getContentPane().add(label_1);

        JSlider slider_dark_alpha = new JSlider();
        slider_dark_alpha.setMinimum(1);
        slider_dark_alpha.setMaximum(9);
        slider_dark_alpha.setValue(5);
        slider_dark_alpha.setBounds(306, 10, 118, 25);
        frmjavaSwing.getContentPane().add(slider_dark_alpha);

        JLabel label_dark_alpha = new JLabel("0.5");
        label_dark_alpha.setBounds(434, 10, 46, 15);
        frmjavaSwing.getContentPane().add(label_dark_alpha);

        JSlider slider_dark_beta = new JSlider();

        slider_dark_beta.setMinimum(1);
        slider_dark_beta.setMaximum(300);
        slider_dark_beta.setBounds(306, 44, 118, 25);
        frmjavaSwing.getContentPane().add(slider_dark_beta);

        JLabel label_dark_beta = new JLabel("50");
        label_dark_beta.setBounds(434, 50, 46, 15);
        frmjavaSwing.getContentPane().add(label_dark_beta);



        slider_alpha.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showAlphaValue.setText(slider_alpha.getValue() + "");
                BufferedImage newImage = matToBufferedImage(matConvertTo(source,
                                         slider_alpha.getValue(), slider_beta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_beta.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showBetaValue.setText(slider_beta.getValue() + "");
                BufferedImage newImage = matToBufferedImage(matConvertTo(source,
                                         slider_alpha.getValue(), slider_beta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_dark_alpha.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                label_dark_alpha.setText((float)slider_dark_alpha.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(matConvertTo(source,
                                         (float)slider_dark_alpha.getValue() / 10, 0 - slider_dark_beta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });


        slider_dark_beta.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent e) {
                label_dark_beta .setText(slider_dark_beta.getValue() + "");
                BufferedImage newImage = matToBufferedImage(matConvertTo(source,
                                         (float)slider_dark_alpha.getValue() / 10, 0 - slider_dark_beta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });


    }
    public Mat matConvertTo(Mat source, double alpha, double beta)
    {

        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        source.convertTo(destination, -1, alpha, beta);

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
