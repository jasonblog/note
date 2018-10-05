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
import org.opencv.imgcodecs.Imgcodecs;

public class Ch10_25_1ScreenSaver
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    static void save(String path) throws Exception {
        Robot robot = new Robot();
        Dimension d = Toolkit.getDefaultToolkit().getScreenSize();
        Rectangle rect = new Rectangle(0, 0, d.width, d.height);
        BufferedImage image = robot.createScreenCapture(rect);
        ImageIO.write(image, "jpg", new File(path));

    }
    //      public static void main(String[] args) {
    //      try{
    //      Ch10_25_1ScreenSaver.save("C://opencv3.1//samples/MyScreen.png");
    //      }catch(Exception e){
    //      e.printStackTrace();
    //      }
    //      }

    public static void main(String[] args) throws AWTException {
        Robot robot = new Robot();
        robot.delay(5000);
        Dimension d = Toolkit.getDefaultToolkit().getScreenSize();
        Rectangle rect = new Rectangle(0, 0, d.width, d.height);
        BufferedImage image = robot.createScreenCapture(rect);
        //BufferedImage im = new BufferedImage(image.getWidth(), image.getHeight(),BufferedImage.TYPE_3BYTE_BGR);
        System.out.println("image.getType()=" + image.getType());
        System.out.println("BufferedImage.TYPE_BYTE_GRAY=" + BufferedImage.TYPE_BYTE_GRAY);
        System.out.println("BufferedImage.TYPE_4BYTE_ABGR=" + BufferedImage.TYPE_4BYTE_ABGR);
        System.out.println("BufferedImage.TYPE_INT_BGR=" + BufferedImage.TYPE_INT_BGR);
        System.out.println("BufferedImage.TYPE_INT_RGB=" + BufferedImage.TYPE_INT_RGB);
        Mat result = bufferedImg2Mat(image);

        Imgcodecs.imwrite("C://opencv3.1//samples/MyScreen.png", result);
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


}
