package c01.simpleFactory.village;
/**
 * 弓箭手
 * @author Yan
 *
 */
public class Archer implements Adventurer {

	@Override
	public String getType() {
		System.out.println("我是弓箭手");
		return  this.getClass().getSimpleName();
	}
}
