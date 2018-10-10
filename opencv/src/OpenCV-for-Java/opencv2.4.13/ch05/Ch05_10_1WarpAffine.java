package ch05;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Point;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch05_10_1WarpAffine
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    private JFrame frmjavaSwing;
    double alpha = 2;
    double beta = 50;
    /**
     * Launch the application.
     */
    public static void main(String[] args)
    {

        EventQueue.invokeLater(new Runnable() {
            public void run() {
                try {
                    Ch05_10_1WarpAffine window = new Ch05_10_1WarpAffine();
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
    public Ch05_10_1WarpAffine()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Highgui.imread("C://opencv3.1//samples//lena.jpg");
        //final Mat source = Highgui.imread("C://opencv3.1//samples//board.jpg");
        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv¼v¹³§á¦±¶É±×½m²ß");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showDot1Value = new JLabel("0.1");
        showDot1Value.setBounds(210, 10, 27, 15);
        frmjavaSwing.getContentPane().add(showDot1Value);

        final JLabel showDot2Value = new JLabel("0.1");
        showDot2Value.setBounds(210, 35, 27, 15);
        frmjavaSwing.getContentPane().add(showDot2Value);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 91, 438, 467);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_Dot2 = new JSlider();
        slider_Dot2.setMaximum(10);
        slider_Dot2.setValue(0);
        slider_Dot2.setBounds(47, 33, 162, 25);
        frmjavaSwing.getContentPane().add(slider_Dot2);

        final JSlider slider_Dot3 = new JSlider();
        slider_Dot3.setMaximum(10);
        slider_Dot3.setValue(0);

        slider_Dot3.setBounds(47, 56, 162, 25);
        frmjavaSwing.getContentPane().add(slider_Dot3);



        final JSlider slider_Dot1 = new JSlider();
        slider_Dot1.setMaximum(10);
        slider_Dot1.setValue(0);

        slider_Dot1.setBounds(47, 10, 162, 25);
        frmjavaSwing.getContentPane().add(slider_Dot1);

        JLabel lblAlpha = new JLabel("var1");
        lblAlpha.setBounds(10, 20, 46, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("var2");
        lblBeta.setBounds(10, 45, 46, 15);
        frmjavaSwing.getContentPane().add(lblBeta);

        JLabel lblDot = new JLabel("var3");
        lblDot.setBounds(10, 68, 27, 15);
        frmjavaSwing.getContentPane().add(lblDot);


        final JLabel showDot3Value = new JLabel("0.1");
        showDot3Value.setBounds(210, 60, 25, 15);
        frmjavaSwing.getContentPane().add(showDot3Value);

        JLabel lblVar_2 = new JLabel("var6");
        lblVar_2.setBounds(234, 66, 32, 15);
        frmjavaSwing.getContentPane().add(lblVar_2);

        JLabel lblVar_1 = new JLabel("var5");
        lblVar_1.setBounds(234, 43, 32, 15);
        frmjavaSwing.getContentPane().add(lblVar_1);

        JLabel lblVar = new JLabel("var4");
        lblVar.setBounds(234, 18, 32, 15);
        frmjavaSwing.getContentPane().add(lblVar);

        final JSlider slider_Dot4 = new JSlider();
        slider_Dot4.setMaximum(10);

        slider_Dot4.setValue(0);
        slider_Dot4.setBounds(265, 10, 162, 25);
        frmjavaSwing.getContentPane().add(slider_Dot4);

        final JSlider slider_Dot5 = new JSlider();
        slider_Dot5.setMaximum(10);

        slider_Dot5.setValue(0);
        slider_Dot5.setBounds(265, 35, 162, 25);
        frmjavaSwing.getContentPane().add(slider_Dot5);

        final JSlider slider_Dot6 = new JSlider();
        slider_Dot6.setMaximum(10);

        slider_Dot6.setValue(0);
        slider_Dot6.setBounds(265, 56, 162, 25);
        frmjavaSwing.getContentPane().add(slider_Dot6);

        final JLabel showDot4Value = new JLabel("0.1");
        showDot4Value.setBounds(437, 10, 27, 15);
        frmjavaSwing.getContentPane().add(showDot4Value);

        final JLabel showDot5Value = new JLabel("0.1");
        showDot5Value.setBounds(437, 35, 27, 15);
        frmjavaSwing.getContentPane().add(showDot5Value);

        final JLabel showDot6Value = new JLabel("0.1");
        showDot6Value.setBounds(437, 56, 27, 15);
        frmjavaSwing.getContentPane().add(showDot6Value);
        slider_Dot1.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showDot1Value.setText((float)slider_Dot1.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(wrap(source, slider_Dot1.getValue(),
                                         (float)slider_Dot2.getValue() / 10, (float)slider_Dot3.getValue() / 10,
                                         (float)slider_Dot4.getValue() / 10, (float)slider_Dot5.getValue() / 10,
                                         (float)slider_Dot6.getValue() / 10));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_Dot3.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showDot3Value.setText((float)slider_Dot3.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(wrap(source, slider_Dot1.getValue(),
                                         (float)slider_Dot2.getValue() / 10, (float)slider_Dot3.getValue() / 10,
                                         (float)slider_Dot4.getValue() / 10, (float)slider_Dot5.getValue() / 10,
                                         (float)slider_Dot6.getValue() / 10));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_Dot2.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showDot2Value.setText((float)slider_Dot2.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(wrap(source, slider_Dot1.getValue(),
                                         (float)slider_Dot2.getValue() / 10, (float)slider_Dot3.getValue() / 10,
                                         (float)slider_Dot4.getValue() / 10, (float)slider_Dot5.getValue() / 10,
                                         (float)slider_Dot6.getValue() / 10));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_Dot4.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showDot4Value.setText((float)slider_Dot4.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(wrap(source, slider_Dot1.getValue(),
                                         (float)slider_Dot2.getValue() / 10, (float)slider_Dot3.getValue() / 10,
                                         (float)slider_Dot4.getValue() / 10, (float)slider_Dot5.getValue() / 10,
                                         (float)slider_Dot6.getValue() / 10));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_Dot5.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showDot5Value.setText((float)slider_Dot5.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(wrap(source, slider_Dot1.getValue(),
                                         (float)slider_Dot2.getValue() / 10, (float)slider_Dot3.getValue() / 10,
                                         (float)slider_Dot4.getValue() / 10, (float)slider_Dot5.getValue() / 10,
                                         (float)slider_Dot6.getValue() / 10));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_Dot6.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showDot6Value.setText((float)slider_Dot6.getValue() / 10 + "");
                BufferedImage newImage = matToBufferedImage(wrap(source, slider_Dot1.getValue(),
                                         (float)slider_Dot2.getValue() / 10, (float)slider_Dot3.getValue() / 10,
                                         (float)slider_Dot4.getValue() / 10, (float)slider_Dot5.getValue() / 10,
                                         (float)slider_Dot6.getValue() / 10));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }

    public Mat wrap(Mat source, double dot1, double dot2, double dot3, double dot4,
                    double dot5, double dot6)
    {
        Mat wrapMat = new Mat(2, 3, CvType.CV_32FC1);
        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        MatOfPoint2f srcTri = new MatOfPoint2f();

        MatOfPoint2f dstTri = new MatOfPoint2f();

        Point[] srcPoints = new Point[3];
        srcPoints[0] = new Point(0, 0);
        srcPoints[1] = new Point(source.cols() - 1, 0);
        srcPoints[2] = new Point(0, source.rows() - 1);

        srcTri.fromArray(srcPoints);

        Point[] dstPoints = new Point[3];
        dstPoints[0] = new Point(source.cols()*dot1, source.rows()*dot2);
        dstPoints[1] = new Point(source.cols()*dot3, source.rows()*dot4);
        dstPoints[2] = new Point(source.cols()*dot5, source.rows()*dot6);

        dstTri.fromArray(dstPoints);

        wrapMat = Imgproc.getAffineTransform(srcTri, dstTri);
        //System.out.println(srcTri.size()+","+srcTri.dims()+","+srcTri.dump());
        //System.out.println("--------");
        //System.out.println(dstTri.size());
        Imgproc.warpAffine(source, destination, wrapMat, destination.size());

        return destination;

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
