package ch05;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import org.opencv.core.Size;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;


public class Ch05_4_4BoxFilterBlur
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    private JFrame frmjavaSwing;

    int apertureLinearSize = 1;//follow API,must be positive and odd
    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {

        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch05_4_4BoxFilterBlur window = new Ch05_4_4BoxFilterBlur();
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
    public Ch05_4_4BoxFilterBlur()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg");

        BufferedImage image = matToBufferedImage(BlurFilter(source,
                              apertureLinearSize));

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("Box filter¼Ò½k½m²ß");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showKsizeValue = new JLabel("1");
        showKsizeValue.setBounds(372, 10, 46, 15);
        frmjavaSwing.getContentPane().add(showKsizeValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_X = new JSlider();
        slider_X.setMaximum(203);
        slider_X.setMinimum(1);
        slider_X.setValue(1);
        slider_X.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showKsizeValue.setText(slider_X.getValue() + "");
                BufferedImage newImage = matToBufferedImage(BlurFilter(source,
                                         slider_X.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_X.setBounds(164, 10, 200, 25);
        frmjavaSwing.getContentPane().add(slider_X);

        JLabel lblAlpha = new JLabel("Ksize");
        lblAlpha.setBounds(116, 10, 38, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

    }
    public Mat BlurFilter(Mat source, int KSize)
    {

        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Imgproc.boxFilter(source, destination, -1, new Size(KSize, KSize));
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
