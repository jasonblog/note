package ch11;

import java.awt.EventQueue;
import java.awt.Graphics;
import java.awt.Panel;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.JLayeredPane;
import java.awt.Color;
import java.awt.image.BufferedImage;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;

import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch11_1_1ShiTomasi
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
                    Ch11_1_1ShiTomasi window = new Ch11_1_1ShiTomasi();
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
    public Ch11_1_1ShiTomasi()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Highgui.imread("C://opencv3.1//samples//lena.jpg",
                                          Highgui.CV_LOAD_IMAGE_COLOR);

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("Shi-Tomasi角點檢測方法");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showMCValue = new JLabel("1");
        showMCValue.setBounds(262, 10, 46, 15);
        frmjavaSwing.getContentPane().add(showMCValue);

        final JLabel showQLValue = new JLabel("0.01");
        showQLValue.setBounds(262, 43, 37, 15);
        frmjavaSwing.getContentPane().add(showQLValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_ql = new JSlider();
        slider_ql.setValue(1);
        slider_ql.setMinimum(1);
        slider_ql.setMaximum(10);
        slider_ql.setBounds(81, 35, 171, 25);
        frmjavaSwing.getContentPane().add(slider_ql);

        final JSlider slider_mc = new JSlider();
        slider_mc.setMinimum(1);
        slider_mc.setValue(2);

        slider_mc.setBounds(81, 10, 171, 25);
        frmjavaSwing.getContentPane().add(slider_mc);

        JLabel lblAlpha = new JLabel("maxCorners");
        lblAlpha.setBounds(10, 20, 77, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("qualityLevel");
        lblBeta.setBounds(10, 45, 77, 15);
        frmjavaSwing.getContentPane().add(lblBeta);

        JLabel lblMindistance = new JLabel("minDistance");
        lblMindistance.setBounds(298, 18, 77, 15);
        frmjavaSwing.getContentPane().add(lblMindistance);

        final JSlider slider_md = new JSlider();
        slider_md.setValue(1);
        slider_md.setMinimum(1);

        slider_md.setBounds(305, 35, 143, 25);
        frmjavaSwing.getContentPane().add(slider_md);

        final JLabel showMDValue = new JLabel("1");
        showMDValue.setBounds(456, 43, 46, 15);
        frmjavaSwing.getContentPane().add(showMDValue);


        slider_ql.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showQLValue.setText((float)slider_ql.getValue() / 100 + "");
                BufferedImage newImage = matToBufferedImage(ShiTomasi(slider_mc.getValue(),
                                         (float)slider_ql.getValue() / 100, slider_md.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_md.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showMDValue.setText(slider_md.getValue() + "");
                BufferedImage newImage = matToBufferedImage(ShiTomasi(slider_mc.getValue(),
                                         (float)slider_ql.getValue() / 100, slider_md.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));

            }
        });
        slider_mc.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showMCValue.setText(slider_mc.getValue() + "");
                BufferedImage newImage = matToBufferedImage(ShiTomasi(slider_mc.getValue(),
                                         (float)slider_ql.getValue() / 100, slider_md.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });



    }
    public Mat ShiTomasi(int maxCorners, double qualityLevel, double minDistance)
    {
        Mat source = Highgui.imread("C://opencv3.1//samples//lena.jpg",
                                    Highgui.CV_LOAD_IMAGE_COLOR);
        Mat destination = source.clone();
        Imgproc.cvtColor(destination, destination, Imgproc.COLOR_BGR2GRAY);
        MatOfPoint corners = new MatOfPoint();

        Imgproc.goodFeaturesToTrack(destination, corners, maxCorners, qualityLevel,
                                    minDistance);
        //System.out.println(corners.size().height);
        MatOfPoint2f Pts = new MatOfPoint2f();
        corners.convertTo(Pts, CvType.CV_32FC2);
        Point[] AllPoint = Pts.toArray();

        for (Point px : AllPoint) {
            Core.circle(source, px, 6, new Scalar(0, 0, 255));
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
