<%@ page language="java" contentType="text/html; charset=BIG5"
    pageEncoding="BIG5"%>
<%@ page import="org.opencv.core.Core,org.opencv.core.CvType,org.opencv.core.Mat,org.opencv.core.Scalar"  %> 
   
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=BIG5">
<title>Insert title here</title>
</head>
<body>
test!
<%

//System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
out.println("Hello OpenCV " + Core.VERSION);

Mat m1  = Mat.zeros(1, 3, CvType.CV_8UC1);
Mat m2  = Mat.zeros(1, 3, CvType.CV_8UC3);

Mat m3  = Mat.ones(5, 5,CvType.CV_8UC1);
Mat m6  = Mat.ones(5, 5,CvType.CV_8UC3);

Mat m7  = Mat.eye(3, 3, CvType.CV_8UC1);
Mat m8  = Mat.eye(3, 3, CvType.CV_8UC3);
Mat m9  = new Mat(5, 10, CvType.CV_8UC1,new Scalar(0));

out.println("<p><pre>m1 = " + m1.dump()+"</pre>");
out.println("<p><pre>m2 = " + m2.dump()+"</pre>");
out.println("<p><pre>m3 = " + m3.dump()+"</pre>");
out.println("<p><pre>m6 = " + m6.dump()+"</pre>");
out.println("<p><pre>m7 = " + m7.dump()+"</pre>");
out.println("<p><pre>m8 = " + m8.dump()+"</pre>");
out.println("<p><pre>m9 = " + m9.dump()+"</pre>");
out.println("<p><pre>m9 = " + m9.dump()+"</pre>");

%>
<a href="http://127.0.0.1:8080/opencvWeb/test3.jsp">next page</a>
</body>
</html>