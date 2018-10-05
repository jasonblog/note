package ch04;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.concurrent.TimeUnit;

import javax.swing.JButton;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.highgui.VideoCapture;

import com.xuggle.mediatool.IMediaWriter;
import com.xuggle.mediatool.ToolFactory;
import com.xuggle.xuggler.IRational;

public class Ch04_2_6RecordByWebCam
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }
    static boolean isWhile = true;
    private static IRational FRAME_RATE = IRational.make(3, 1);

    public static void main(String arg[]) throws InterruptedException {
        VideoCapture capture = new VideoCapture(0);
        JFrame frame1 = new JFrame("show image");
        frame1.setTitle("從webcam錄影並存檔");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(680, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        frame1.getContentPane().setLayout(null);
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);

        if (!capture.isOpened())
        {
            System.out.println("Error");
        } else
        {
            final Mat webcam_image = new Mat();
            capture.read(webcam_image);
            frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);

            String outFile = "c:\\opencv3.1\\samples\\output2b.mp4";
            final IMediaWriter writer = ToolFactory.makeWriter(outFile);

            JButton btnNewButton = new JButton("停止錄影");

            btnNewButton.setBounds(680, 97, 87, 23);
            frame1.getContentPane().add(btnNewButton);
            btnNewButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent arg0) {
                    isWhile = false;
                    writer.close();
                }
            });

            writer.addVideoStream(0, 0, FRAME_RATE, 680, 480);
            long startTime = System.nanoTime();
            int index = 0;

            while (isWhile) {
                capture.read(webcam_image);
                System.out.println("使用webcam錄影並存檔");
                panel1.setimagewithMat(webcam_image);
                frame1.repaint();

                writer.encodeVideo(0, panel1.matToBufferedImage(webcam_image),
                                   System.nanoTime() - startTime, TimeUnit.NANOSECONDS);
                Thread.sleep((long)(1000 / FRAME_RATE.getDouble()));
                index++;
                System.out.println("encoded image: " + index);

            }
        }
    }
}
