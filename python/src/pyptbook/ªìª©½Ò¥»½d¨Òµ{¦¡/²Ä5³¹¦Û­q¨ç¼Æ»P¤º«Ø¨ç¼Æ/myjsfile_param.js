function futurevalue(pv,i,n){
	var fv = pv*(Math.pow(1+i,n));
	document.write("本利合 = " + fv + "</br>");
}
function Getval(name){
	var val =  document.getElementById(name).value;
	return val;
}