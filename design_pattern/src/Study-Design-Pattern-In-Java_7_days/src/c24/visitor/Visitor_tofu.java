package c24.visitor;
/**
 * 指定做豆腐(Concrete Visitor)
 */
public class Visitor_tofu implements Visitor {

	@Override
	public void cook(DarkChef chef) {
		System.out.println(chef.getName() + " : 豆腐三重奏");
	}

	@Override
	public void cook(SuperChef chef) {
		System.out.println(chef.getName() + " : 熊貓豆腐");
	}

	@Override
	public void cook(SuperNoodleChef chef) {
		System.out.println(chef.getName() + " : 鐵桿臭豆腐");
	}

}
