package ch13;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfDouble;
import org.opencv.core.MatOfInt;
import org.opencv.core.MatOfRect;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import org.opencv.objdetect.CascadeClassifier;
import org.opencv.photo.Photo;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch13_1_9CheckParameterDetectMultiScale
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
                    Ch13_1_9CheckParameterDetectMultiScale window = new
                    Ch13_1_9CheckParameterDetectMultiScale();
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
    public Ch13_1_9CheckParameterDetectMultiScale()
    {
        initialize();

    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//testCar1.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv DetectMultiScale3參數調整練習");
        frmjavaSwing.setBounds(100, 100, 667, 637);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showRedValue = new JLabel("0");
        showRedValue.setBounds(119, 32, 46, 15);
        frmjavaSwing.getContentPane().add(showRedValue);

        final JLabel showGreenValue = new JLabel("1");
        showGreenValue.setBounds(366, 32, 46, 15);
        frmjavaSwing.getContentPane().add(showGreenValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 52, 620, 532);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_sigmaR = new JSlider();
        slider_sigmaR.setValue(0);
        slider_sigmaR.setMaximum(300);
        slider_sigmaR.setBounds(342, 10, 159, 25);
        frmjavaSwing.getContentPane().add(slider_sigmaR);

        final JSlider slider_sigmaS = new JSlider();
        slider_sigmaS.setMaximum(300);
        slider_sigmaS.setValue(1);

        slider_sigmaS.setBounds(79, 10, 134, 25);
        frmjavaSwing.getContentPane().add(slider_sigmaS);

        JLabel lblAlpha = new JLabel("minSize");
        lblAlpha.setBounds(10, 10, 56, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("maxSize");
        lblBeta.setBounds(265, 10, 67, 15);
        frmjavaSwing.getContentPane().add(lblBeta);
        slider_sigmaS.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showRedValue.setText((float)slider_sigmaS.getValue() + "");
                BufferedImage newImage = matToBufferedImage(CheckParameter((
                                             float)slider_sigmaS.getValue(), (float)slider_sigmaR.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_sigmaR.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showGreenValue.setText((float)slider_sigmaR.getValue() + "");
                BufferedImage newImage = matToBufferedImage(CheckParameter((
                                             float)slider_sigmaS.getValue(), (float)slider_sigmaR.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat CheckParameter(float minSz, float maxSz)
    {
        MatOfInt rejectLevels = new MatOfInt(5555); //沒做用
        MatOfDouble levelWeights = new MatOfDouble(666666666); //沒做用
        double scaleFactor = 1.1; //good=1.1
        int minNeighbors = 1; //0 error
        int flags = 0;
        Size minSize = new Size((int)minSz, (int)minSz);
        Size maxSize = new Size((int)maxSz, (int)maxSz);
        boolean outputRejectLevels = true; //false沒做用


        CascadeClassifier carsDetector = new
        CascadeClassifier("C://opencv3.1//samples//cars3.xml");
        MatOfRect carsDetections = new MatOfRect();

        Mat src = Imgcodecs.imread("C://opencv3.1//samples//testCar1.jpg",
                                   Imgcodecs.CV_LOAD_IMAGE_COLOR);

        carsDetector.detectMultiScale3(src, carsDetections, rejectLevels, levelWeights,
                                       scaleFactor, minNeighbors, flags, minSize, maxSize, outputRejectLevels);

        //carsDetector.detectMultiScale(src, carsDetections);
        for (Rect rect : carsDetections.toArray()) {
            Imgproc.rectangle(src, new Point(rect.x, rect.y), new Point(rect.x + rect.width,
                              rect.y + rect.height),
                              new Scalar(0, 255, 0), 3);
        }

        return src;

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
