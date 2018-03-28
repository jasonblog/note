package c12.state.warrior;
/**
 * 隨著HP變化的狀態(State)
 */
public interface State {
	/**
	 * 狀態不同，行為模式不同(傳入warrior所以狀態可以取得warrior的資料)
	 * @param warrior 
	 */
	void move(Warrior warrior);
}
