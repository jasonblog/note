package c24.visitor;

import org.junit.Test;

/**
 * 拜訪者模式-測試
 */
public class ChefTest {
	@Test
	public void test(){
		System.out.println("============拜訪者模式測試============");

		// 準備參賽的廚師們
		ChefGroup chefGropu = new ChefGroup();
		chefGropu.join(new SuperChef("小當家"));
		chefGropu.join(new DarkChef("紹安"));
		chefGropu.join(new SuperNoodleChef("解師傅"));
		
		System.out.println("------------第一回合:燒賣--------------");
		Visitor round1 = new Visitor_saoMai();
		chefGropu.topic(round1);
		
		System.out.println("------------第二回合:豆腐--------------");
		Visitor round2 = new Visitor_tofu();
		chefGropu.topic(round2);
		
		// 假如有第三回合，我們只需要增加Visitor的實做類別，不會影響到其他程式
		// 假如要新增參賽者，那就...很麻煩了
	}
}
