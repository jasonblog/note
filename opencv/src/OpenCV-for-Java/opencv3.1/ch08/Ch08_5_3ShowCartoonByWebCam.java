package ch08;
import javax.swing.JFrame;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;
import org.opencv.videoio.VideoCapture;

public class Ch08_5_3ShowCartoonByWebCam
{
    static {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    }

    public static void main(String arg[])
    {
        VideoCapture capture = new VideoCapture();
        capture.open(0);
        JFrame frame1 = new JFrame("show image");
        frame1.setTitle("從webcam讀取影像至Java Swing視窗1");
        frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame1.setSize(640, 480);
        frame1.setBounds(0, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel1 = new Panel();
        frame1.setContentPane(panel1);
        frame1.setVisible(true);

        JFrame frame2 = new JFrame("show image");
        frame2.setTitle("從webcam讀取影像至Java Swing視窗2");
        frame2.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame2.setSize(640, 480);
        frame2.setBounds(160, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel2 = new Panel();
        frame2.setContentPane(panel2);
        frame2.setVisible(true);

        JFrame frame3 = new JFrame("show image");
        frame3.setTitle("從webcam讀取影像至Java Swing視窗2");
        frame3.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame3.setSize(640, 480);
        frame3.setBounds(280, 0, frame1.getWidth(), frame1.getHeight());
        Panel panel3 = new Panel();
        frame3.setContentPane(panel3);
        frame3.setVisible(true);

        if (!capture.isOpened()) {
            System.out.println("Error");
        } else {
            Mat webcam_image = new Mat();


            capture.read(webcam_image);
            frame1.setSize(webcam_image.width() + 40, webcam_image.height() + 60);
            frame2.setSize(webcam_image.width() + 50, webcam_image.height() + 70);
            frame2.setSize(webcam_image.width() + 60, webcam_image.height() + 80);

            while (true) {
                capture.read(webcam_image);
                //System.out.println("從webcam讀取影像至Java Swing視窗");
                Mat webcamClone1 = webcam_image.clone();
                Mat webcamClone2 = webcam_image.clone();
                Mat layer1 = Blur(webcamClone1, 85);
                Mat layer2 = BlurAndCanny(webcamClone2, 181, 73, 1, 1, 5);
                Mat result = Merge(layer1, layer2, 2.6, 1, -406);

                panel1.setimagewithMat(webcam_image);
                panel2.setimagewithMat(layer2);
                panel3.setimagewithMat(result);
                frame1.repaint();
                frame2.repaint();
                frame3.repaint();
            }
        }
    }

    public static Mat BlurAndCanny(Mat source, double threshold1, double threshold2,
                                   int GaussianKernelSize, int ksize, int op)
    {
        Mat destination = new Mat(source.rows(), source.cols(), source.type());
        //Imgproc.GaussianBlur(source, destination,new Size(GaussianKernelSize,GaussianKernelSize),0,0);
        Imgproc.Canny(source, destination, threshold1, threshold2);
        // Imgproc.GaussianBlur(destination, destination,new Size(GaussianKernelSize,GaussianKernelSize),0,0);
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

    public static Mat Blur(Mat source1, int GaussianKernelSize)
    {
        Mat processBlur = new Mat(source1.rows(), source1.cols(), source1.type());
        Imgproc.medianBlur(source1, processBlur, GaussianKernelSize);
        // Imgproc.GaussianBlur(source1, processBlur,new Size(GaussianKernelSize,GaussianKernelSize),0,0);
        // System.out.println("processBlur="+processBlur.type());
        // System.out.println("processBlur="+processBlur.channels());
        return processBlur;

    }



    public static Mat Merge(Mat source1, Mat source2, double alpha, double beta,
                            double gamma)
    {
        Mat destination = new Mat(source1.rows(), source1.cols(), source1.type());
        Core.addWeighted(source1, alpha, source2, beta, gamma, destination);
        return destination;

    }
}
