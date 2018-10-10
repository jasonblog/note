package ch10;

import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.List;
import javax.swing.JFrame;
import javax.swing.JPanel;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.highgui.VideoCapture;
import org.opencv.imgproc.Imgproc;
import org.opencv.core.CvType;
class Panel extends JPanel
{
    private static final long serialVersionUID = 1L;
    private BufferedImage image;
    // Create a constructor method
    public Panel()
    {
        super();
    }
    private BufferedImage getimage()
    {
        return image;
    }
    public void setimage(BufferedImage newimage)
    {
        image = newimage;
        return;
    }
    public void setimagewithMat(Mat newimage)
    {
        image = this.matToBufferedImage(newimage);
        return;
    }
    /**
     * Converts/writes a Mat into a BufferedImage.
     *
     * @param matrix Mat of type CV_8UC3 or CV_8UC1
     * @return BufferedImage of type TYPE_3BYTE_BGR or TYPE_BYTE_GRAY
     */
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
    @Override
    protected void paintComponent(Graphics g)
    {
        super.paintComponent(g);
        //BufferedImage temp=new BufferedImage(640, 480, BufferedImage.TYPE_3BYTE_BGR);
        BufferedImage temp = getimage();

        //Graphics2D g2 = (Graphics2D)g;
        if (temp != null) {
            g.drawImage(temp, 10, 10, temp.getWidth(), temp.getHeight(), this);
        }
    }
}
