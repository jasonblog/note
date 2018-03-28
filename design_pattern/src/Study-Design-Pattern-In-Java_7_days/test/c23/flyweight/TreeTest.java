package c23.flyweight;

import org.junit.Test;

/**
 * 蠅量級模式 - 測試
 */
public class TreeTest {
	@Test
	public void test(){
		System.out.println("============蠅量級模式測試============");

		Tree rose = TreeManager.getTree("玫瑰");
		rose.setOwner("Rose");
		rose.display();
		System.out.println("Jacky來買一顆玫瑰花");
		Tree jRose = TreeManager.getTree("玫瑰");
		jRose.setOwner("Jacky");
		System.out.println("##Jacky看玫瑰的時候，其實我們沒有創一棵的給他，而是拿Rose那顆換個名牌");
		jRose.display();
		
		System.out.println();
		Tree hinoki = TreeManager.getTree("台灣紅檜");
		hinoki.setOwner("林務局");
		hinoki.display();
	}
}
