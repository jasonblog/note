package ch10;

import java.awt.AWTException;
import java.awt.Dimension;
import java.awt.Rectangle;
import java.awt.Robot;
import java.awt.Toolkit;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferByte;
import java.awt.image.DataBufferInt;
import java.io.File;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;
import javax.imageio.ImageIO;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Core.MinMaxLocResult;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

public class Ch10_25_2FindApLocationInDesktop
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }

    public static void main(String[] args) throws AWTException {

        Mat template = Imgcodecs.imread("C://opencv3.1//samples//wordTitle.png");
        Robot robot = new Robot();
        robot.delay(5000);
        Dimension d = Toolkit.getDefaultToolkit().getScreenSize();
        Rectangle rect = new Rectangle(0, 0, d.width, d.height);
        BufferedImage image = robot.createScreenCapture(rect);

        Mat screen = bufferedImg2Mat(image);

        Mat findResult = findTemplete(screen, template, 0);

        Imgcodecs.imwrite("C://opencv3.1//samples//whereIsTheWord.png", findResult);
    }

    public static Mat bufferedImg2Mat(BufferedImage in)
    {
        Mat out;
        byte[] data;
        int r, g, b;

        if (in.getType() == BufferedImage.TYPE_INT_RGB) {
            out = new Mat(in.getHeight(), in.getWidth(), CvType.CV_8UC3);
            data = new byte[in.getHeight()*in.getWidth() * (int) out.elemSize()];
            int[] dataBuff = in.getRGB(0, 0, in.getWidth(), in.getHeight(), null, 0,
                                       in.getWidth());

            for (int i = 0; i < dataBuff.length; i++) {
                data[i * 3] = (byte)((dataBuff[i] >> 0) & 0xFF);
                data[i * 3 + 1] = (byte)((dataBuff[i] >> 8) & 0xFF);
                data[i * 3 + 2] = (byte)((dataBuff[i] >> 16) & 0xFF);
            }
        } else {
            out = new Mat(in.getHeight(), in.getWidth(), CvType.CV_8UC1);
            data = new byte[in.getHeight()*in.getWidth() * (int) out.elemSize()];
            int[] dataBuff = in.getRGB(0, 0, in.getWidth(), in.getHeight(), null, 0,
                                       in.getWidth());

            for (int i = 0; i < dataBuff.length; i++) {
                r = (byte)((dataBuff[i] >> 16) & 0xFF);
                g = (byte)((dataBuff[i] >> 8) & 0xFF);
                b = (byte)((dataBuff[i] >> 0) & 0xFF);
                data[i] = (byte)((0.21 * r) + (0.71 * g) + (0.07 * b));  // luminosity
            }
        }

        out.put(0, 0, data);
        return out;
    }
    public static Mat findTemplete(Mat src, Mat template, int match_method)
    {
        Mat srcClone = src.clone();

        int result_cols = src.cols() - template.cols() + 1;
        int result_rows = src.rows() - template.rows() + 1;
        Mat result = new Mat(result_rows, result_cols, CvType.CV_32FC1);
        Imgproc.matchTemplate(src, template, result, match_method);
        Core.normalize(result, result, 0, 1, Core.NORM_MINMAX, -1, new Mat());
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

        //擷取ROI區
        Mat subSrc = srcClone.submat((int) matchLoc.y,
                                     (int)(matchLoc.y + template.rows()), (int)matchLoc.x,
                                     (int)(matchLoc.x + template.cols()));

        //比對結果
        if (isTheSame(subSrc, template)) {
            //若有抽檢像素值有符合,則畫出黑框
            Imgproc.rectangle(src, matchLoc, new Point(matchLoc.x + template.cols(),
                              matchLoc.y + template.rows()), new Scalar(0, 0, 0), 3);
        }

        return src;
    }


    //比對2個Mat的3個像素值是否類似
    public static boolean isTheSame(Mat img1, Mat img2)
    {
        //System.out.println("img1-size="+img1.size());
        //System.out.println("img2-size="+img2.size());

        boolean isTheSame = false;
        //like抽檢
        double dataA0 = img1.get(0, 0)[0];
        double dataA1 = img1.get(0, 1)[0];
        double dataA2 = img1.get(0, 2)[0];
        double dataA3 = img1.get(0, 3)[0];
        double dataA4 = img1.get(0, 0)[1];
        double dataA5 = img1.get(0, 1)[1];
        double dataA6 = img1.get(0, 2)[1];
        double dataA7 = img1.get(0, 0)[2];
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



}
