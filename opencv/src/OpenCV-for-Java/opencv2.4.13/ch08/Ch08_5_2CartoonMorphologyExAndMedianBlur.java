package ch08;

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

public class Ch08_5_2CartoonMorphologyExAndMedianBlur
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
                    Ch08_5_2CartoonMorphologyExAndMedianBlur window = new
                    Ch08_5_2CartoonMorphologyExAndMedianBlur();
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
    public Ch08_5_2CartoonMorphologyExAndMedianBlur()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source1 = Highgui.imread("C://opencv3.1//samples//lena.jpg");
        final Mat source2 = source1.clone();

        BufferedImage image = matToBufferedImage(source1);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv 卡通化處理練習2");
        frmjavaSwing.setBounds(100, 100, 560, 686);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showAlphaValue = new JLabel("0");
        showAlphaValue.setBounds(241, 169, 29, 15);
        frmjavaSwing.getContentPane().add(showAlphaValue);

        final JLabel showBetaValue = new JLabel("1");
        showBetaValue.setBounds(290, 0, 29, 15);
        frmjavaSwing.getContentPane().add(showBetaValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(30, 204, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_Blur = new JSlider();
        slider_Blur.setValue(0);
        slider_Blur.setMaximum(201);
        slider_Blur.setBounds(89, 0, 200, 25);
        frmjavaSwing.getContentPane().add(slider_Blur);

        final JSlider slider_Gamma = new JSlider();
        slider_Gamma.setMaximum(500);
        slider_Gamma.setValue(0);
        slider_Gamma.setBounds(290, 169, 115, 25);
        frmjavaSwing.getContentPane().add(slider_Gamma);


        final JSlider slider_Alpha = new JSlider();
        slider_Alpha.setMaximum(200);
        slider_Alpha.setValue(0);
        slider_Alpha.setBounds(42, 169, 200, 25);
        frmjavaSwing.getContentPane().add(slider_Alpha);

        JLabel lblAlpha = new JLabel("透明度");
        lblAlpha.setBounds(55, 144, 46, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBetaval = new JLabel("模糊度");
        lblBetaval.setBounds(42, 7, 46, 15);
        frmjavaSwing.getContentPane().add(lblBetaval);

        JLabel lblGamma = new JLabel("明亮度");
        lblGamma.setBounds(323, 144, 46, 15);
        frmjavaSwing.getContentPane().add(lblGamma);

        final JLabel showTypeValue = new JLabel("0");
        showTypeValue.setBounds(408, 169, 36, 15);
        frmjavaSwing.getContentPane().add(showTypeValue);

        JLabel lblNewLabel_1 = new JLabel("Layer1");
        lblNewLabel_1.setBounds(0, 0, 46, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_1);

        JLabel lblGaussianKernelSize = new JLabel("Gaussian kernel size");
        lblGaussianKernelSize.setBounds(10, 45, 131, 15);
        frmjavaSwing.getContentPane().add(lblGaussianKernelSize);

        final JLabel label_ks_val = new JLabel("1");
        label_ks_val.setBounds(323, 42, 46, 15);
        frmjavaSwing.getContentPane().add(label_ks_val);

        final JSlider slider_ks = new JSlider();
        slider_ks.setValue(1);
        slider_ks.setMinimum(1);
        slider_ks.setMaximum(103);
        slider_ks.setBounds(139, 35, 174, 25);
        frmjavaSwing.getContentPane().add(slider_ks);



        JLabel lblLayer = new JLabel("Layer2");
        lblLayer.setBounds(0, 32, 46, 15);
        frmjavaSwing.getContentPane().add(lblLayer);

        JLabel lblMerge = new JLabel("Merge");
        lblMerge.setBounds(0, 131, 46, 15);
        frmjavaSwing.getContentPane().add(lblMerge);

        JLabel label_2 = new JLabel("threshold1");
        label_2.setBounds(10, 70, 64, 15);
        frmjavaSwing.getContentPane().add(label_2);

        final JLabel label_th1_val = new JLabel("0");
        label_th1_val.setBounds(239, 70, 31, 15);
        frmjavaSwing.getContentPane().add(label_th1_val);

        final JSlider slider_th1 = new JSlider();
        slider_th1.setValue(0);
        slider_th1.setMaximum(900);
        slider_th1.setBounds(78, 60, 153, 25);
        frmjavaSwing.getContentPane().add(slider_th1);



        JLabel label_4 = new JLabel("threshold2");
        label_4.setBounds(269, 70, 64, 15);
        frmjavaSwing.getContentPane().add(label_4);

        final JLabel label_th2_val = new JLabel("0");
        label_th2_val.setBounds(475, 70, 46, 15);
        frmjavaSwing.getContentPane().add(label_th2_val);

        final JSlider slider_th2 = new JSlider();
        slider_th2.setValue(0);
        slider_th2.setMaximum(900);
        slider_th2.setBounds(333, 67, 137, 25);
        frmjavaSwing.getContentPane().add(slider_th2);

        final JSlider slider_Ksize = new JSlider();

        slider_Ksize.setValue(0);
        slider_Ksize.setMaximum(50);
        slider_Ksize.setBounds(78, 95, 150, 25);
        frmjavaSwing.getContentPane().add(slider_Ksize);

        JLabel label = new JLabel("Ksize");
        label.setBounds(10, 95, 46, 15);
        frmjavaSwing.getContentPane().add(label);

        final JLabel label_Ksize_val = new JLabel("0");
        label_Ksize_val.setBounds(241, 95, 46, 15);
        frmjavaSwing.getContentPane().add(label_Ksize_val);

        final JLabel label_OP_val = new JLabel("0");
        label_OP_val.setBounds(380, 102, 25, 15);
        frmjavaSwing.getContentPane().add(label_OP_val);

        JLabel label_3 = new JLabel("op");
        label_3.setBounds(269, 95, 25, 15);
        frmjavaSwing.getContentPane().add(label_3);

        final JSlider slider_OP = new JSlider();

        slider_OP.setValue(2);
        slider_OP.setMinimum(2);
        slider_OP.setMaximum(6);
        slider_OP.setBounds(294, 95, 75, 25);
        frmjavaSwing.getContentPane().add(slider_OP);


        slider_ks.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                if (slider_ks.getValue() % 2 == 0) {
                    slider_ks.setValue(slider_ks.getValue() + 1);
                }

                label_ks_val.setText(slider_ks.getValue() + "");
                BufferedImage newImage = matToBufferedImage(BlurAndCanny(source2,
                                         slider_th1.getValue(), slider_th2.getValue(), slider_ks.getValue(),
                                         slider_Ksize.getValue(), slider_OP.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_th1.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {

                label_th1_val.setText(slider_th1.getValue() + "");
                BufferedImage newImage = matToBufferedImage(BlurAndCanny(source2,
                                         slider_th1.getValue(), slider_th2.getValue(), slider_ks.getValue(),
                                         slider_Ksize.getValue(), slider_OP.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));

            }
        });
        slider_th2.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                label_th2_val.setText(slider_th2.getValue() + "");
                BufferedImage newImage = matToBufferedImage(BlurAndCanny(source2,
                                         slider_th1.getValue(), slider_th2.getValue(), slider_ks.getValue(),
                                         slider_Ksize.getValue(), slider_OP.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_Gamma.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showTypeValue.setText(slider_Gamma.getValue() + "");
                Mat layer1 = Blur(source1, slider_Blur.getValue());
                Mat layer2 = BlurAndCanny(source2, slider_th1.getValue(), slider_th2.getValue(),
                                          slider_ks.getValue(), slider_Ksize.getValue(), slider_OP.getValue());
                BufferedImage newImage = matToBufferedImage(Merge(layer1, layer2,
                                         (float)slider_Alpha.getValue() / 10, 1, -slider_Gamma.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));

            }
        });
        slider_Alpha.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_Alpha.getValue()/10);
                showAlphaValue.setText((float)slider_Alpha.getValue() / 10 + "");

                if (slider_Blur.getValue() % 2 == 0) {
                    slider_Blur.setValue(slider_Blur.getValue() + 1);
                }

                Mat layer1 = Blur(source1, slider_Blur.getValue());
                Mat layer2 = BlurAndCanny(source2, slider_th1.getValue(), slider_th2.getValue(),
                                          slider_ks.getValue(), slider_Ksize.getValue(), slider_OP.getValue());
                BufferedImage newImage = matToBufferedImage(Merge(layer1, layer2,
                                         (float)slider_Alpha.getValue() / 10, 1, -slider_Gamma.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });


        slider_Blur.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                if (slider_Blur.getValue() % 2 == 0) {
                    slider_Blur.setValue(slider_Blur.getValue() + 1);
                }

                showBetaValue.setText(slider_Blur.getValue() + "");
                //BufferedImage newImage=matToBufferedImage(BlurAndMerge(source1,source2,(float)slider_Alpha.getValue()/10,1,-slider_Gamma.getValue(),slider_Blur.getValue()));
                BufferedImage newImage = matToBufferedImage(Blur(source1,
                                         slider_Blur.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_Ksize.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                label_Ksize_val.setText(slider_Ksize.getValue() + "");
                BufferedImage newImage = matToBufferedImage(BlurAndCanny(source2,
                                         slider_th1.getValue(), slider_th2.getValue(), slider_ks.getValue(),
                                         slider_Ksize.getValue(), slider_OP.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_OP.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                label_OP_val.setText(slider_OP.getValue() + "");
                BufferedImage newImage = matToBufferedImage(BlurAndCanny(source2,
                                         slider_th1.getValue(), slider_th2.getValue(), slider_ks.getValue(),
                                         slider_Ksize.getValue(), slider_OP.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat BlurAndCanny(Mat source, double threshold1, double threshold2,
                            int GaussianKernelSize, int ksize, int op)
    {
        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        Imgproc.GaussianBlur(source, destination, new Size(GaussianKernelSize,
                             GaussianKernelSize), 0, 0);
        Imgproc.Canny(destination, destination, threshold1, threshold2);
        Mat destination1 = new Mat(destination.rows(), destination.cols(),
                                   destination.type(), new Scalar(255, 255, 255));
        Mat destination2 = new Mat(destination.rows(), destination.cols(),
                                   destination.type());
        Mat destination3 = new Mat(destination.rows(), destination.cols(),
                                   destination.type());
        //Core.subtract(destination1, destination, destination2);
        // System.out.println("destination2="+destination2.type());
        // System.out.println("destination2="+destination2.channels());
        Imgproc.cvtColor(destination, destination2, Imgproc.COLOR_GRAY2RGB);

        Mat element = Imgproc.getStructuringElement(0, new Size(2 * ksize + 1,
                      2 * ksize + 1));
        Imgproc.morphologyEx(destination2, destination2, op, element);
        Imgproc.cvtColor(destination2, destination2, Imgproc.COLOR_RGB2GRAY);
        Core.subtract(destination1, destination2, destination2);
        Imgproc.cvtColor(destination2, destination2, Imgproc.COLOR_GRAY2RGB);
        return destination2;
    }

    public Mat Blur(Mat source1, int GaussianKernelSize)
    {
        Mat processBlur = new Mat(source1.rows(), source1.cols(), source1.type());
        Imgproc.medianBlur(source1, processBlur, GaussianKernelSize);
        // Imgproc.GaussianBlur(source1, processBlur,new Size(GaussianKernelSize,GaussianKernelSize),0,0);
        // System.out.println("processBlur="+processBlur.type());
        // System.out.println("processBlur="+processBlur.channels());
        return processBlur;

    }



    public Mat Merge(Mat source1, Mat source2, double alpha, double beta,
                     double gamma)
    {
        Mat destination = new Mat(source1.rows(), source1.cols(), source1.type());
        Core.addWeighted(source1, alpha, source2, beta, gamma, destination);
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
