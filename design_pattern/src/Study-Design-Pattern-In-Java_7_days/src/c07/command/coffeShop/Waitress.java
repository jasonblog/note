package c07.command.coffeShop;

import java.util.ArrayList;
import java.util.List;
/**
 * 服務生(Invoker)
 */
public class Waitress {
	private int snackQty = 2; // 製作點心的原料
	private int drinkQty = 4; // 飲料剩餘的杯數 
	private List<Order> orderList = new ArrayList<>();
	
	/**
	 * 服務生接收訂單
	 * @param order
	 */
	public void setOrder(Order order) {
		 
		if(order.name.equals("snackOrder")){
			if(snackQty <= 0){
				System.out.println("點心賣完了");
			} else {
				System.out.println("增加點心訂單");
				snackQty--;
				orderList.add(order);				
			}
		} 
		
		if(order.name.equals("drinkOrder")){
			if(drinkQty <= 0){
				System.out.println("飲料賣完了");
			} else {
				System.out.println("增加飲料訂單");
				drinkQty--;
				orderList.add(order);				
			}
		} 
	}

	/**
	 * 取消訂單
	 * @param order
	 */
	public void cancelOrder(Order order) {
		
		if(order.name.equals("drinkOrder")){
			drinkQty++;
			System.out.println("取消一杯飲料");
		}
		
		if(order.name.equals("snackOrder")){
			snackQty++;
			System.out.println("取消一個點心");
		}
		orderList.remove(order);		
	}
	
	/**
	 * 將訂單送到廚房
	 */
	public void notifyBaker() {
		for(Order order : orderList){
			order.sendOrder();
		}
		orderList.clear();
	}
}
