package c01.simpleFactory.village;
/**
 * 鬥士
 * @author Yan
 *
 */
public class Warrior implements Adventurer {

	@Override
	public String getType() {
		System.out.println("我是鬥士");	
		return  this.getClass().getSimpleName();
	}
	
}
