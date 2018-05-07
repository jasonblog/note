window.addEventListener("load", function() {
    	showplot();
})
function showplot() {
    	var ExpP = document.getElementById("epf");
    	var xi = []; var yi = []; var k = 0;
    	for (i = 0; i < 5.5; i=i+0.05) {
            	xi[k] = i;
            	yi[k] = Math.cos(Math.sqrt(i));
            	k+=1;
    	}
    	console.log(k);
    	Plotly.plot( ExpP, [{
        	x: xi,
        	y: yi,
        	type: 'bar',
        	marker: {
              	color: 'DarkOrange'
            	}  	  
       	}]);
}