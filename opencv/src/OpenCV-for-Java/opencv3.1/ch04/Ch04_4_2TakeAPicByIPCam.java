package ch04;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.videoio.VideoCapture;

public class Ch04_4_2TakeAPicByIPCam
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }

    public static void main(String arg[])
    {
        VideoCapture capture = new VideoCapture();
        capture.open("http://username:password@192.168.1.4:8080/mjpg/video.mjpg");

        if (!capture.isOpened()) {
            System.out.println("Error");
        } else {
            Mat webcam_image = new Mat();
            capture.read(webcam_image);

            Imgcodecs.imwrite("C://opencv3.1//samples//TakeAPicByIPCam.png", webcam_image);
            System.out.println("使用IPCam拍照完成!");
        }
    }
}
