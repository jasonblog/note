package c11.composite.organizationTree;
/**
 * 人力支援部門(Leaf)
 */
public class HumanResouce extends Department {

	public HumanResouce(String name) {
		super(name);
	}

	/**
	 * 部門實際的工作由子類別決定
	 */
	@Override
	public void lineOfDuty() {
		System.out.println(name +  ":想辦法拐騙冒險者來完成任務");
	}

}
