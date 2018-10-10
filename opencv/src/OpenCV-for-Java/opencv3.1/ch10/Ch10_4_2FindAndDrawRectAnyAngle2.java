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
import org.opencv.core.RotatedRect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch10_4_2FindAndDrawRectAnyAngle2
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
                    Ch10_4_2FindAndDrawRectAnyAngle2 window = new
                    Ch10_4_2FindAndDrawRectAnyAngle2();
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
    public Ch10_4_2FindAndDrawRectAnyAngle2()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//stuff.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv 尋找輪廓並且繪出外框矩形2");
        frmjavaSwing.setBounds(100, 100, 687, 639);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showShapeValue = new JLabel("1");
        showShapeValue.setBounds(135, 21, 25, 15);
        frmjavaSwing.getContentPane().add(showShapeValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(15, 60, 558, 535);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_cth1 = new JSlider();
        slider_cth1.setValue(1);
        slider_cth1.setMinimum(1);
        slider_cth1.setMaximum(300);
        slider_cth1.setBounds(15, 21, 110, 25);
        frmjavaSwing.getContentPane().add(slider_cth1);

        JLabel lblBeta = new JLabel("Canny_threshold1");
        lblBeta.setBounds(15, 10, 110, 15);
        frmjavaSwing.getContentPane().add(lblBeta);

        slider_cth1.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showShapeValue.setText(slider_cth1.getValue() + "");
                BufferedImage newImage = matToBufferedImage(FindAndDrawContours(
                                             slider_cth1.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }


    public Mat FindAndDrawContours(double threshold1)
    {
        Mat source = Imgcodecs.imread("C://opencv3.1//samples//stuff.jpg",
                                      Imgcodecs.CV_LOAD_IMAGE_GRAYSCALE);
        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Imgproc.GaussianBlur(source, source, new Size(3, 3), 10, 0);
        Imgproc.Canny(source, destination, threshold1, 100);
        List<MatOfPoint> contours = new ArrayList<MatOfPoint>();
        Mat hierarchy = new Mat(source.rows(), source.cols(), CvType.CV_8UC1,
                                new Scalar(0));
        Imgproc.findContours(destination, contours, hierarchy, Imgproc.RETR_TREE,
                             Imgproc.CHAIN_APPROX_SIMPLE);

        Mat drawing = Mat.zeros(destination.size(), CvType.CV_8UC3);
        Point[] allPoint = new Point[4];

        for (int i = 0; i < contours.size(); i++) {
            RotatedRect r = Imgproc.minAreaRect(new MatOfPoint2f(contours.get(
                                                    i).toArray()));
            Mat boxMat = new Mat();
            Imgproc.boxPoints(r, boxMat);
            // System.out.println(boxMat.dump());//發現有浮點數,故要宣告 MatOfPoint2f
            Imgproc.drawContours(drawing, contours, i, new Scalar(255, 0, 0, 255), 2);

            MatOfPoint2f allPointMatOfPoint = new MatOfPoint2f(boxMat);
            allPoint = allPointMatOfPoint.toArray();


            //矩形的4個點
            Imgproc.line(drawing, allPoint[0], allPoint[1], new Scalar(0, 255, 0), 2);
            Imgproc.line(drawing, allPoint[1], allPoint[2], new Scalar(0, 255, 0), 2);
            Imgproc.line(drawing, allPoint[2], allPoint[3], new Scalar(0, 255, 0), 2);
            Imgproc.line(drawing, allPoint[3], allPoint[0], new Scalar(0, 255, 0), 2);

            //          System.out.println(boxMat.dump());
            //           r.points(allPoint);
            //           for (int j = 0; j < 4; j++){
            //               Imgproc.line(drawing, allPoint[j], allPoint[(j+1)%4], new Scalar(0,255,0),2);
            //           }

        }

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
