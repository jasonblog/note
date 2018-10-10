package ch09;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.List;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.photo.Photo;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch09_5_1ColorChange
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
                    Ch09_5_1ColorChange window = new Ch09_5_1ColorChange();
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
    public Ch09_5_1ColorChange()
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
        frmjavaSwing.setTitle("opencv colorChange³B²z½m²ß");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showRedValue = new JLabel("1");
        showRedValue.setBounds(82, 43, 46, 15);
        frmjavaSwing.getContentPane().add(showRedValue);

        final JLabel showGreenValue = new JLabel("1");
        showGreenValue.setBounds(249, 43, 46, 15);
        frmjavaSwing.getContentPane().add(showGreenValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_Green = new JSlider();
        slider_Green.setMinimum(1);
        slider_Green.setValue(1);
        slider_Green.setMaximum(500);
        slider_Green.setBounds(220, 10, 75, 25);
        frmjavaSwing.getContentPane().add(slider_Green);

        final JSlider slider_Red = new JSlider();
        slider_Red.setMinimum(1);
        slider_Red.setMaximum(500);
        slider_Red.setValue(1);

        slider_Red.setBounds(57, 10, 86, 25);
        frmjavaSwing.getContentPane().add(slider_Red);

        JLabel lblAlpha = new JLabel("red_mul");
        lblAlpha.setBounds(10, 10, 56, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("green_mul");
        lblBeta.setBounds(155, 10, 67, 15);
        frmjavaSwing.getContentPane().add(lblBeta);

        JLabel lblNewLabel_1 = new JLabel("blue_mul");
        lblNewLabel_1.setBounds(315, 10, 85, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_1);

        final JSlider slider_Blue = new JSlider();

        slider_Blue.setValue(1);
        slider_Blue.setMinimum(1);
        slider_Blue.setMaximum(500);
        slider_Blue.setBounds(373, 8, 75, 25);
        frmjavaSwing.getContentPane().add(slider_Blue);

        final JLabel showBlueValue = new JLabel("1");
        showBlueValue.setBounds(416, 43, 46, 15);
        frmjavaSwing.getContentPane().add(showBlueValue);

        slider_Blue.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showBlueValue.setText(slider_Blue.getValue() + "");
                BufferedImage newImage = matToBufferedImage(colorChange(slider_Red.getValue(),
                                         slider_Green.getValue(), slider_Blue.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_Red.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showRedValue.setText(slider_Red.getValue() + "");
                BufferedImage newImage = matToBufferedImage(colorChange(slider_Red.getValue(),
                                         slider_Green.getValue(), slider_Blue.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_Green.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showGreenValue.setText(slider_Green.getValue() + "");
                BufferedImage newImage = matToBufferedImage(colorChange(slider_Red.getValue(),
                                         slider_Green.getValue(), slider_Blue.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat colorChange(int Red, int Green, int Blue)
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

        Photo.colorChange(src, src_mask, destination, Red, Green, Blue);
        // Photo.colorChange(src,src_mask, destination,300,100,100);
        Imgcodecs.imwrite("C://opencv3.1//samples//colorChange_1.jpg", destination);
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
