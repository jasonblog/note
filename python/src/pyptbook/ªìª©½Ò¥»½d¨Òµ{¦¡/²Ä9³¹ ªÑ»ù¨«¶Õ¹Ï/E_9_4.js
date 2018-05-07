window.addEventListener("load", function() {
  if (window.File && window.FileReader && window.FileList && window.Blob) {
      //alert("Great success! All the File APIs are supported.");
      openFile();
    } else {
      alert('The File APIs are not fully supported in this browser.');
    }
})
function openFile(event) {
  var input = event.target;
  var reader = new FileReader();
  var numary;
  reader.onload = function(){
    var text = [];
    var node = document.getElementById('output');
    text = reader.result;
    numary = strtary(text);
    drawplot(numary);
  }
  reader.readAsText(input.files[0]);
}
function strtary(rawdata) {
  var sizeary = rawdata.length;
  console.log("size = " + sizeary);
  var strbyline = rawdata.split('\n');
  var row = strbyline.length-1;
  var col = 90;
  num = new Array(row);
  for (i = 0; i < row; i++) {
    if (i == 0) {
      strbyline[i]=strbyline[i].replace(/\//g,"-");
      num[i] = new Array(col);
      num[i] = strbyline[i].split("\t");
 
    } else {
        num[i] = new Array(col);
        num[i] = strbyline[i].split("\t");
 
    }
    console.log("num = " + num[i]);
  }
  return num;
}
function drawplot(numary) {
  var trace1 = {	
    x: numary[0],
    open: numary[1],
    high: numary[2],
    low: numary[3],
    close: numary[4],
    decreasing: {line: {color: '#7F7F7F'}},
    increasing: {line: {color: '#17BECF'}},
    line: {color: 'rgba(31,119,180,1)'},
    type: 'candlestick',
    xaxis: 'x',
    yaxis: 'y'
  };
  var data = [trace1];
  var layout = {
    dragmode: 'zoom',
    margin: {
      r: 10,
      t: 25,
      b: 40,
      l: 60
    },
    showlegend: false,
    xaxis: {
      autorange: true,
      domain: [0, 1],
      range: ['2017-01-02', '2017-05-05'],
      rangeslider: {range: ['2017-01-02', '2017-05-05']},
      title: 'Date',
      type: 'date'
    },
    yaxis: {
      autorange: true,
      domain: [0, 1],
      range: [150, 200],
      type: 'linear'
    },
    height: "auto",
    width: "auto"
  };
  Plotly.plot('output', data, layout);
}