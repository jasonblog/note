package c11.composite.organizationTree;
/**
 * 客服部門(Leaf)
 */
public class ServiceDepartment extends Department {

	public ServiceDepartment(String name) {
		super(name);
	}

	/**
	 * 部門實際的工作由子類別決定
	 */
	@Override
	public void lineOfDuty() {
		System.out.println(name +  ":處理客訴，告訴客戶，這肯定是冒險者的錯，不是協會的錯");
	}

}
