package c11.composite;

import org.junit.Test;

import c11.composite.organizationTree.AbstractAssociation;
import c11.composite.organizationTree.Association;
import c11.composite.organizationTree.HumanResouce;
import c11.composite.organizationTree.ServiceDepartment;

/**
 * 合成模式-測試
 */
public class BranchOrganizationTest {
	@Test
	public void test(){
		System.out.println("============合成模式測試============");

		AbstractAssociation root = new Association("冒險者總會");
		root.add(new HumanResouce("總會-人力資源單位"));
		root.add(new ServiceDepartment("總會-客服單位"));
		
		AbstractAssociation mars = new Association("火星分會");
		mars.add(new HumanResouce("火星分會-人力資源單位"));
		mars.add(new ServiceDepartment("火星分會-客服單位"));
		root.add(mars);

		AbstractAssociation saturn = new Association("土星分會");
		saturn.add(new HumanResouce("土星分會-人力資源單位"));
		saturn.add(new ServiceDepartment("土星分會-客服單位"));
		root.add(saturn);
		
		AbstractAssociation m1 = new Association("土衛1號辦事處");
		m1.add(new HumanResouce("土衛1號辦事處-人力資源單位"));
		m1.add(new ServiceDepartment("土衛1號辦事處-客服單位"));
		saturn.add(m1);
		
		// 地區偏遠，沒人會過來客服的地方
		AbstractAssociation m2 = new Association("土衛2號辦事處");
		m2.add(new HumanResouce("土衛2號辦事處-人力資源單位"));
		saturn.add(m2);
		
		System.out.println("結構圖:");
		root.display(1);
		System.out.println("職責表");
		root.lineOfDuty();
	}
}
