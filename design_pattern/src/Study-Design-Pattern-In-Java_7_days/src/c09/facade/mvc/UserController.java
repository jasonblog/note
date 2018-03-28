package c09.facade.mvc;

/**
 * 處理會員註冊的controller
 */
public class UserController {
	// 顯示使用者詳細資料
	public String showUserDetail(Object object){
		
		
		return "使用者詳細資料頁面";
	}
	
	// 註冊會員
	public String register(Object object){
		String result = "註冊成功頁面";
		
		return result;
	}
	
}
