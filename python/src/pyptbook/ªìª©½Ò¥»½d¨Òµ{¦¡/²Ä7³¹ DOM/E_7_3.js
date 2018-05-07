function ById() {
    	document.getElementById("result").innerHTML = "getElementById()";
}
function ByTagName() {
    var w = document.getElementsByTagName("LI");
    document.getElementById("result").innerHTML = w[2].innerHTML;
}
function ByName() {
    var w = document.getElementsByName("inv_exper")[0].tagName;
    document.getElementById("result").innerHTML = w;
}
function ByClassName() {
    var w = document.getElementsByClassName("format");
    var temp = "";
    for (i = 0; i<w.length; i++) {
        	temp += w[i].tagName + "  ";
    }
    document.getElementById("result").innerHTML = temp;
}