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
import org.opencv.core.RotatedRect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;
import javax.swing.JButton;
import javax.swing.SwingConstants;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

public class Ch10_18_1FindPalmCenter
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
                    Ch10_18_1FindPalmCenter window = new Ch10_18_1FindPalmCenter();
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
    public Ch10_18_1FindPalmCenter()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Highgui.imread("C://opencv3.1//samples//palm2.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv 尋找手掌心");
        frmjavaSwing.setBounds(100, 100, 687, 331);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(15, 60, 260, 230);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JLabel label = new JLabel("");
        label.setBounds(314, 60, 260, 230);
        frmjavaSwing.getContentPane().add(label);

        JButton btnNewButton = new JButton("Find");
        btnNewButton.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent arg0) {
                Mat[] getMat = FindAndDrawContours();
                BufferedImage newImage0 = matToBufferedImage(getMat[0]);
                BufferedImage newImage1 = matToBufferedImage(getMat[1]);
                lblNewLabel.setIcon(new ImageIcon(newImage0));
                label.setIcon(new ImageIcon(newImage1));

            }
        });
        btnNewButton.setVerticalAlignment(SwingConstants.BOTTOM);
        btnNewButton.setBounds(25, 10, 87, 23);
        frmjavaSwing.getContentPane().add(btnNewButton);
    }


    public Mat[] FindAndDrawContours()
    {
        Mat[] returnMat = new Mat[2];
        //手掌圖
        Mat source = Highgui.imread("C://opencv3.1//samples//palm2.jpg");
        Mat target = new Mat(source.size(), CvType.CV_8U);
        //轉灰階
        Imgproc.cvtColor(source, target, Imgproc.COLOR_RGB2GRAY);

        //除噪
        Imgproc.GaussianBlur(target, target, new Size(15, 15), 0, 0);
        Mat threshold_output = new Mat(source.rows(), source.cols(), source.type());
        Imgproc.threshold(target, threshold_output, 87, 255, Imgproc.THRESH_BINARY);


        Mat destination1 = new Mat();
        List<MatOfPoint> contours = new ArrayList<MatOfPoint>();

        //找輪廓
        Mat hierarchy = new Mat(source.rows(), source.cols(), CvType.CV_8UC1,
                                new Scalar(0));
        Imgproc.findContours(threshold_output, contours, hierarchy, Imgproc.RETR_TREE,
                             Imgproc.CHAIN_APPROX_SIMPLE);


        Imgproc.distanceTransform(threshold_output, destination1, Imgproc.CV_DIST_L2,
                                  3);
        int temp = 0;
        //半徑
        int R = 0;

        //圓心
        int centerX = 0;
        int centerY = 0;


        double d = 0;

        //遍歷,最適圓處理
        for (int i = 0; i < source.rows(); i++) {
            for (int j = 0; j < source.cols(); j++) {
                //檢查該點是否在輪廓內,計算點到輪廓邊界距離
                d = Imgproc.pointPolygonTest(new MatOfPoint2f(contours.get(0).toArray()),
                                             new Point(j, i), true);
                //System.out.println("d="+d);
                //if(d>0){
                temp = (int) destination1.get(i, j)[0];

                if (temp > R) {
                    R = temp;
                    centerX = j;
                    centerY = i;
                }

                //}
            }
        }

        System.out.println("centerX=" + centerX + ",centerY=" + centerY + ",R=" + R);


        //最適橢圓處理
        RotatedRect r = null;

        for (int i = 0; i < contours.size(); i++) {
            // Imgproc.drawContours(drawing, contours,i,new Scalar(255,0,0,255),2);

            if (contours.get(i).toArray().length > 115) {
                r = Imgproc.fitEllipse(new MatOfPoint2f(contours.get(i).toArray()));
                //Core.ellipse(drawing, r, new Scalar(0,0,255),3,8);
                //繪出最適橢圓
                Core.ellipse(source, r, new Scalar(0, 0, 255), 1, 8);
                Point ellipsePt = r.center;
                Core.line(source, ellipsePt, ellipsePt, new Scalar(0, 0, 255), 5);
            }

        }

        Core.circle(source, new Point(centerX, centerY), R, new Scalar(255, 255, 255));
        Core.line(source, new Point(centerX, centerY), new Point(centerX, centerY),
                  new Scalar(255, 255, 255), 5);
        Mat normalizeDst = new Mat();
        Core.normalize(destination1, normalizeDst, 0, 255, Core.NORM_MINMAX);
        Highgui.imwrite("C://opencv3.1//samples//distanceTransform_palm2.jpg",
                        normalizeDst);

        returnMat[0] = source;
        returnMat[1] = threshold_output;
        return returnMat;
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
