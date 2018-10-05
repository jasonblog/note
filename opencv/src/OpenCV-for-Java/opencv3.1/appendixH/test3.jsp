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
test!<br>
<%

Mat m1 = new Mat(2, 2, CvType.CV_32FC1);
m1.put(0, 0, 1);
m1.put(0, 1, 2);
m1.put(1, 0, 3);
m1.put(1, 1, 4);

out.println("矩陣m1的行數="+m1.cols()+"<br>");
out.println("矩陣m1的列數="+m1.rows()+"<br>");
out.println("矩陣m1所有元素個數="+m1.total()+"<br>");
out.println("矩陣m1的Size="+m1.size()+"<br>");
out.println("矩陣m1(1,1)的元素="+m1.get(0,0)[0]+"<br>");
out.println("矩陣m1(2,2)的元素="+m1.get(1,1)[0]+"<br>");
out.println("矩陣m1所有元素="+m1.dump()+"<br>");

%>
</body>
</html>