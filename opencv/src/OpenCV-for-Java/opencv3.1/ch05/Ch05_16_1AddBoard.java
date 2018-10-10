package ch05;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch05_16_1AddBoard
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
                    Ch05_16_1AddBoard window = new Ch05_16_1AddBoard();
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
    public Ch05_16_1AddBoard()
    {
        initialize();
        System.out.println("Core.BORDER_CONSTANT=" + Core.BORDER_CONSTANT);
        System.out.println("Core.BORDER_REPLICATE=" + Core.BORDER_REPLICATE);
        System.out.println("Core.BORDER_TRANSPARENT=" + Core.BORDER_TRANSPARENT);
        System.out.println("Core.BORDER_WRAP=" + Core.BORDER_WRAP);
        System.out.println("Core.BORDER_DEFAULT=" + Core.BORDER_DEFAULT);
        System.out.println("Core.BORDER_ISOLATED=" + Core.BORDER_ISOLATED);
        System.out.println("Core.BORDER_REFLECT=" + Core.BORDER_REFLECT);
        System.out.println("Core.BORDER_REFLECT101=" + Core.BORDER_REFLECT101);
        System.out.println("Core.BORDER_REFLECT_101=" + Core.BORDER_REFLECT_101);
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("讀取影像至Java Swing視窗");
        frmjavaSwing.setBounds(100, 100, 613, 690);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showAlphaValue = new JLabel("0");
        showAlphaValue.setBounds(306, 10, 46, 15);
        frmjavaSwing.getContentPane().add(showAlphaValue);

        final JLabel showBetaValue = new JLabel("0");
        showBetaValue.setBounds(306, 43, 46, 15);
        frmjavaSwing.getContentPane().add(showBetaValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 585, 578);
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_beta = new JSlider();
        slider_beta.setValue(0);
        slider_beta.setMaximum(4);
        slider_beta.setBounds(106, 36, 200, 25);
        frmjavaSwing.getContentPane().add(slider_beta);

        final JSlider slider_alpha = new JSlider();
        slider_alpha.setMaximum(5);
        slider_alpha.setValue(0);
        slider_alpha.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showAlphaValue.setText((float)slider_alpha.getValue() / 100 + "");
                BufferedImage newImage = matToBufferedImage(AddBoard((float)
                                         slider_alpha.getValue() / 100, slider_beta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_alpha.setBounds(96, 10, 200, 25);
        frmjavaSwing.getContentPane().add(slider_alpha);

        JLabel lblAlpha = new JLabel("邊框大小");
        lblAlpha.setBounds(10, 20, 76, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("邊框型式");
        lblBeta.setBounds(10, 45, 62, 15);
        frmjavaSwing.getContentPane().add(lblBeta);


        slider_beta.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showBetaValue.setText((float)slider_beta.getValue() + "");
                BufferedImage newImage = matToBufferedImage(AddBoard((float)
                                         slider_alpha.getValue() / 100, slider_beta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

    }
    public Mat AddBoard(float borderSize, int borderType)
    {
        Mat source = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg");
        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        float top;
        float bottom;
        float left;
        float right;
        top = ((borderSize * source.rows()));
        bottom = (borderSize * source.rows());
        left = (borderSize * source.cols());
        right = (borderSize * source.cols());
        destination = source;
        Core.copyMakeBorder(source, destination, (int)top, (int)bottom, (int)left,
                            (int)right, borderType);
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
