package ch07;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfFloat;
import org.opencv.core.MatOfInt;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;
import javax.swing.JButton;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.util.ArrayList;
import java.util.List;

public class Ch07_10_1Histogram
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
                    Ch07_10_1Histogram window = new Ch07_10_1Histogram();
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
    public Ch07_10_1Histogram()
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
        frmjavaSwing.setTitle("opencv 直方圖統計練習");
        frmjavaSwing.setBounds(100, 100, 780, 620);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel_histogram = new JLabel("");
        lblNewLabel_histogram.setBounds(250, 10, 512, 551);
        frmjavaSwing.getContentPane().add(lblNewLabel_histogram);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(5, 60, image.getHeight() + 10, image.getWidth() + 10);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        JButton btn3 = new JButton("直方圖統計");
        btn3.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent arg0) {
                BufferedImage newImage = matToBufferedImage(getHistogram(source));
                lblNewLabel_histogram.setIcon(new ImageIcon(newImage));
            }
        });
        btn3.setBounds(10, 10, 114, 23);
        frmjavaSwing.getContentPane().add(btn3);


    }

    public Mat getHistogram(Mat source)
    {
        List<Mat> bgr_planes = new ArrayList<Mat>();
        List<Mat> bgr_plane = new ArrayList<Mat>();
        Core.split(source, bgr_planes);
        //bgr_plane.add(bgr_planes.get(0));
        MatOfInt histSize = new MatOfInt(256);
        MatOfFloat histRange = new MatOfFloat(0, 256);
        Mat hist = new  Mat();
        int hist_w = 512;
        int hist_h = 500;
        long bin_w;
        Mat histImage = new Mat(hist_h, hist_w, CvType.CV_8UC3);

        for (int j = 0; j < 3; j++) {


            bgr_plane.clear();
            bgr_plane.add(bgr_planes.get(j));

            Imgproc.calcHist(bgr_plane, new MatOfInt(0), new Mat(), hist, histSize,
                             histRange);
            //System.out.println("hist="+b_hist.dump()+"size="+b_hist.size());


            bin_w = Math.round((double)(hist_w / 256));

            Core.normalize(hist, hist, 0, histImage.rows(), Core.NORM_MINMAX);



            for (int i = 1; i < 256; i++) {

                if (j == 0) {
                    Core.line(histImage, new Point(bin_w * (i - 1),
                                                   hist_h - Math.round(hist.get(i - 1, 0)[0])),
                              new Point(bin_w * (i), hist_h - Math.round(Math.round(hist.get(i, 0)[0]))),
                              new  Scalar(255, 0, 0), 2, 8, 0);
                } else if (j == 1) {
                    Core.line(histImage, new Point(bin_w * (i - 1),
                                                   hist_h - Math.round(hist.get(i - 1, 0)[0])),
                              new Point(bin_w * (i), hist_h - Math.round(Math.round(hist.get(i, 0)[0]))),
                              new  Scalar(0, 255, 0), 2, 8, 0);
                } else if (j == 2) {
                    Core.line(histImage, new Point(bin_w * (i - 1),
                                                   hist_h - Math.round(hist.get(i - 1, 0)[0])),
                              new Point(bin_w * (i), hist_h - Math.round(Math.round(hist.get(i, 0)[0]))),
                              new  Scalar(0, 0, 255), 2, 8, 0);
                }


            }
        }

        return histImage;

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
