package c12.state.warrior;
/**
 * 隨著HP變化的狀態(ConcreteState)，HP小於30%，背水一戰狀態
 */
public class DesperateState implements State{
	/**
	 * 狀態不同，行為模式不同(傳入warrior所以狀態可以取得warrior的資料)
	 * @param warrior 
	 */
	@Override 	
	public void move(Warrior warrior) {
		int hp = warrior.getHP();
		if(hp == 0){
			warrior.setState(new UnableState());
			warrior.move();
		} else if ( hp > 30 ) {
			warrior.setState(new FuryState());
			warrior.move();
		} else {
			System.out.println("HP=" + warrior.getHP() + " ,背水一戰 傷害增加50%, 防禦增加50%");	
		} 
	}
}
