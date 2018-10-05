package ch10;

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
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.core.Core.MinMaxLocResult;
import org.opencv.imgproc.Imgproc;

import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch10_8_3FindTempleteFix
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
                    Ch10_8_3FindTempleteFix window = new Ch10_8_3FindTempleteFix();
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
    public Ch10_8_3FindTempleteFix()
    {
        initialize();
        System.out.println("Imgproc.TM_SQDIFF=" + Imgproc.TM_SQDIFF);
        System.out.println("Imgproc.TM_SQDIFF_NORMED=" + Imgproc.TM_SQDIFF_NORMED);
        System.out.println("Imgproc.TM_CCORR=" + Imgproc.TM_CCORR);
        System.out.println("Imgproc.TM_CCORR_NORMED=" + Imgproc.TM_CCORR_NORMED);
        System.out.println("Imgproc.TM_CCOEFF=" + Imgproc.TM_CCOEFF);
        System.out.println("Imgproc.TM_CCOEFF_NORMED =" + Imgproc.TM_CCOEFF_NORMED);

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
        frmjavaSwing.setTitle("opencv 找出某物練習");
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
        //比對結果
        if (isTheSame(subSrc, template)) {
            //若有抽檢像素值有符合,則畫出黑框
            Imgproc.rectangle(src, matchLoc, new Point(matchLoc.x + template.cols(),
                              matchLoc.y + template.rows()), new Scalar(0, 0, 0), 3);
        }

        return src;
    }

    //比對2個Mat的3個像素值是否類似
    public boolean isTheSame(Mat img1, Mat img2)
    {
        //System.out.println("img1-size="+img1.size());
        //System.out.println("img2-size="+img2.size());

        boolean isTheSame = false;
        //like抽檢
        double dataA0 = img1.get(0, 0)[0]; //point1's B
        double dataA1 = img1.get(0, 1)[0]; //point2's B
        double dataA2 = img1.get(0, 2)[0]; //point3's B
        //double dataA3=img1.get(0, 3)[0];
        double dataA4 = img1.get(0, 0)[1]; //point1's G
        double dataA5 = img1.get(0, 1)[1];
        double dataA6 = img1.get(0, 2)[1];
        double dataA7 = img1.get(0, 0)[2]; //point1's R
        double dataA8 = img1.get(0, 1)[2];
        double dataA9 = img1.get(0, 2)[2];

        //System.out.println("img1="+dataA0+","+dataA1+","+dataA2+","+dataA3+","+dataA4+","+dataA5+","+dataA6+","+dataA7+","+dataA8+","+dataA9);


        double dataB0 = img2.get(0, 0)[0];
        double dataB1 = img2.get(0, 1)[0];
        double dataB2 = img2.get(0, 2)[0];
        double dataB3 = img2.get(0, 3)[0];
        double dataB4 = img2.get(0, 0)[1];
        double dataB5 = img2.get(0, 1)[1];
        double dataB6 = img2.get(0, 2)[1];
        double dataB7 = img2.get(0, 0)[2];
        double dataB8 = img2.get(0, 1)[2];
        double dataB9 = img2.get(0, 2)[2];
        //System.out.println("img2="+dataB0+","+dataB1+","+dataB2+","+dataB3+","+dataB4+","+dataB5+","+dataB6+","+dataB7+","+dataB8+","+dataB9);

        if (Math.abs(dataA0 - dataB0) <= 5 && Math.abs(dataA1 - dataB1) <= 5 &&
            Math.abs(dataA2 - dataB2) <= 5 && Math.abs(dataA4 - dataB4) <= 5 &&
            Math.abs(dataA5 - dataB5) <= 5) {
            //if(Math.abs(dataA0-dataB0)<=5 && Math.abs(dataA1-dataB1)<=5 && Math.abs(dataA2-dataB2)<=5){
            //if(Math.abs(dataA0-dataB0)<=5 && Math.abs(dataA1-dataB1)<=5 && Math.abs(dataA2-dataB2)<=5&& Math.abs(dataA4-dataB4)<=5 &&Math.abs(dataA5-dataB5)<=5&&(dataA6==dataB6)&&(dataA7==dataB7)&&(dataA8==dataB8)&&(dataA9==dataB9)){
            isTheSame = true;
        }

        return isTheSame;
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
