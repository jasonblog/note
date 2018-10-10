<%@ page language="java" contentType="text/html; charset=BIG5"
    pageEncoding="BIG5"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=BIG5">
<title>Opencv Web test blur</title>
</head>
<body>
高斯模糊測試!
<img src="http://localhost:8080/opencvWeb/showImg">
<p><hr><p>
<form method=post action=showBlur.jsp>
kSize:<select name=ksize>
<option selected="selected">1
<option>5
<option>7
<option>9
<option>11
<option>13
<option>15
<option>19
<option>21

</select>

<input type="submit" name="submit" value="processing"><p>
</form>
 
</body>
</html>
