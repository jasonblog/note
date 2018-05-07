function showfirstChild() {
    	var w = document.getElementById("fclass").firstChild.innerHTML;
    	document.getElementById("result").innerHTML = w;
}
function showlastChild() {
    	var w = document.getElementById("fclass").lastChild.innerHTML;
    	document.getElementById("result").innerHTML = w;
}
function shownodeName() {
    	var w = document.getElementById("fclass").nodeName;
    	document.getElementById("result").innerHTML = w;
}
function shownodeType() {
    	var w = document.getElementById("fclass").nodeType;
    	document.getElementById("result").innerHTML = w;
}
function showparentNode() {
    	var w = document.getElementById("fclass").parentNode.nodeName;
    	document.getElementById("result").innerHTML = w;
}