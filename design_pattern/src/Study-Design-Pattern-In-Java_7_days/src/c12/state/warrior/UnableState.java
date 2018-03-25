package c12.state.warrior;
/**
 * 隨著HP變化的狀態(ConcreteState)，HP = 0% ， 無法戰鬥狀態　
 */
public class UnableState implements State{
	/**
	 * 狀態不同，行為模式不同(傳入warrior所以狀態可以取得warrior的資料)
	 * @param warrior 
	 */
	@Override 	
	public void move(Warrior warrior) {
		System.out.println("HP=" + warrior.getHP() + " , 無法戰鬥");	
	}
}
