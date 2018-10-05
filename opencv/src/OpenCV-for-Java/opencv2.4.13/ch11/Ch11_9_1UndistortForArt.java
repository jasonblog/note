package ch11;

import java.awt.EventQueue;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import org.opencv.calib3d.Calib3d;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfPoint2f;
import org.opencv.core.MatOfPoint3f;
import org.opencv.core.Point3;
import org.opencv.core.Size;
import org.opencv.core.TermCriteria;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;
import javax.swing.JSlider;
import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

public class Ch11_9_1UndistortForArt
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
                    Ch11_9_1UndistortForArt window = new Ch11_9_1UndistortForArt();
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
    public Ch11_9_1UndistortForArt()
    {
        initialize();
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize()
    {
        final Mat source = Highgui.imread("C://opencv3.1//samples//lena.jpg");
        //final Mat source = Highgui.imread("C://opencv249//sources//samples//cpp//005.jpg");
        BufferedImage image = matToBufferedImage(source);

        frmjavaSwing = new JFrame();
        frmjavaSwing.setTitle("藝術化扭曲影像");
        frmjavaSwing.setBounds(100, 100, 520, 550);
        frmjavaSwing.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frmjavaSwing.getContentPane().setLayout(null);

        final JLabel showAlphaValue = new JLabel("3");
        showAlphaValue.setBounds(250, 10, 46, 15);
        frmjavaSwing.getContentPane().add(showAlphaValue);

        final JLabel showBetaValue = new JLabel("3");
        showBetaValue.setBounds(250, 35, 46, 15);
        frmjavaSwing.getContentPane().add(showBetaValue);

        final JLabel lblNewLabel = new JLabel("");
        lblNewLabel.setBounds(10, 68, 438, 438);
        lblNewLabel.setIcon(new ImageIcon(image));
        frmjavaSwing.getContentPane().add(lblNewLabel);

        final JSlider slider_beta = new JSlider();
        slider_beta.setValue(3);
        slider_beta.setMinimum(1);
        slider_beta.setMaximum(31);
        slider_beta.setBounds(47, 33, 200, 25);
        frmjavaSwing.getContentPane().add(slider_beta);

        final JSlider slider_alpha = new JSlider();
        slider_alpha.setMinimum(1);
        slider_alpha.setMaximum(31);
        slider_alpha.setValue(3);
        slider_alpha.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                //System.out.println(slider_alpha.getValue());
                showAlphaValue.setText(slider_alpha.getValue() + "");
                BufferedImage newImage = matToBufferedImage(matConvertTo(
                                             slider_alpha.getValue(), slider_beta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });
        slider_alpha.setBounds(47, 10, 200, 25);
        frmjavaSwing.getContentPane().add(slider_alpha);

        JLabel lblAlpha = new JLabel("X");
        lblAlpha.setBounds(10, 20, 46, 15);
        frmjavaSwing.getContentPane().add(lblAlpha);

        JLabel lblBeta = new JLabel("Y");
        lblBeta.setBounds(10, 45, 46, 15);
        frmjavaSwing.getContentPane().add(lblBeta);


        slider_beta.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent arg0) {
                showBetaValue.setText(slider_beta.getValue() + "");
                BufferedImage newImage = matToBufferedImage(matConvertTo(
                                             slider_alpha.getValue(), slider_beta.getValue()));
                lblNewLabel.setIcon(new ImageIcon(newImage));
            }
        });





    }
    public Mat matConvertTo(double alpha, double beta)
    {
        Mat source = Highgui.imread("C://opencv3.1//samples//005.jpg");
        Mat source1 = Highgui.imread("C://opencv3.1//samples//lena.jpg");
        Mat destination = source.clone();
        Imgproc.cvtColor(source, destination, Imgproc.COLOR_BGR2GRAY);
        Size boarderSize = new Size(9, 6);
        MatOfPoint2f imageCorners = new MatOfPoint2f();
        boolean found = Calib3d.findChessboardCorners(destination, boarderSize,
                        imageCorners, Calib3d.CALIB_CB_ADAPTIVE_THRESH +
                        Calib3d.CALIB_CB_NORMALIZE_IMAGE + Calib3d.CALIB_CB_FAST_CHECK);

        if (found) {
            TermCriteria term = new TermCriteria(TermCriteria.EPS | TermCriteria.MAX_ITER,
                                                 30, 0.1);
            Imgproc.cornerSubPix(destination, imageCorners, new Size(11, 11), new Size(-1,
                                 -1), term);
            Calib3d.drawChessboardCorners(source, boarderSize, imageCorners, found);
            //Highgui.imwrite("C://opencv249//sources//samples//cpp//test-Calib3d-2.jpg", source);

        }

        ArrayList<Mat> rvecs = new ArrayList<Mat>();
        ArrayList<Mat> tvecs = new ArrayList<Mat>();
        ArrayList<Mat> objectPoints = new ArrayList<Mat>();
        ArrayList<Mat> imagePoints = new ArrayList<Mat>();
        MatOfPoint3f obj = new MatOfPoint3f();
        Mat cameraMatrix = new Mat(3, 3, CvType.CV_32FC1);
        cameraMatrix.put(0, 0, 1);
        cameraMatrix.put(1, 1, 1);
        Mat distCoeffs = new Mat();
        Mat undistort = new Mat(source.size(), source.type());

        for (int j = 0; j < 54; j++) {
            //obj.push_back(new MatOfPoint3f(new Point3(j/6,j%9,0.0f)));
            obj.push_back(new MatOfPoint3f(new Point3(j / alpha, j % beta, 0.0f)));
        }

        objectPoints.add(obj);
        imagePoints.add(imageCorners);
        Calib3d.calibrateCamera(objectPoints, imagePoints, destination.size(),
                                cameraMatrix, distCoeffs, rvecs, tvecs);
        Imgproc.undistort(source1, undistort, cameraMatrix, distCoeffs);

        return undistort;

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
