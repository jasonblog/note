$(document).ready(function(){
	$("#btn_insert").click(function(){
		document.getElementById("result").innerHTML = '抓取中.... 若期間過長，請稍後1分鐘!';
		var stock_id = $("#insert_stock").val();
		var start_date = $("#start_date").val();
		var end_date = $("#end_date").val();

		$.ajax({
				type: 'GET',        //傳送方式
				url: "/E_14_2_Py",    //傳送目的地
				data:{              //要傳送的資料            
					'stock_id':stock_id,
					'start_date':start_date,
					'end_date':end_date
				},
				success: function (Data) {  //假設後端執行"成功"後,做以下動作(Data是回傳內容) 
				$("#result").html(Data)
			},
				error: function (e) {       //假設後端執行"失敗"後,做以下動作(e是錯誤訊息內容)
				console.log(e);
			}
		}); 
	})
});
