package c20.prototype;

import org.junit.Test;

/**
 * 原型模式-測式
 */
public class ResumeTest {
	@Test
	public void test() throws CloneNotSupportedException {
		System.out.println("============原型模式測試============");
		Resume resume = new Resume("傑克","見習道士",1);
		resume.setExperience("2011/01/01", "仙靈島");
		
		// 履歷表2跟1有許多相似的地方，因此直接複製履歷表1做修改
		Resume resume2  = (Resume) resume.clone();
		resume2.setLevel(5);
		resume2.setExperience("2012/03/31", "隱龍窟");
		
		// 履歷表3跟1有許多相似的地方，因此直接複製履歷表1做修改
		Resume resume3  = (Resume) resume2.clone();
		resume3.setProfession("殭屍道長");
		resume3.setExperience("2012/11/31", "赤鬼王血池");
		
		System.out.println("---第一份履歷---");
		resume.display();
		System.out.println("---第二份履歷(複製上一份修改)---");
		resume2.display();
		System.out.println("---第三份履歷(複製第一份修改)---");	
		resume3.display();
	}
}
