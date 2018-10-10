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

public class Ch09_6_1IlluminationChange
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
                    Ch09_6_1IlluminationChange window = new Ch09_6_1IlluminationChange();
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
    public Ch09_6_1IlluminationChange()
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
        frmjavaSwing.setTitle("opencv illuminationChange³B²z½m²ß");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showRedValue = new JLabel("1");
        showRedValue.setBounds(84, 32, 46, 15);
        frmjavaSwing.getContentPane().add(showRedValue);

        final JLabel showGreenValue = new JLabel("0.1");
        showGreenValue.setBounds(256, 32, 46, 15);
        frmjavaSwing.getContentPane().add(showGreenValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_beta = new JSlider();
        slider_beta.setValue(0);
        slider_beta.setMaximum(60);
        slider_beta.setBounds(220, 10, 75, 25);
        frmjavaSwing.getContentPane().add(slider_beta);

        final JSlider slider_alpha = new JSlider();
        slider_alpha.setMinimum(1);
        slider_alpha.setMaximum(600);
        slider_alpha.setValue(1);

        slider_alpha.setBounds(57, 10, 86, 25);
        frmjavaSwing.getContentPane().add(slider_alpha);

        JLabel lblAlpha = new JLabel("alpha");
        lblAlpha.setBounds(10, 10, 56, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("beta");
        lblBeta.setBounds(155, 10, 67, 15);
        frmjavaSwing.getContentPane().add(lblBeta);
        slider_alpha.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showRedValue.setText((float)slider_alpha.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(illuminationChange((
                                             float)slider_alpha.getValue() / 10, (float)slider_beta.getValue() / 10));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_beta.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showGreenValue.setText((float)slider_beta.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(illuminationChange((
                                             float)slider_alpha.getValue() / 10, (float)slider_beta.getValue() / 10));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat illuminationChange(float alpha, float beta)
    {
        Mat src = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg",
                                   Imgcodecs.CV_LOAD_IMAGE_COLOR);
        Mat destination = new Mat();
        Mat src_mask = new Mat(src.rows(), src.cols(), src.depth());

        for (int i = 0; i < src_mask.rows(); i++) {
            for (int j = 0; j < src_mask.cols(); j++) {
                double[] data = new double[3];
                data = src_mask.get(i, j);
                data[0] = 255;
                src_mask.put(i, j, data);
            }
        }

        Photo.illuminationChange(src, src_mask, destination, alpha, beta);
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
