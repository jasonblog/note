package c12.state.warrior;

/**
 * 鬥士類別(Context)
 */
public class Warrior {
	private int hp ; 		// 生命值(直接以0~100表示)
	private State state; 	// 目前狀態

	public Warrior(){
		 // 一開始為滿HP狀態
		this.hp = 100 ;
		state = new NormalState();
	}
	
	/**
	 * 治療-恢復HP
	 * @param time
	 */
	public void heal(int heal){
		this.hp +=  heal;
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
		state.move(this);
	}

	public void setState(State state){
		this.state = state;
	}
	public int getHP(){
		return this.hp;
	}
}
