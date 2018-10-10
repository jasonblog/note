package ch10;

import java.awt.EventQueue;
import java.awt.image.BufferedImage;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;
import javax.swing.JButton;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfInt;
import org.opencv.core.MatOfPoint;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import org.opencv.imgproc.Moments;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.util.ArrayList;
import java.util.List;

public class Ch10_3_2ShowContours
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }

    private JFrame frmRr;

    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {
        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch10_3_2ShowContours window = new Ch10_3_2ShowContours();
                    window.frmRr.setVisible(true);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }

    /**
     * Create the application.
     */
    public Ch10_3_2ShowContours()
    {
        initialize();
        System.out.println("Imgproc.RETR_TREE=" + Imgproc.RETR_TREE);
        System.out.println("Imgproc.RETR_CCOMP=" + Imgproc.RETR_CCOMP);
        System.out.println("Imgproc.RETR_EXTERNAL=" + Imgproc.RETR_EXTERNAL);
        System.out.println("Imgproc.RETR_LIST=" + Imgproc.RETR_LIST);
        System.out.println("Imgproc.CHAIN_APPROX_SIMPLE=" +
                           Imgproc.CHAIN_APPROX_SIMPLE);
        System.out.println("Imgproc.CHAIN_APPROX_NONE=" + Imgproc.CHAIN_APPROX_NONE);
        System.out.println("Imgproc.CHAIN_APPROX_TC89_KCOS=" +
                           Imgproc.CHAIN_APPROX_TC89_KCOS);
        System.out.println("Imgproc.CHAIN_APPROX_TC89_L1=" +
                           Imgproc.CHAIN_APPROX_TC89_L1);
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        Mat source = Imgcodecs.imread("C://opencv3.1//samples//ContoursTest.png",
                                      Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
        BufferedImage image = matToBufferedImage(source);


        frmRr = new JFrame();
        frmRr.setTitle("輪廓順序表示法");
        frmRr.setBounds(100, 100, 637, 529);
        frmRr.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmRr.getContentPane().setLayout(null);

        JLabel lblNewLabel = new JLabel("輪廓: ");
        lblNewLabel.setBounds(10, 10, 112, 15);
        frmRr.getContentPane().add(lblNewLabel);

        final JSlider slider_jpg = new JSlider();
        slider_jpg.setMaximum(11);

        slider_jpg.setValue(0);
        slider_jpg.setBounds(41, 10, 168, 25);
        frmRr.getContentPane().add(slider_jpg);

        final JLabel lblNewLabel_jpg = new JLabel("0");
        lblNewLabel_jpg.setBounds(218, 10, 22, 15);
        frmRr.getContentPane().add(lblNewLabel_jpg);

        final JLabel Label_img = new JLabel("");
        Label_img.setBounds(10, 284, 587, 208);
        frmRr.getContentPane().add(Label_img);

        JLabel Label_1 = new JLabel("");
        Label_1.setBounds(16, 62, 581, 198);
        frmRr.getContentPane().add(Label_1);
        Label_1.setIcon(new ImageIcon(image));

        JLabel lblNewLabel_1 = new JLabel("原圖:");
        lblNewLabel_1.setBounds(20, 35, 46, 15);
        frmRr.getContentPane().add(lblNewLabel_1);

        JLabel label = new JLabel("輪廓圖:");
        label.setBounds(26, 270, 46, 15);
        frmRr.getContentPane().add(label);

        JLabel lblNewLabel_2 = new JLabel("Mode:");
        lblNewLabel_2.setBounds(247, 10, 40, 15);
        frmRr.getContentPane().add(lblNewLabel_2);

        final JSlider slider_mode = new JSlider();

        slider_mode.setValue(0);
        slider_mode.setMaximum(3);
        slider_mode.setBounds(281, 10, 79, 24);
        frmRr.getContentPane().add(slider_mode);

        final JLabel Label_mode = new JLabel("0");
        Label_mode.setBounds(370, 10, 28, 15);
        frmRr.getContentPane().add(Label_mode);

        JLabel lblNewLabel_3 = new JLabel("Method:");
        lblNewLabel_3.setBounds(394, 10, 46, 15);
        frmRr.getContentPane().add(lblNewLabel_3);

        final JSlider slider_method = new JSlider();

        slider_method.setValue(1);
        slider_method.setMinimum(1);
        slider_method.setMaximum(4);
        slider_method.setBounds(438, 10, 79, 24);
        frmRr.getContentPane().add(slider_method);

        final JLabel Label_method = new JLabel("1");
        Label_method.setBounds(527, 10, 46, 15);
        frmRr.getContentPane().add(Label_method);
        slider_jpg.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                lblNewLabel_jpg.setText(slider_jpg.getValue() + "");
                System.out.println("選擇第:" + slider_jpg.getValue() + "個輪廓");
                BufferedImage newImage = matToBufferedImage(findContours(slider_jpg.getValue(),
                                         slider_mode.getValue(), slider_method.getValue()));
                Label_img.setIcon(new ImageIcon(newImage));

            }
        });
        slider_mode.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                Label_mode.setText(slider_mode.getValue() + "");
                BufferedImage newImage = matToBufferedImage(findContours(slider_jpg.getValue(),
                                         slider_mode.getValue(), slider_method.getValue()));
                Label_img.setIcon(new ImageIcon(newImage));
            }
        });

        slider_method.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent e) {
                Label_method.setText(slider_method.getValue() + "");
                BufferedImage newImage = matToBufferedImage(findContours(slider_jpg.getValue(),
                                         slider_mode.getValue(), slider_method.getValue()));
                Label_img.setIcon(new ImageIcon(newImage));
            }
        });
    }

    public Mat findContours(int i, int mode, int method)
    {

        Mat source = Imgcodecs.imread("C://opencv3.1//samples//ContoursTest.png",
                                      Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
        Moments  moments = new  Moments();
        Mat drawing = Mat.zeros(source.size(), CvType.CV_8UC3);
        List<MatOfPoint> src_contours = new ArrayList<MatOfPoint>();
        Mat src_hierarchy = new Mat(source.rows(), source.cols(), CvType.CV_8UC1,
                                    new Scalar(0));

        try {
            Imgproc.findContours(source, src_contours, src_hierarchy, mode, method);
            System.out.println("共有:" + src_contours.size());

            Imgproc.drawContours(drawing, src_contours, i, new Scalar(255, 0, 0, 255));
        } catch (Exception e) {
            e.printStackTrace();
        }

        return drawing;
    }
    public BufferedImage matToBufferedImage(Mat matrix)
    {
        int cols = matrix.cols();
        int rows = matrix.rows();
        int elemSize = (int) matrix.elemSize();
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
