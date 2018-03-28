package c03.abstractfactory.village;

/**
 * 鬥士
 */
public class Warrior extends Adventurer {
	@Override public void display() {
		System.out.println("我是弓箭手，裝備:");
		weapon.display();
		System.out.println();
		clothes.display();
		System.out.println();
	}
}
