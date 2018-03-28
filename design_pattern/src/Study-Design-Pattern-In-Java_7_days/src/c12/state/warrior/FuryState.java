package c12.state.warrior;
/**
 * 隨著HP變化的狀態(ConcreteState)，HP < 70% ， 狂怒狀態　
 */
public class FuryState implements State{
	/**
	 * 狀態不同，行為模式不同(傳入warrior所以狀態可以取得warrior的資料)
	 * @param warrior 
	 */
	@Override 	
	public void move(Warrior warrior) {
		int hp = warrior.getHP();
		if( hp > 70){
			warrior.setState(new NormalState());
			warrior.move();
		} else if (hp <= 30) {
			warrior.setState(new DesperateState());
			warrior.move();
		} else {
			System.out.println("HP=" + warrior.getHP() + " ,狂怒狀態 傷害增加30%");	
		}
	}
}
