package ch10;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.Arrays;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfFloat;
import org.opencv.core.MatOfInt;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.core.Core.MinMaxLocResult;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch10_12_1CalcBackProject
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
                    Ch10_12_1CalcBackProject window = new Ch10_12_1CalcBackProject();
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
    public Ch10_12_1CalcBackProject()
    {
        initialize();

    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//palm2.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv 反向投影練習");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showKsizeValue = new JLabel("1");
        showKsizeValue.setBounds(304, 20, 46, 15);
        frmjavaSwing.getContentPane().add(showKsizeValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(20, 53, 260, 197);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_method = new JSlider();
        slider_method.setMinimum(1);
        slider_method.setMaximum(50);
        slider_method.setValue(1);

        slider_method.setBounds(144, 10, 150, 25);
        frmjavaSwing.getContentPane().add(slider_method);

        JLabel lblAlpha = new JLabel("Histogram resolution");
        lblAlpha.setBounds(10, 20, 124, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        final JLabel lblNewLabel_2 = new JLabel("");
        lblNewLabel_2.setBounds(21, 275, 260, 197);
        frmjavaSwing.getContentPane().add(lblNewLabel_2);

        slider_method.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showKsizeValue.setText(slider_method.getValue() + "");

                BufferedImage newImage = matToBufferedImage(BackProject(
                                             slider_method.getValue()));
                lblNewLabel_2.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat BackProject(int bins)
    {
        Mat source2 = Imgcodecs.imread("C://opencv3.1//samples//palm2.jpg");
        Mat handle = new Mat();
        Imgproc.cvtColor(source2, handle, Imgproc.COLOR_RGB2HSV);

        //create single color image

        MatOfInt histSize = new MatOfInt(bins, bins);

        // hue varies from 0 to 179, see cvtColor
        // saturation varies from 0 (black-gray-white) to
        // 255 (pure spectrum color)
        MatOfFloat ranges = new MatOfFloat(0, 179, 0, 255);

        Mat hist = new Mat();

        // we compute the histogram from the 0-th and 1-st channels
        MatOfInt channels = new MatOfInt(0, 1);

        Imgproc.calcHist(Arrays.asList(handle), channels, new Mat(), hist, histSize,
                         ranges, false);
        Core.normalize(hist, hist, 0, 255, Core.NORM_MINMAX, -1, new Mat());

        Mat outputFrame = new Mat();

        Imgproc.calcBackProject(Arrays.asList(handle), channels, hist, outputFrame,
                                ranges, 1);

        return outputFrame;
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
