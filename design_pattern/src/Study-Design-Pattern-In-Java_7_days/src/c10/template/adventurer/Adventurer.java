package c10.template.adventurer;
/**
 * 進入迷宮的冒險者介面
 */
public abstract class Adventurer {
	protected int level ; //冒險者等級
	protected String type ; // 冒險者類別
	
	public String getType(){
		return this.type;
	};
	
	public int getLevel(){
		return this.level;
	};
	
	public void setLevel(int level){
		this.level = level;
	};
}
