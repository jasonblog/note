package ch10;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.List;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;
import java.awt.event.MouseMotionAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseAdapter;

public class Ch10_6_1PointPolygonTest
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    private JFrame frmjavaSwing;
    double dist;
    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {

        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch10_6_1PointPolygonTest window = new Ch10_6_1PointPolygonTest();
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
    public Ch10_6_1PointPolygonTest()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//shape-sample.jpg");
        String showData = "";


        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv 判斷某點是否在凸邊形的輪廓內");
        frmjavaSwing.setBounds(100, 100, 510, 358);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");


        lblNewLabel.setBounds(15, 151, 337, 163);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JLabel Label_Pt = new JLabel("");
        Label_Pt.setBounds(184, 10, 110, 26);
        frmjavaSwing.getContentPane().add(Label_Pt);

        final JLabel Label_click = new JLabel("info");
        Label_click.setBounds(20, 70, 391, 25);
        frmjavaSwing.getContentPane().add(Label_click);

        lblNewLabel.addMouseMotionListener(new MouseMotionAdapter() {
            @Override
            public void mouseMoved(MouseEvent arg0) {
                Label_Pt.setText("X:" + arg0.getX() + ",Y:" + arg0.getY());


            }
        });

        lblNewLabel.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent arg0) {
                BufferedImage newImage = matToBufferedImage(isInner(1, arg0.getX(),
                                         arg0.getY()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
                int simpleDist = (int) dist;

                if (simpleDist > 0) {
                    Label_click.setText("點擊於X:" + arg0.getX() + ",Y:" + arg0.getY() +
                                        "在區域內,距離:" + simpleDist);
                } else if (simpleDist < 0) {
                    Label_click.setText("點擊於X:" + arg0.getX() + ",Y:" + arg0.getY() +
                                        "在區域外,距離:" + (0 - simpleDist));
                } else if (simpleDist == 0) {
                    Label_click.setText("點擊於X:" + arg0.getX() + ",Y:" + arg0.getY() +
                                        "在輪廓上");
                }
            }
        });
    }


    public Mat isInner(double threshold1, int X, int Y)
    {
        Mat source = Imgcodecs.imread("C://opencv3.1//samples//shape-sample.jpg",
                                      Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
        //Mat source = Imgcodecs.imread("C://opencv249//sources//samples//c//airplane.jpg",Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Imgproc.GaussianBlur(source, source, new Size(3, 3), 10, 0);
        Imgproc.Canny(source, destination, threshold1, 100);
        List<MatOfPoint> contours = new ArrayList<MatOfPoint>();

        Mat hierarchy = new Mat(source.rows(), source.cols(), CvType.CV_8UC1,
                                new Scalar(0));
        Imgproc.findContours(destination, contours, hierarchy, Imgproc.RETR_TREE,
                             Imgproc.CHAIN_APPROX_SIMPLE);
        Mat drawing = Mat.zeros(destination.size(), CvType.CV_8UC3);

        dist = Imgproc.pointPolygonTest(new MatOfPoint2f(contours.get(0).toArray()),
                                        new Point(X, Y), true);
        Imgproc.drawContours(drawing, contours, 0, new Scalar(255, 0, 0, 255), 1);

        return drawing;
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
