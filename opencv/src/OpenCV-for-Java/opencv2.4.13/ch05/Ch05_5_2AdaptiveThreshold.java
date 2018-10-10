package ch05;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;

import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch05_5_2AdaptiveThreshold
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
                    Ch05_5_2AdaptiveThreshold window = new Ch05_5_2AdaptiveThreshold();
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
    public Ch05_5_2AdaptiveThreshold()
    {
        initialize();
        System.out.println("THRESH_BINARY=" + Imgproc.THRESH_BINARY);
        System.out.println("THRESH_BINARY_INV=" + Imgproc.THRESH_BINARY_INV);
        System.out.println("ADAPTIVE_THRESH_GAUSSIAN_C=" +
                           Imgproc.ADAPTIVE_THRESH_GAUSSIAN_C);
        System.out.println("ADAPTIVE_THRESH_MEAN_C=" + Imgproc.ADAPTIVE_THRESH_MEAN_C);
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Highgui.imread("C://opencv3.1//samples//lena-gray.jpg",
                                          CvType.CV_8UC1);

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv adaptiveThreshold½m²ß");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showAlphaValue = new JLabel("1");
        showAlphaValue.setBounds(260, 10, 40, 15);
        frmjavaSwing.getContentPane().add(showAlphaValue);

        final JLabel showBetaValue = new JLabel("1");
        showBetaValue.setBounds(256, 35, 29, 15);
        frmjavaSwing.getContentPane().add(showBetaValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_Maxval = new JSlider();
        slider_Maxval.setMinimum(1);
        slider_Maxval.setValue(1);
        slider_Maxval.setMaximum(300);
        slider_Maxval.setBounds(67, 33, 185, 25);
        frmjavaSwing.getContentPane().add(slider_Maxval);

        final JSlider slider_AdaptiveMethod = new JSlider();
        slider_AdaptiveMethod.setMaximum(1);
        slider_AdaptiveMethod.setValue(0);
        slider_AdaptiveMethod.setBounds(380, 10, 46, 25);
        frmjavaSwing.getContentPane().add(slider_AdaptiveMethod);


        final JSlider slider_blockSize = new JSlider();
        slider_blockSize.setMinimum(3);
        slider_blockSize.setMaximum(301);
        slider_blockSize.setValue(3);

        slider_blockSize.setBounds(67, 10, 192, 25);
        frmjavaSwing.getContentPane().add(slider_blockSize);

        JLabel lblThresh = new JLabel("blockSize");
        lblThresh.setBounds(10, 20, 58, 15);
        frmjavaSwing.getContentPane().add(lblThresh);

        JLabel lblMaxval = new JLabel("Maxval");
        lblMaxval.setBounds(10, 45, 46, 15);
        frmjavaSwing.getContentPane().add(lblMaxval);

        JLabel lblAdaptiveMethod = new JLabel("adaptiveMethod");
        lblAdaptiveMethod.setBounds(291, 10, 93, 15);
        frmjavaSwing.getContentPane().add(lblAdaptiveMethod);

        final JLabel showAdaptiveMethodValue = new JLabel("0");
        showAdaptiveMethodValue.setBounds(429, 10, 19, 15);
        frmjavaSwing.getContentPane().add(showAdaptiveMethodValue);

        JLabel lblNewLabel_1 = new JLabel("type");
        lblNewLabel_1.setBounds(306, 35, 46, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_1);

        final JSlider slider_Type = new JSlider();

        slider_Type.setValue(0);
        slider_Type.setMaximum(1);
        slider_Type.setBounds(343, 25, 46, 25);
        frmjavaSwing.getContentPane().add(slider_Type);

        final JLabel showTypeValue = new JLabel("0");
        showTypeValue.setBounds(402, 35, 46, 15);
        frmjavaSwing.getContentPane().add(showTypeValue);

        slider_blockSize.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                if (slider_blockSize.getValue() % 2 == 0) {
                    slider_blockSize.setValue(slider_blockSize.getValue() + 1);
                }

                showAlphaValue.setText(slider_blockSize.getValue() + "");
                BufferedImage newImage = matToBufferedImage(threshold(source,
                                         slider_blockSize.getValue(), slider_Maxval.getValue(),
                                         slider_AdaptiveMethod.getValue(), slider_Type.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_Type.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showTypeValue.setText(slider_Type.getValue() + "");
                BufferedImage newImage = matToBufferedImage(threshold(source,
                                         slider_blockSize.getValue(), slider_Maxval.getValue(),
                                         slider_AdaptiveMethod.getValue(), slider_Type.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));

            }
        });
        slider_AdaptiveMethod.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showAdaptiveMethodValue.setText(slider_AdaptiveMethod.getValue() + "");
                BufferedImage newImage = matToBufferedImage(threshold(source,
                                         slider_blockSize.getValue(), slider_Maxval.getValue(),
                                         slider_AdaptiveMethod.getValue(), slider_Type.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));

            }
        });

        slider_Maxval.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showBetaValue.setText(slider_Maxval.getValue() + "");
                BufferedImage newImage = matToBufferedImage(threshold(source,
                                         slider_blockSize.getValue(), slider_Maxval.getValue(),
                                         slider_AdaptiveMethod.getValue(), slider_Type.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat threshold(Mat source, int blockSize, double maxval,
                         int AdaptiveMethod, int type)
    {

        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Imgproc.adaptiveThreshold(source, destination, maxval, AdaptiveMethod, type,
                                  blockSize, 0);
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
