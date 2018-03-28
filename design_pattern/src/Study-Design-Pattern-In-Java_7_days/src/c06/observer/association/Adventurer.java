package c06.observer.association;
/**
 * 冒險者(Observer)
 */
public abstract class Adventurer {
	protected String name;
	
	public Adventurer(String name){
		this.name = name;
	}
	/**
	 * 冒險者接受任務
	 */
	public abstract void getQuestions(String questions);
}
