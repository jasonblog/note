package ch05;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch05_9_3RotateUseGetRotationMatrix2D
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
                    Ch05_9_3RotateUseGetRotationMatrix2D window = new
                    Ch05_9_3RotateUseGetRotationMatrix2D();
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
    public Ch05_9_3RotateUseGetRotationMatrix2D()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Highgui.imread("C://opencv3.1//samples//lena.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv¨¤«×±ÛÂà½m²ß");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showAlphaValue = new JLabel("0");
        showAlphaValue.setBounds(250, 10, 46, 15);
        frmjavaSwing.getContentPane().add(showAlphaValue);

        final JLabel showBetaValue = new JLabel("0");
        showBetaValue.setBounds(250, 35, 46, 15);
        frmjavaSwing.getContentPane().add(showBetaValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_scale = new JSlider();
        slider_scale.setMinimum(1);
        slider_scale.setValue(10);
        slider_scale.setBounds(47, 33, 200, 25);
        frmjavaSwing.getContentPane().add(slider_scale);

        final JSlider slider_Angle = new JSlider();
        slider_Angle.setMaximum(359);
        slider_Angle.setValue(0);
        slider_Angle.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showAlphaValue.setText((float)slider_Angle.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(rotate(source,
                                         slider_Angle.getValue(), (float)slider_scale.getValue() / 10));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_Angle.setBounds(47, 10, 200, 25);
        frmjavaSwing.getContentPane().add(slider_Angle);

        JLabel lblAlpha = new JLabel("¨¤«×");
        lblAlpha.setBounds(10, 20, 46, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("scale");
        lblBeta.setBounds(10, 45, 46, 15);
        frmjavaSwing.getContentPane().add(lblBeta);


        slider_scale.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showBetaValue.setText((float)slider_scale.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(rotate(source,
                                         slider_Angle.getValue(), (float)slider_scale.getValue() / 10));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

    }
    public Mat rotate(Mat source, double angle, double scale)
    {
        Mat rotMat = new Mat(2, 3, CvType.CV_32FC1);
        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Point center = new Point(destination.cols() / 2, destination.rows() / 2);
        rotMat = Imgproc.getRotationMatrix2D(center, angle, scale);
        Imgproc.warpAffine(source, destination, rotMat, destination.size());

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
