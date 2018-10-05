
import java.io.*;
import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfByte;
import org.opencv.core.Size;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;

/**
 * Servlet implementation class showImg
 */
@WebServlet("/showOpencvBlurImg")
public class showOpencvBlurImg extends HttpServlet
{
    private static final long serialVersionUID = 1L;

    /**
     * @see HttpServlet#HttpServlet()
     */
    public showOpencvBlurImg()
    {
        super();
        // TODO Auto-generated constructor stub
    }

    /**
     * @see HttpServlet#doGet(HttpServletRequest request, HttpServletResponse response)
     */
    protected void doGet(HttpServletRequest request,
                         HttpServletResponse response) throws ServletException, IOException {
        try
        {
            System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
            Mat source = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg");
            Mat destination = new Mat(source.rows(), source.cols(), source.type());

            Imgproc.GaussianBlur(source, destination, new Size(9, 9), 0, 0);
            MatOfByte buffer = new MatOfByte();
            Imgcodecs.imencode(".jpg", destination, buffer);
            byte data1[] = buffer.toArray();
            response.setContentType("image/jpeg"); //設置返回的文件類型
            OutputStream toClient =
            response.getOutputStream(); //得到向客戶端輸出二進制數據的對象
            toClient.write(data1); //輸出數據
            toClient.close();
        } catch (IOException e) //錯誤處理
        {
            PrintWriter toClient = response.getWriter(); //得到向客戶端輸出文本的對象
            response.setContentType("text/html;charset=big5");
            toClient.write("無法打開圖片!");
            toClient.close();
        }
    }

    /**
     * @see HttpServlet#doPost(HttpServletRequest request, HttpServletResponse response)
     */
    protected void doPost(HttpServletRequest request,
                          HttpServletResponse response) throws ServletException, IOException {
    }

}
