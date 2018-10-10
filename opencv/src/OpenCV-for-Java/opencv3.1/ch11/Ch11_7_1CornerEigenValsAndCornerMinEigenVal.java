package ch11;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Core.MinMaxLocResult;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch11_7_1CornerEigenValsAndCornerMinEigenVal
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
                    Ch11_7_1CornerEigenValsAndCornerMinEigenVal window = new
                    Ch11_7_1CornerEigenValsAndCornerMinEigenVal();
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
    public Ch11_7_1CornerEigenValsAndCornerMinEigenVal()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//DSC_0734.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("客制化角點檢測視窗");
        frmjavaSwing.setBounds(100, 100, 515, 511);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showAlphaValue = new JLabel("1");
        showAlphaValue.setBounds(194, 35, 46, 15);
        frmjavaSwing.getContentPane().add(showAlphaValue);

        final JLabel showBetaValue = new JLabel("24");
        showBetaValue.setBounds(454, 35, 46, 15);
        frmjavaSwing.getContentPane().add(showBetaValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 80, 477, 387);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_myHarris = new JSlider();
        slider_myHarris.setMinimum(24);
        slider_myHarris.setValue(24);
        slider_myHarris.setBounds(244, 35, 200, 25);
        frmjavaSwing.getContentPane().add(slider_myHarris);

        final JSlider slider_myShi = new JSlider();
        slider_myShi.setMinimum(1);
        slider_myShi.setValue(1);
        slider_myShi.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showAlphaValue.setText(slider_myShi.getValue() + "");
                BufferedImage newImage = matToBufferedImage(myShiTomasi(
                                             slider_myShi.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_myShi.setBounds(24, 35, 171, 25);
        frmjavaSwing.getContentPane().add(slider_myShi);

        JLabel lblAlpha = new JLabel("myShiTomasiQualityLevel");
        lblAlpha.setBounds(24, 10, 171, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("myHarrisQualityLevel");
        lblBeta.setBounds(256, 10, 124, 15);
        frmjavaSwing.getContentPane().add(lblBeta);


        slider_myHarris.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showBetaValue.setText(slider_myHarris.getValue() + "");
                BufferedImage newImage = matToBufferedImage(myHarris(
                                             slider_myHarris.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

    }

    public Mat myHarris(double myHarrisQualityLevel)
    {
        Mat source = Imgcodecs.imread("C://opencv3.1//samples//DSC_0734.jpg");
        // 設置參數
        int blockSize = 3;
        int apertureSize = 3;

        Mat sourceGray = new Mat();

        // 使用cornerEigenValsAndVecs()函數檢測角點
        Mat dstHarris = Mat.zeros(source.size(), CvType.CV_32FC(6));
        Mat resHarris = Mat.zeros(source.size(), CvType.CV_32FC1);

        Imgproc.cvtColor(source, sourceGray, Imgproc.COLOR_BGR2GRAY);
        Imgproc.cornerEigenValsAndVecs(sourceGray, dstHarris, blockSize, apertureSize);

        // 特徵點選擇
        for (int j = 0; j < source.rows(); j++) {
            for (int i = 0; i < source.cols(); i++) {
                // 兩個特徵值
                double lambda1 = dstHarris.get(j, i)[0];
                double lambda2 = dstHarris.get(j, i)[1];
                resHarris.put(j, i, new double[] {lambda1 * lambda2 - 0.04 * Math.pow((lambda1 + lambda2), 2) });
            }
        }

        // 各類閾值
        double HarrisMinVal = 0;
        double HarrisMaxVal = 0;
        int maxQualityLevel = 100;
        MinMaxLocResult mmr = Core.minMaxLoc(resHarris);

        HarrisMaxVal = mmr.maxVal;
        HarrisMinVal = mmr.minVal;

        //角點檢測函數實現

        //深度拷貝原圖像用於繪製角點
        // Mat srcCopy = source1.clone();

        // 角點滿足條件則繪製
        for (int j = 0; j < source.rows(); j++)  {
            for (int i = 0; i < source.cols(); i++)   {
                if (resHarris.get(j, i)[0] > HarrisMinVal + (HarrisMaxVal - HarrisMinVal)
                    *myHarrisQualityLevel / maxQualityLevel) {
                    Imgproc.circle(source, new Point(i, j), 4, new Scalar(0, 0, 255), 1, 8, 0);
                }
            }
        }

        return source;

    }

    public Mat myShiTomasi(double myShiTomasiQualityLevel)
    {
        Mat source = Imgcodecs.imread("C://opencv3.1//samples//DSC_0734.jpg");
        // 設置參數
        int blockSize = 3;
        int apertureSize = 3;

        int maxQualityLevel = 100;

        Mat sourceGray = new Mat();
        Imgproc.cvtColor(source, sourceGray, Imgproc.COLOR_BGR2GRAY);
        Mat dstShiTomasi = Mat.zeros(source.size(), CvType.CV_32FC1);
        Imgproc.cornerMinEigenVal(sourceGray, dstShiTomasi, blockSize, apertureSize);
        MinMaxLocResult mmr = Core.minMaxLoc(dstShiTomasi);

        // 角點滿足條件則繪製
        for (int j = 0; j < source.rows(); j++)  {
            for (int i = 0; i < source.cols(); i++)   {
                if (dstShiTomasi.get(j, i)[0] > mmr.minVal + (mmr.maxVal - mmr.minVal)
                    *myShiTomasiQualityLevel / maxQualityLevel) {
                    Imgproc.circle(source, new Point(i, j), 4, new Scalar(0, 255, 0), 1, 8, 0);
                }
            }
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
