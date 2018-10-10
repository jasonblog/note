package ch10;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfFloat;
import org.opencv.core.MatOfInt;
import org.opencv.core.MatOfPoint;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.core.Core.MinMaxLocResult;
import org.opencv.imgproc.Imgproc;

import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch10_19_4MatchTempleteAndCompareHist
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
                    Ch10_19_4MatchTempleteAndCompareHist window = new
                    Ch10_19_4MatchTempleteAndCompareHist();
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
    public Ch10_19_4MatchTempleteAndCompareHist()
    {
        initialize();

    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Imgcodecs.imread("C://opencv3.1//samples//DSC_0864.jpg");

        //final Mat template = Imgcodecs.imread("C://opencv3.1//samples//ladybug.jpg");

        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("opencv 找出某物練習,整合compareHist");
        frmjavaSwing.setBounds(100, 100, 684, 421);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showKsizeValue = new JLabel("0");
        showKsizeValue.setBounds(224, 10, 46, 15);
        frmjavaSwing.getContentPane().add(showKsizeValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 83, 550, 294);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_method = new JSlider();
        slider_method.setMaximum(5);
        slider_method.setValue(0);

        slider_method.setBounds(64, 10, 150, 25);
        frmjavaSwing.getContentPane().add(slider_method);

        JLabel lblAlpha = new JLabel("Method");
        lblAlpha.setBounds(10, 20, 46, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblNewLabel_1 = new JLabel("Template1");
        lblNewLabel_1.setBounds(10, 45, 64, 15);
        frmjavaSwing.getContentPane().add(lblNewLabel_1);

        JLabel lblNewLabel_2 = new JLabel("");
        lblNewLabel_2.setIcon(new
                              ImageIcon("C://opencv3.1//samples//goat-template1.jpg"));
        lblNewLabel_2.setBounds(74, 35, 46, 42);
        frmjavaSwing.getContentPane().add(lblNewLabel_2);

        JLabel lblTemplate = new JLabel("Template2");
        lblTemplate.setBounds(342, 45, 64, 15);
        frmjavaSwing.getContentPane().add(lblTemplate);

        JLabel lblNewLabel_ladybug = new JLabel("");
        lblNewLabel_ladybug.setIcon(new
                                    ImageIcon("C://opencv3.1//samples//ladybug.jpg"));
        lblNewLabel_ladybug.setBounds(411, 45, 54, 42);
        frmjavaSwing.getContentPane().add(lblNewLabel_ladybug);

        JLabel label = new JLabel("Method");
        label.setBounds(342, 10, 46, 15);
        frmjavaSwing.getContentPane().add(label);

        final JSlider slider_method2 = new JSlider();

        slider_method2.setValue(0);
        slider_method2.setMaximum(5);
        slider_method2.setBounds(388, 10, 150, 25);
        frmjavaSwing.getContentPane().add(slider_method2);

        final JLabel label_method2 = new JLabel("0");
        label_method2.setBounds(548, 10, 46, 15);
        frmjavaSwing.getContentPane().add(label_method2);

        slider_method2.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                label_method2.setText(slider_method2.getValue() + "");
                Mat src = Imgcodecs.imread("C://opencv3.1//samples//DSC_0864.jpg");
                Mat template = Imgcodecs.imread("C://opencv3.1//samples//ladybug.jpg");
                BufferedImage newImage = matToBufferedImage(findTemplete(src, template,
                                         slider_method2.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });

        slider_method.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showKsizeValue.setText(slider_method.getValue() + "");
                Mat src = Imgcodecs.imread("C://opencv3.1//samples//DSC_0864.jpg");
                Mat template = Imgcodecs.imread("C://opencv3.1//samples//goat-template1.jpg");
                BufferedImage newImage = matToBufferedImage(findTemplete(src, template,
                                         slider_method.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
    }
    public Mat findTemplete(Mat src, Mat template, int match_method)
    {

        int result_cols = src.cols() - template.cols() + 1;
        int result_rows = src.rows() - template.rows() + 1;
        //System.out.println("result_cols="+result_cols+",result_rows="+result_rows);
        Mat srcClone = src.clone();
        Mat result = new Mat(result_rows, result_cols, CvType.CV_32FC1);
        Imgproc.matchTemplate(src, template, result, match_method);
        Core.normalize(result, result, 0, 255, Core.NORM_MINMAX, -1, new Mat());
        MinMaxLocResult mmr = Core.minMaxLoc(result);
        Point matchLoc;

        if (match_method == Imgproc.TM_SQDIFF ||
            match_method == Imgproc.TM_SQDIFF_NORMED) {
            matchLoc = mmr.minLoc;
        } else {
            matchLoc = mmr.maxLoc;
        }

        //matchTemplate有找到用綠色框表示
        Imgproc.rectangle(src, matchLoc, new Point(matchLoc.x + template.cols(),
                          matchLoc.y + template.rows()), new Scalar(0, 255, 0));


        //System.out.println( matchLoc.y+" ,"+(matchLoc.y + template.rows())+","+matchLoc.x+","+(matchLoc.x + template.cols()));
        //擷取ROI區
        Mat subSrc = srcClone.submat((int) matchLoc.y,
                                     (int)(matchLoc.y + template.rows()), (int)matchLoc.x,
                                     (int)(matchLoc.x + template.cols()));
        //Imgcodecs.imwrite("C://opencv3.1//samples//test-result.jpg", subSrc);

        Mat img1Hist = getHistogram(template);
        Mat img2Hist = getHistogram(subSrc);
        double resultCompareHist = Imgproc.compareHist(img1Hist, img2Hist, 3);

        //比對結果
        if (resultCompareHist < 0.3) {
            //使用compareHist<0.3就有符合,則畫出黑框
            Imgproc.rectangle(src, matchLoc, new Point(matchLoc.x + template.cols(),
                              matchLoc.y + template.rows()), new Scalar(0, 0, 0), 3);
        }


        //System.out.println("result0="+result0);

        return src;
    }





    public  Mat getHistogram(Mat source)
    {

        Mat HSVsource = new Mat();
        //BGR轉換到HSV colorspace
        Imgproc.cvtColor(source, HSVsource, Imgproc.COLOR_BGR2HSV);

        Mat hist = new Mat();
        int Hbins = 30;
        int Sbins = 32;

        //對Hue channel使用30個bins,對Saturation chhannel使用32個bins
        MatOfInt mHistSize = new MatOfInt(Hbins, Sbins);

        //對Hue的取值Range(0,256),Saturation取值Range:(0,180)
        MatOfFloat mRanges = new MatOfFloat(0, 180, 0, 256);

        //使用chnnel0 及channel1
        MatOfInt mChannels = new MatOfInt(0, 1);

        List<Mat> Lhsv = Arrays.asList(HSVsource);

        //計算HSV色彩空間的直方圖
        Imgproc.calcHist(Lhsv, mChannels, new Mat(), hist, mHistSize, mRanges, false);
        Core.normalize(hist, hist, 0, 255, Core.NORM_MINMAX, -1, new Mat());

        return hist;
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
