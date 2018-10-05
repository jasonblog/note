package ch04;
import org.opencv.core.*;
import org.opencv.highgui.Highgui;
import org.opencv.highgui.VideoCapture;

public class Ch04_2_2TakeAPicClearByWebCam
{
    static{ System.loadLibrary(Core.NATIVE_LIBRARY_NAME); }
    public static void main(String args[])
    {
        VideoCapture camera = new VideoCapture(0);

        if (!camera.isOpened()) {
            System.out.println("Error");
        } else {
            Mat frame = new Mat();

            try {
                System.out.println("使用webcam拍照明亮版");

                camera.read(frame);
                Thread.sleep(500);
                camera.read(frame);
                Thread.sleep(500);
                camera.read(frame);
                Thread.sleep(500);
                Highgui.imwrite("c:\\opencv3\\samples\\camera_clear.jpg", frame);
                System.out.println("拍照完成!");
            } catch (Exception e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }

        camera.release();
    }
}