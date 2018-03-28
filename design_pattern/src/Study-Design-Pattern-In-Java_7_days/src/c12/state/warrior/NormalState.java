package c12.state.warrior;
/**
 * 隨著HP變化的狀態(ConcreteState)，HP > 70% ， 一般狀態　
 */
public class NormalState implements State{
	/**
	 * 狀態不同，行為模式不同(傳入warrior所以狀態可以取得warrior的資料)
	 * @param warrior 
	 */
	@Override 	
	public void move(Warrior warrior) {
		if(warrior.getHP() > 70){
			System.out.println("HP=" + warrior.getHP() + " , 一般狀態 ");	
		} else {
			warrior.setState(new FuryState());
			warrior.move();
		}
	}
}
