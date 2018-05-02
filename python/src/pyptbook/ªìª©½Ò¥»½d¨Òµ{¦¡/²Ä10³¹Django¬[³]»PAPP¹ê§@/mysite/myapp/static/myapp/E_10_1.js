$(document).ready(function(){
$("#btn").click(function(){
  var amount = $("#amount").val();
  var rate = $("#rate").val();
  var period = $("#period").val(); 
  $.ajax({
      type: 'GET',      //傳送方式
      url: "/result",  //傳送目的地
      data:{            //要傳送的資料   
        'amount':amount,
        'rate':rate,
        'period':period    
      },
      success: function (Data) {  /*假設後端執行"成功"後,做以下動作
(Data是回傳內容) */
      $("#result").html(Data)
    },
        error: function (e) {     /*假設後端執行"失敗"後,
做以下動作(e是錯誤訊息內容) */
        console.log(e);
      }  
    })
  });
});
