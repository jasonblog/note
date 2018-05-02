window.addEventListener("load", function() {
   start_P_event();
   start_H3_event();
   start_DIV_event();
})
function remove_H3_event() {
    	var h3_node =  document.getElementById("h3_title");
    	h3_node.removeEventListener("click",  click_h3, false);
}
function start_H3_event() {
    	var h3_node =  document.getElementById("h3_title");
    	h3_node.addEventListener("click", click_h3, false);
}
function click_h3() {
    	alert("您點選的是<h3>")
}
function remove_P_event() {
    	var p_node =  document.getElementById("p_poetry");
    	p_node.removeEventListener("click",   click_p, false);
}
function start_P_event() {
    	var p_node =  document.getElementById("p_poetry");
    	p_node.addEventListener("click",  click_p, false);
}
function click_p() {
    	alert("您點選的是<p>")
}
function remove_DIV_event() {
    	var div_node =  document.getElementById("div_poetry");
    	div_node.removeEventListener("click",  click_div, false);
}
function start_DIV_event() {
    	var div_node =  document.getElementById("div_poetry");
    	div_node.addEventListener("click",  click_div, false);
}
function click_div() {
    	alert("您點選的是<div>")
}