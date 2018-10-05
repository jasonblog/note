
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


@WebServlet("/loadDLL")
public class loadOpencvDLL extends HttpServlet
{
    private static final long serialVersionUID = 1L;


    public loadOpencvDLL()
    {
        super();
    }

    protected void doGet(HttpServletRequest request,
                         HttpServletResponse response) throws ServletException, IOException {
        try
        {
            System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
            //System.load(arg0);
            PrintWriter out = response.getWriter();
            //out.write("Load DLL ok!");

            out.println("<!DOCTYPE html>");
            out.println("<html>");
            out.println("<head>");
            out.println("<title>Hello! World!</title>");
            out.println("</head>");
            out.println("<body>");
            out.println("Load DLL ok!");
            out.println("<a href=http://127.0.0.1:8080/opencvWeb/test2.jsp>next page</a>");
            out.println("<h1>Hello! Opencv!</h1>");
            out.println("</body>");
            out.println("</html>");
        } catch (Exception e) //錯誤處理
        {

        }
    }

    protected void doPost(HttpServletRequest request,
                          HttpServletResponse response) throws ServletException, IOException {
    }

}
