
import java.io.*;
import java.text.*;
import java.util.*;
import java.io.IOException;
import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * Servlet implementation class showImg
 */
@WebServlet("/showImg")
public class showImg extends HttpServlet
{
    private static final long serialVersionUID = 1L;

    /**
     * @see HttpServlet#HttpServlet()
     */
    public showImg()
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
            FileInputStream hFile = new
            FileInputStream("C:\\opencv3.1\\samples\\lena.jpg"); //以byte流的方式打開文件 d:\1.gif
            int i = hFile.available(); //得到文件大小
            byte data[] = new byte[i];
            hFile.read(data); //讀數據
            hFile.close();
            response.setContentType("image/jpeg"); //設置返回的文件類型
            OutputStream toClient =
            response.getOutputStream(); //得到向客戶端輸出二進制數據的對象
            toClient.write(data); //輸出數據
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
