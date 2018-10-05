package ch05;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch05_5_1Threshold
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    private JFrame frmjavaSwing;
    double thresh = 0;
    double maxval = 0;
    int type = 0;

    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {

        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch05_5_1Threshold window = new Ch05_5_1Threshold();
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
    public Ch05_5_1Threshold()
    {
        initialize();
        System.out.println("THRESH_BINARY=" + Imgproc.THRESH_BINARY);
        System.out.println("THRESH_BINARY_INV=" + Imgproc.THRESH_BINARY_INV);
        System.out.println("THRESH_TRUNC=" + Imgproc.THRESH_TRUNC);
        System.out.println("THRESH_TOZERO=" + Imgproc.THRESH_TOZERO);
        System.out.println("THRESH_TOZERO_INV=" + Imgproc.THRESH_TOZERO_INV);
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Highgui.imread("C://opencv3.1//samples//lena.jpg");
        BufferedImage image = matToBufferedImage(threshold(source, thresh, maxval,
                              type));

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv Threshold½m²ß");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showAlphaValue = new JLabel("2");
        showAlphaValue.setBounds(260, 10, 29, 15);
        frmjavaSwing.getContentPane().add(showAlphaValue);

        final JLabel showBetaValue = new JLabel("50");
        showBetaValue.setBounds(259, 35, 29, 15);
        frmjavaSwing.getContentPane().add(showBetaValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_Maxval = new JSlider();
        slider_Maxval.setValue(0);
        slider_Maxval.setMaximum(600);
        slider_Maxval.setBounds(57, 35, 200, 25);
        frmjavaSwing.getContentPane().add(slider_Maxval);

        final JSlider slider_type = new JSlider();
        slider_type.setMaximum(4);
        slider_type.setValue(0);
        slider_type.setBounds(314, 10, 115, 25);
        frmjavaSwing.getContentPane().add(slider_type);


        final JSlider slider_Threshold = new JSlider();
        slider_Threshold.setMaximum(600);
        slider_Threshold.setValue(0);
        slider_Threshold.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showAlphaValue.setText(slider_Threshold.getValue() + "");
                BufferedImage newImage = matToBufferedImage(threshold(source,
                                         slider_Threshold.getValue(), slider_Maxval.getValue(), slider_type.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_Threshold.setBounds(57, 10, 200, 25);
        frmjavaSwing.getContentPane().add(slider_Threshold);

        JLabel lblThresh = new JLabel("Thresh");
        lblThresh.setBounds(10, 20, 46, 15);
        frmjavaSwing.getContentPane().add(lblThresh);

        JLabel lblMaxval = new JLabel("Maxval");
        lblMaxval.setBounds(10, 45, 46, 15);
        frmjavaSwing.getContentPane().add(lblMaxval);

        JLabel lblType = new JLabel("type");
        lblType.setBounds(290, 10, 46, 15);
        frmjavaSwing.getContentPane().add(lblType);

        final JLabel showTypeValue = new JLabel("0");
        showTypeValue.setBounds(435, 10, 19, 15);
        frmjavaSwing.getContentPane().add(showTypeValue);

        slider_type.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showTypeValue.setText(slider_type.getValue() + "");
                BufferedImage newImage = matToBufferedImage(threshold(source,
                                         slider_Threshold.getValue(), slider_Maxval.getValue(), slider_type.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));

            }
        });

        slider_Maxval.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showBetaValue.setText(slider_Maxval.getValue() + "");
                BufferedImage newImage = matToBufferedImage(threshold(source,
                                         slider_Threshold.getValue(), slider_Maxval.getValue(), slider_type.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat threshold(Mat source, double thresh, double maxval, int type)
    {

        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Imgproc.threshold(source, destination, thresh, maxval, type);
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
