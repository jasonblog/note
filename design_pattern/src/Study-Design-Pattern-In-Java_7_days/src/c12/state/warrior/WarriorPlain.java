package c12.state.warrior;

/**
 * 沒使用策略模式的鬥士類別
 */
public class WarriorPlain {
	private int hp ; 		// 生命值(直接以0~100表示)

	public WarriorPlain(){
		 // 一開始為滿HP狀態
		this.hp = 100 ;
	}
	
	/**
	 * 治療-恢復HP
	 * @param time
	 */
	public void heal(int heal){
		// 無法戰鬥的時候不能接受治療
		if(hp == 0) {
			hp = 0;
		} else {
			this.hp +=  heal;
		}
		if(hp > 100) {
			hp = 100;
		}
	}
	
	/**
	 * 受傷-減少hp
	 * @param damage
	 */
	public void getDamage(int damage){
		this.hp -= damage;
		if(hp < 0) {
			hp = 0;
		}
	}
	
	/**
	 * 將產生怪物的策略交給Status處理
	 */
	public void move(){
		if(hp == 0){
			System.out.println("無法戰鬥");
			//...麻煩自行想像下面還有幾十行才能玩成狀態設定
			return ;
		}
		if(hp > 70) {
			System.out.println("一般狀態");
			//...麻煩自行想像下面還有幾十行才能玩成狀態設定
		} else if (hp < 30) {
			System.out.println("背水一戰狀態");
			//...麻煩自行想像下面還有幾十行才能玩成狀態設定
		} else {
			System.out.println("狂怒狀態");
			//...麻煩自行想像下面還有幾十行才能玩成狀態設定			
		}
		//.....這邊假設 if else 總共還有十幾種不同的狀態
	}
}
