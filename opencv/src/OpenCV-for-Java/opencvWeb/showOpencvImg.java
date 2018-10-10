
import java.awt.image.BufferedImage;
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
import org.opencv.imgcodecs.Imgcodecs;

/**
 * Servlet implementation class showImg
 */
@WebServlet("/showOpencvImg")
public class showOpencvImg extends HttpServlet
{
    private static final long serialVersionUID = 1L;

    /**
     * @see HttpServlet#HttpServlet()
     */
    public showOpencvImg()
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
            //System.load(arg0);

            Mat source = Imgcodecs.imread("C://opencv3.1//samples//lena.jpg");
            MatOfByte buffer = new MatOfByte();
            Imgcodecs.imencode(".jpg", source, buffer);//對影像編碼並且放置於緩衝記憶體
            byte data1[] = buffer.toArray(); //轉陣列
            response.setContentType("image/jpeg"); //設置返回的文件類型
            OutputStream toClient =
            response.getOutputStream(); //得到向客戶端輸出二進制數據的對象
            toClient.write(data1); //輸出數據.web的作法
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
