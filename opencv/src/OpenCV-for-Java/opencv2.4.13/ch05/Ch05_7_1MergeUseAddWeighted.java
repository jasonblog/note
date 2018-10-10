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

public class Ch05_7_1MergeUseAddWeighted
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    private JFrame frmjavaSwing;
    double alpha = 0.0;
    double beta = 0.0;
    double gamma = 1;

    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {

        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch05_7_1MergeUseAddWeighted window = new Ch05_7_1MergeUseAddWeighted();
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
    public Ch05_7_1MergeUseAddWeighted()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source1 = Highgui.imread("C://opencv3.1//samples//lena.jpg");
        final Mat source2 = Highgui.imread("C://opencv3.1//samples//baboon.jpg");

        BufferedImage image = matToBufferedImage(merge(source1, source2, alpha, beta,
                              gamma));

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv Merge½m²ß");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showAlphaValue = new JLabel("0");
        showAlphaValue.setBounds(260, 10, 29, 15);
        frmjavaSwing.getContentPane().add(showAlphaValue);

        final JLabel showBetaValue = new JLabel("0");
        showBetaValue.setBounds(259, 35, 29, 15);
        frmjavaSwing.getContentPane().add(showBetaValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_Beta = new JSlider();
        slider_Beta.setValue(0);
        slider_Beta.setMaximum(20);
        slider_Beta.setBounds(57, 35, 200, 25);
        frmjavaSwing.getContentPane().add(slider_Beta);

        final JSlider slider_Gamma = new JSlider();
        slider_Gamma.setMaximum(200);
        slider_Gamma.setValue(0);
        slider_Gamma.setBounds(290, 35, 115, 25);
        frmjavaSwing.getContentPane().add(slider_Gamma);


        final JSlider slider_Alpha = new JSlider();
        slider_Alpha.setMaximum(20);
        slider_Alpha.setValue(0);
        slider_Alpha.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_Alpha.getValue()/10);
                showAlphaValue.setText((float)slider_Alpha.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(merge(source1, source2,
                                         (float)slider_Alpha.getValue() / 10, (float)slider_Beta.getValue() / 10,
                                         slider_Gamma.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_Alpha.setBounds(57, 10, 200, 25);
        frmjavaSwing.getContentPane().add(slider_Alpha);

        JLabel lblAlpha = new JLabel("Alpha");
        lblAlpha.setBounds(10, 20, 46, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBetaval = new JLabel("Beta");
        lblBetaval.setBounds(10, 45, 46, 15);
        frmjavaSwing.getContentPane().add(lblBetaval);

        JLabel lblGamma = new JLabel("Gamma");
        lblGamma.setBounds(290, 10, 46, 15);
        frmjavaSwing.getContentPane().add(lblGamma);

        final JLabel showTypeValue = new JLabel("0");
        showTypeValue.setBounds(415, 35, 19, 15);
        frmjavaSwing.getContentPane().add(showTypeValue);

        slider_Gamma.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showTypeValue.setText(slider_Gamma.getValue() + "");
                BufferedImage newImage = matToBufferedImage(merge(source1, source2,
                                         (float)slider_Alpha.getValue() / 10, (float)slider_Beta.getValue() / 10,
                                         slider_Gamma.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));

            }
        });

        slider_Beta.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showBetaValue.setText((float)slider_Beta.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(merge(source1, source2,
                                         (float)slider_Alpha.getValue() / 10, (float)slider_Beta.getValue() / 10,
                                         slider_Gamma.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat merge(Mat source1, Mat source2, double alpha, double beta,
                     double gamma)
    {

        Mat destination = new Mat(source1.rows(), source1.cols(), source1.type());
        Core.addWeighted(source1, alpha, source2, beta, gamma, destination);
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
