window.addEventListener("load", function() {
    	showplot();
})
function showplot() {
    	var ExpP = document.getElementById("epf");
    	var xi = []; var yi = [];
    	for (i = 1; i <= 7; i+=1) {
            	xi[i-1] = i;
            	yi[i-1] = Math.pow(2, i);
    	}
    	console.log(xi);
    	console.log(yi);
    	Plotly.plot( ExpP, [{
        	x: xi,
        	y: yi,
        	marker: {
              	color: 'red'
            	} }], {
        	margin: { t: 1 } } );
}