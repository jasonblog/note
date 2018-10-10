package ch10;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.imgproc.Imgproc;
import java.awt.EventQueue;
import java.awt.image.BufferedImage;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch10_20_2FitLine
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
                    Ch10_20_2FitLine window = new Ch10_20_2FitLine();
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
    public Ch10_20_2FitLine()
    {
        initialize();
        System.out.println("Imgproc.DIST_L1=" + Imgproc.DIST_L1);
        System.out.println("Imgproc.DIST_L2=" + Imgproc.DIST_L2);
        System.out.println("Imgproc.DIST_L12=" + Imgproc.DIST_L12);
        System.out.println("Imgproc.DIST_FAIR=" + Imgproc.DIST_FAIR);
        System.out.println("Imgproc.DIST_HUBER=" + Imgproc.DIST_HUBER);
        System.out.println("Imgproc.DIST_WELSCH=" + Imgproc.DIST_WELSCH);
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source =  new Mat(512, 512, CvType.CV_8UC3, new Scalar(255, 255,
                                    255));
        BufferedImage image = matToBufferedImage(source);


        final Mat normalDistubitionMat = new Mat(100, 2, CvType.CV_32FC1);

        Core.randu(normalDistubitionMat, 254, 150);
        System.out.println(normalDistubitionMat.dump());


        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("");
        frmjavaSwing.setBounds(100, 100, 687, 639);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showShapeValue = new JLabel("1");
        showShapeValue.setBounds(196, 10, 25, 15);
        frmjavaSwing.getContentPane().add(showShapeValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(15, 60, 558, 535);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_cth1 = new JSlider();
        slider_cth1.setValue(1);
        slider_cth1.setMinimum(1);
        slider_cth1.setMaximum(7);
        slider_cth1.setBounds(76, 10, 110, 25);
        frmjavaSwing.getContentPane().add(slider_cth1);

        JLabel lblBeta = new JLabel("distType");
        lblBeta.setBounds(15, 10, 110, 15);
        frmjavaSwing.getContentPane().add(lblBeta);

        slider_cth1.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                if (slider_cth1.getValue() == 3) {
                    slider_cth1.setValue(4);
                }

                showShapeValue.setText(slider_cth1.getValue() + "");
                BufferedImage newImage = matToBufferedImage(FindFitLine(normalDistubitionMat,
                                         slider_cth1.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }


    public Mat FindFitLine(Mat data, int  distType)
    {
        Mat source = new Mat(512, 512, CvType.CV_8UC3, new Scalar(255, 255, 255));
        Mat line = new Mat();


        //System.out.println(pointDataMat.dump());
        Point samplePt;

        for (int i = 0; i < data.rows(); i++) {

            samplePt = new Point(data.get(i, 0)[0], data.get(i, 1)[0]);
            Imgproc.circle(source, samplePt, 2, new Scalar(255, 0, 0));

        }

        Imgproc.fitLine(data, line, distType, 0, 0.01, 0.01);
        // System.out.println("dump:"+line.size());//1x4
        double v0 = line.get(0, 0)[0];
        double v1 = line.get(1, 0)[0];
        double v2 = line.get(2, 0)[0];
        double v3 = line.get(3, 0)[0];

        //System.out.println(v0 + "," + v1 + "," + v2 + "," + v3);

        try {
            if (v0 != 0) {
                int left = (int)((-v2 * v1 / v0) + v3);
                int right = (int)(((source.cols() - v2) * v1 / v0) + v3);

                Imgproc.line(source, new Point(source.cols() - 1, right),   new Point(0, left),
                             new Scalar(0, 0, 0), 2);
            }

        } catch (Exception e) {
        }

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
