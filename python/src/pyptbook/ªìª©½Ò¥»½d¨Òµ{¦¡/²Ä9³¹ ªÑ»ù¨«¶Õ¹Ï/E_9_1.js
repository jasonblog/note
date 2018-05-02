window.addEventListener("load", function() {
    	showplot();
})
function showplot() {
    	var ExpP = document.getElementById("epf");
    	Plotly.plot( ExpP, [{
        	x: [1, 2, 3, 4, 5, 6, 7],
        	y: [0.125, 0.25, 0.5, 1, 2, 4, 8],
        	marker: {
              	color: 'DodgerBlue'
            	} }], {
        	margin: { t: 1 } } );
}