package c10.template.maze;

import c10.template.adventurer.Adventurer;

/**
 * 迷宮樣版(Template)-規範迷宮冒險的演算法
 */
public abstract class MazeTemplate {
	protected int difficulty ; // 迷宮難度
	protected Adventurer adventurer; // 進入迷宮的冒險者
	protected boolean isDoubleMaze = false ; // hook，決定是否有隱藏的迷宮
	
	/** 
	 * @param adventurer 進入迷宮的冒險者
	 * @return 
	 */
	public Adventurer adventure(Adventurer adventurer){
		this.adventurer = adventurer;
		
		// 確認冒險者等級
		if(!checkLevel(adventurer.getLevel())) {
			System.out.println("冒險者等級不足，請提升等級至 " + difficulty  + " 後開放迷宮");
		} else {
			System.out.println("---" + adventurer.getType()  + "開始進行困難度 " + difficulty + " 的迷宮");
			createMaze();		// 產生迷宮
			start();  			// 冒險者闖迷宮
			
			if(isDoubleMaze){
				hiddenMaze(); 		// 由掛勾hook決定是否有隱藏迷宮，有的話可以進入隱藏關卡
			}
			showResult();		// 結算冒險結果
		}
		return this.adventurer; 
	}
	
	/**
	 * 冒險者等級是否足夠
	 * @param level
	 * @return
	 */
	private boolean checkLevel(int level){
		if(level < difficulty){
			return false;
		} 
		return true;
	}
	
	/**
	 * 產生迷宮內容(由子類別實作)
	 * @return
	 */
	abstract void createMaze();
	
	/**
	 * 冒險者進入迷宮(由子類別實作)
	 * @return
	 */
	abstract void start();
	
	/**
	 * 進入隱藏迷宮(隱藏迷宮，由hook觸發)
	 * @return
	 */
	void hiddenMaze(){
		System.out.println("進入隱藏迷宮");
	};
	
	/**
	 * 顯示冒險結果
	 */
	Adventurer showResult(){
		this.adventurer.setLevel(adventurer.getLevel() + 50*difficulty);  // 完成迷宮後冒險者等級增加
		System.out.println("---" + adventurer.getType() + "完成困難度 " + difficulty + " 迷宮!!!");
		return this.adventurer;
	};
}
