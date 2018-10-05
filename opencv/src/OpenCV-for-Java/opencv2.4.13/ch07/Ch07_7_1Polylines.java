package ch07;

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
import org.opencv.core.Point;
import org.opencv.core.Scalar;

public class Ch07_7_1Polylines
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
                    Ch07_7_1Polylines window = new Ch07_7_1Polylines();
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
    public Ch07_7_1Polylines()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        Mat source =  paint();
        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv 畫多邊形練習(中空)");
        frmjavaSwing.setBounds(100, 100, 300, 300);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(5, 5, image.getHeight() + 10, image.getWidth() + 10);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);
    }
    public Mat paint()
    {

        Mat source = new Mat(250, 250, CvType.CV_8UC3, new Scalar(255, 255, 255));
        List<MatOfPoint> allMatOfPoint = new ArrayList<MatOfPoint>();
        MatOfPoint mop1 = new MatOfPoint();
        List<Point> allPoint1 = new ArrayList<Point>();
        allPoint1.add(new Point(38, 21));
        allPoint1.add(new Point(13, 128));
        allPoint1.add(new Point(13, 139));
        allPoint1.add(new Point(155, 240));
        allPoint1.add(new Point(148, 167));
        allPoint1.add(new Point(203, 209));
        allPoint1.add(new Point(233, 133));
        allPoint1.add(new Point(154, 26));
        mop1.fromList(allPoint1);

        MatOfPoint mop2 = new MatOfPoint();
        List<Point> allPoint2 = new ArrayList<Point>();
        allPoint2.add(new Point(125, 0));
        allPoint2.add(new Point(0, 128));
        allPoint2.add(new Point(130, 139));

        mop2.fromList(allPoint2);
        allMatOfPoint.add(mop1);
        allMatOfPoint.add(mop2);
        Core.polylines(source, allMatOfPoint, true, new Scalar(0, 255, 0), 3);
        return source;
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
