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
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;
import org.opencv.imgproc.Moments;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.util.ArrayList;
import java.util.List;

public class Ch10_19_2Moments
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
                    Ch10_19_2Moments window = new Ch10_19_2Moments();
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
    public Ch10_19_2Moments()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        Mat source = Highgui.imread("C://opencv3.1//samples//moment-test1.png",
                                    Highgui.CV_LOAD_IMAGE_GRAYSCALE);
        BufferedImage image = matToBufferedImage(source);


        frmRr = new JFrame();
        frmRr.setTitle("空間矩、中心矩及標準中心矩計算");
        frmRr.setBounds(100, 100, 637, 529);
        frmRr.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmRr.getContentPane().setLayout(null);

        JLabel lblNewLabel = new JLabel("輪廓: ");
        lblNewLabel.setBounds(10, 10, 112, 15);
        frmRr.getContentPane().add(lblNewLabel);

        final JSlider slider_jpg = new JSlider();
        slider_jpg.setMaximum(10);

        slider_jpg.setValue(0);
        slider_jpg.setBounds(41, 10, 200, 25);
        frmRr.getContentPane().add(slider_jpg);

        final JLabel lblNewLabel_jpg = new JLabel("1");
        lblNewLabel_jpg.setBounds(317, 10, 46, 15);
        frmRr.getContentPane().add(lblNewLabel_jpg);

        final JLabel Label_img = new JLabel("");
        Label_img.setBounds(10, 277, 587, 130);
        frmRr.getContentPane().add(Label_img);

        JLabel Label_1 = new JLabel("");
        Label_1.setBounds(10, 70, 587, 160);
        frmRr.getContentPane().add(Label_1);
        Label_1.setIcon(new ImageIcon(image));

        JLabel lblNewLabel_1 = new JLabel("原圖:");
        lblNewLabel_1.setBounds(10, 45, 46, 15);
        frmRr.getContentPane().add(lblNewLabel_1);

        JLabel label = new JLabel("輪廓圖:");
        label.setBounds(20, 240, 46, 15);
        frmRr.getContentPane().add(label);
        slider_jpg.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                lblNewLabel_jpg.setText((slider_jpg.getValue() + 1) + "");
                System.out.println("選擇第:" + (slider_jpg.getValue() + 1) + "個輪廓");
                BufferedImage newImage = matToBufferedImage(findContours(
                                             slider_jpg.getValue()));
                Label_img.setIcon(new ImageIcon(newImage));

            }
        });

    }

    public Mat findContours(int i)
    {
        Mat source = Highgui.imread("C://opencv3.1//samples//moment-test1.png",
                                    Highgui.CV_LOAD_IMAGE_GRAYSCALE);
        Moments  moments = new  Moments();
        Mat drawing = Mat.zeros(source.size(), CvType.CV_8UC3);
        List<MatOfPoint> src_contours = new ArrayList<MatOfPoint>();
        Mat src_hierarchy = new Mat(source.rows(), source.cols(), CvType.CV_8UC1,
                                    new Scalar(0));
        Imgproc.findContours(source, src_contours, src_hierarchy, Imgproc.RETR_TREE,
                             Imgproc.CHAIN_APPROX_SIMPLE);

        System.out.println("共有:" + src_contours.size());

        Imgproc.drawContours(drawing, src_contours, i, new Scalar(255, 0, 0, 255));

        moments = Imgproc.moments(src_contours.get(i), false);
        System.out.println("moments.get_m00=" + moments.get_m00()); //0
        System.out.println("moments.get_m01=" + moments.get_m01());
        System.out.println("moments.get_m02=" + moments.get_m02());
        System.out.println("moments.get_m03=" + moments.get_m03());
        System.out.println("moments.get_m10=" + moments.get_m10());
        System.out.println("moments.get_m11=" + moments.get_m11());
        System.out.println("moments.get_m12=" + moments.get_m12());
        System.out.println("moments.get_m20=" + moments.get_m20());
        System.out.println("moments.get_m21=" + moments.get_m21());
        System.out.println("moments.get_m30=" + moments.get_m30());
        System.out.println("/////////////////////////////////");
        System.out.println("moments.get_get_mu02=" + moments.get_mu02());
        System.out.println("moments.get_get_mu03=" + moments.get_mu03());
        System.out.println("moments.get_get_mu11=" + moments.get_mu11());
        System.out.println("moments.get_get_mu12=" + moments.get_mu12());
        System.out.println("moments.get_get_mu20=" + moments.get_mu20());
        System.out.println("moments.get_get_mu21=" + moments.get_mu21());
        System.out.println("moments.get_get_mu30=" + moments.get_mu30());
        System.out.println("/////////////////////////////////");
        System.out.println("moments.get_get_nu03=" + moments.get_nu03());
        System.out.println("moments.get_get_nu03=" + moments.get_nu03());
        System.out.println("moments.get_get_nu11=" + moments.get_nu11());
        System.out.println("moments.get_get_nu12=" + moments.get_nu12());
        System.out.println("moments.get_get_nu20=" + moments.get_nu20());
        System.out.println("moments.get_get_nu21=" + moments.get_nu21());
        System.out.println("moments.get_get_nu30=" + moments.get_nu30());

        Mat Hu = new Mat();
        Imgproc.HuMoments(moments, Hu);
        //double result=Imgproc.matchShapes(source, target, 1,0);
        System.out.println("HuMoments= " + Hu.dump());//0
        System.out.println("驗證第1階Hu不變矩= " + (moments.get_nu20() +
                           moments.get_nu02()));//0


        //繪出質心
        Point center = new Point(moments.get_m10() / moments.get_m00(),
                                 moments.get_m01() / moments.get_m00());
        System.out.println("質心座標= (" + moments.get_m10() / moments.get_m00() + "," +
                           moments.get_m01() / moments.get_m00() + ")"); //0
        Core.circle(drawing, center, 2, new Scalar(0, 255, 0), -1);

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
