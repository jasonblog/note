package c24.visitor;
/**
 * 指定做燒賣(Concrete Visitor)
 */
public class Visitor_saoMai implements Visitor {

	@Override
	public void cook(DarkChef chef) {
		System.out.println(chef.getName() + " : 魔幻鴉片燒賣");
	}

	@Override
	public void cook(SuperChef chef) {
		System.out.println(chef.getName() + " : 宇宙大燒賣");
	}

	@Override
	public void cook(SuperNoodleChef chef) {
		System.out.println(chef.getName() + " : 鐵桿50人份燒賣");
	}

}
