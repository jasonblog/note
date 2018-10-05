package ch04;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Size;
import org.opencv.videoio.VideoCapture;
import org.opencv.videoio.VideoWriter;
import org.opencv.videoio.Videoio;

public class Ch04_2_6_2RecordByWebCam
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }

    public static void main(String arg[])
    {
        /*
         *  CV_FOURCC('P', 'I', 'M', '1') = MPEG-1 codec
        CV_FOURCC('M', 'J', 'P', 'G') = motion-jpeg codec
        CV_FOURCC('M', 'P', '4', '2') = MPEG-4.2 codec
        CV_FOURCC('D', 'I', 'V', '3') = MPEG-4.3 codec
        CV_FOURCC('D', 'I', 'V', 'X') = MPEG-4 codec
        CV_FOURCC('U', '2', '6', '3') = H263 codec
        CV_FOURCC('I', '2', '6', '3') = H263I codec
        CV_FOURCC('F', 'L', 'V', '1') = FLV1 codec
        http://www.fourcc.org/codecs.php
        CV_FOURCC('P','I','M','1')    = MPEG-1 codec

        CV_FOURCC('M','J','P','G')    = motion-jpeg codec (does not work well)

        CV_FOURCC('M', 'P', '4', '2') = MPEG-4.2 codec

        CV_FOURCC('D', 'I', 'V', '3') = MPEG-4.3 codec

        CV_FOURCC('D', 'I', 'V', 'X') = MPEG-4 codec

        CV_FOURCC('U', '2', '6', '3') = H263 codec

        CV_FOURCC('I', '2', '6', '3') = H263I codec

        CV_FOURCC('F', 'L', 'V', '1') = FLV1 codec
         */
        System.out.println("VideoWriter.fourcc('M', 'J', 'P', 'G')=" +
                           VideoWriter.fourcc('M', 'J', 'P', 'G'));
        System.out.println("VideoWriter.fourcc('M', 'P', '4', '2')=" +
                           VideoWriter.fourcc('M', 'P', '4', '2'));
        System.out.println("VideoWriter.fourcc('D', 'I', 'V', '3')=" +
                           VideoWriter.fourcc('D', 'I', 'V', '3'));
        System.out.println("VideoWriter.fourcc('D', 'I', 'V', 'X')=" +
                           VideoWriter.fourcc('D', 'I', 'V', 'X'));
        System.out.println("VideoWriter.fourcc('U', '2', '6', '3')=" +
                           VideoWriter.fourcc('U', '2', '6', '3'));
        System.out.println("VideoWriter.fourcc('I', '2', '6', '3')=" +
                           VideoWriter.fourcc('I', '2', '6', '3'));

        VideoCapture capture = new VideoCapture();
        capture.open(0);
        VideoWriter vw = new VideoWriter("c:\\test.avi", VideoWriter.fourcc('M', 'J',
                                         'P', 'G'), 25, new Size(640, 480));
        JFrame frame1 = new JFrame("show image");
        frame1.setTitle("從webcam讀取影像至Java Swing視窗");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(640, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);

        if (!capture.isOpened()) {
            System.out.println("Error");
        } else {
            Mat webcam_image = new Mat();
            capture.read(webcam_image);
            frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);

            while (true) {
                capture.read(webcam_image);
                vw.write(webcam_image);
                System.out.println("從webcam讀取影像至Java Swing視窗");
                panel1.setimagewithMat(webcam_image);
                frame1.repaint();
            }
        }
    }
}
