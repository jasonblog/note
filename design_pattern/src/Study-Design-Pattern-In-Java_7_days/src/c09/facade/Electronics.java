package c09.facade;
/**
 * 電子產品介面，全部的電子產品都可以開關電源
 */
public abstract class Electronics {
	private boolean power = false;   // 電源
	
	// 啟動電源
	public void powerOn() {
		this.power = true;
	}
	// 關閉電源
	public void powerOff() {
		this.power = false;
	}
	// 電源是否有開
	public boolean isPowerOn() {
		return power;
	}
	// 顯示機器狀態
	protected void showStatus(){
		if(power){
			System.out.println(this.getClass().getSimpleName() + "運作中");
		} else {
			System.out.println(this.getClass().getSimpleName() + "電源未開啟");
		}
	}
}
