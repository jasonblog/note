package ch10;

import java.awt.EventQueue;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

public class Ch10_13_3ColorPickerForImg
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
                    Ch10_13_3ColorPickerForImg window = new Ch10_13_3ColorPickerForImg();
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
    public Ch10_13_3ColorPickerForImg()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Highgui.imread("C://opencv3.1//samples//DSC_0773.jpg");
        //pointScaleData=source.get(source.rows(),source.cols()-1);
        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("讀取影像至Java Swing視窗");
        frmjavaSwing.setBounds(100, 100, 608, 496);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel lblNewLabel = new JLabel("");

        lblNewLabel.setBounds(10, 68, 564, 378);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        JLabel lbl_Rvalue = new JLabel("");
        lbl_Rvalue.setBounds(34, 18, 46, 15);
        frmjavaSwing.getContentPane().add(lbl_Rvalue);

        JLabel lbl_Hvalue = new JLabel("");
        lbl_Hvalue.setBounds(34, 43, 46, 15);
        frmjavaSwing.getContentPane().add(lbl_Hvalue);

        JLabel lblH = new JLabel("R:");
        lblH.setBounds(10, 20, 28, 15);
        frmjavaSwing.getContentPane().add(lblH);

        JLabel lblS = new JLabel("G:");
        lblS.setBounds(114, 18, 22, 15);
        frmjavaSwing.getContentPane().add(lblS);

        JLabel lbl_Gvalue = new JLabel(" ");
        lbl_Gvalue.setBounds(140, 18, 46, 15);
        frmjavaSwing.getContentPane().add(lbl_Gvalue);

        JLabel lbl_Svalue = new JLabel("");
        lbl_Svalue.setBounds(140, 43, 46, 15);
        frmjavaSwing.getContentPane().add(lbl_Svalue);

        JLabel lblV = new JLabel("B:");
        lblV.setBounds(228, 18, 22, 15);
        frmjavaSwing.getContentPane().add(lblV);

        JLabel lbl_Bvalue = new JLabel("");
        lbl_Bvalue.setBounds(260, 18, 46, 15);
        frmjavaSwing.getContentPane().add(lbl_Bvalue);

        JLabel lbl_Vvalue = new JLabel("");
        lbl_Vvalue.setBounds(260, 43, 46, 15);
        frmjavaSwing.getContentPane().add(lbl_Vvalue);

        JLabel lblH_1 = new JLabel("H:");
        lblH_1.setBounds(10, 43, 28, 15);
        frmjavaSwing.getContentPane().add(lblH_1);

        JLabel lblS_1 = new JLabel("S:");
        lblS_1.setBounds(114, 43, 22, 15);
        frmjavaSwing.getContentPane().add(lblS_1);

        JLabel label = new JLabel("V:");
        label.setBounds(228, 43, 22, 15);
        frmjavaSwing.getContentPane().add(label);


        lblNewLabel.addMouseListener(new MouseAdapter() {
            double[] pointScaleData = new double[3];
            Mat hsv_image = new Mat();
            @Override
            public void mouseClicked(MouseEvent e) {
                //System.out.println("X:"+e.getX()+",Y:"+e.getY());
                //RGV
                pointScaleData = source.get(e.getY() - 1, e.getX() - 1);
                lbl_Bvalue.setText(pointScaleData[0] + "");
                lbl_Gvalue.setText(pointScaleData[1] + "");
                lbl_Rvalue.setText(pointScaleData[2] + "");

                //HSV
                Imgproc.cvtColor(source, hsv_image, Imgproc.COLOR_BGR2HSV);
                pointScaleData = hsv_image.get(e.getY() - 1, e.getX() - 1);
                lbl_Hvalue.setText(pointScaleData[0] + "");
                lbl_Svalue.setText(pointScaleData[1] + "");
                lbl_Vvalue.setText(pointScaleData[2] + "");

            }
        });


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
