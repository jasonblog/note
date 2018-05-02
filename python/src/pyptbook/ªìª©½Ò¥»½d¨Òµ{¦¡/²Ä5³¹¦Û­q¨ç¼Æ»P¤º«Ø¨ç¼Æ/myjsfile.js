function futurevalue(){
	var pv = parseInt(document.getElementById("amount").value);
	var i = parseFloat(document.getElementById("rate").value);
	var n = parseInt(document.getElementById("period").value);
	fv=pv*(Math.pow(1+i,n));
	document.write("本利合 = " + fv + "</br>");
}