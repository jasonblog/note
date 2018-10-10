<%@ page language="java" contentType="text/html; charset=BIG5"
    pageEncoding="BIG5"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=BIG5">
<title>Insert title here</title>
</head>
<body>
高斯模糊測試!
<br>
<%
String ksize=request.getParameter("ksize");
if (ksize.length()==0){
	ksize="1";
}
%>
ksize=<%=ksize %><br>
<img src="http://localhost:8080/opencvWeb/doOpencvBlur?ksize=<%=ksize%>">
 
</body>
</html>