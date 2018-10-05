package ch09;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.photo.Photo;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch09_9_1DetailEnhance
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
                    Ch09_9_1DetailEnhance window = new Ch09_9_1DetailEnhance();
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
    public Ch09_9_1DetailEnhance()
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
        frmjavaSwing.setTitle("opencv detailEnhance ³B²z½m²ß");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showRedValue = new JLabel("1");
        showRedValue.setBounds(119, 32, 46, 15);
        frmjavaSwing.getContentPane().add(showRedValue);

        final JLabel showGreenValue = new JLabel("0.1");
        showGreenValue.setBounds(366, 32, 46, 15);
        frmjavaSwing.getContentPane().add(showGreenValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_sigmaR = new JSlider();
        slider_sigmaR.setValue(0);
        slider_sigmaR.setMaximum(10);
        slider_sigmaR.setBounds(342, 10, 75, 25);
        frmjavaSwing.getContentPane().add(slider_sigmaR);

        final JSlider slider_sigmaS = new JSlider();
        slider_sigmaS.setMinimum(1);
        slider_sigmaS.setMaximum(200);
        slider_sigmaS.setValue(1);

        slider_sigmaS.setBounds(79, 10, 134, 25);
        frmjavaSwing.getContentPane().add(slider_sigmaS);

        JLabel lblAlpha = new JLabel("sigma_s");
        lblAlpha.setBounds(10, 10, 56, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("sigma_r");
        lblBeta.setBounds(265, 10, 67, 15);
        frmjavaSwing.getContentPane().add(lblBeta);
        slider_sigmaS.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showRedValue.setText((float)slider_sigmaS.getValue() + "");
                BufferedImage newImage = matToBufferedImage(detailEnhance((
                                             float)slider_sigmaS.getValue(), (float)slider_sigmaR.getValue() / 10));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_sigmaR.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showGreenValue.setText((float)slider_sigmaR.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(detailEnhance((
                                             float)slider_sigmaS.getValue(), (float)slider_sigmaR.getValue() / 10));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat detailEnhance(float alpha, float beta)
    {
        Mat src = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg",
                                   Imgcodecs.CV_LOAD_IMAGE_COLOR);
        Mat destination = new Mat();

        Photo.detailEnhance(src, destination, alpha, beta);
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
