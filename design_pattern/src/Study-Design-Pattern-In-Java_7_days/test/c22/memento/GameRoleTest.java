package c22.memento;

import org.junit.Test;

/**
 * 備忘錄模式-測試
 *
 */
public class GameRoleTest {
	@Test
	public void test() {
		System.out.println("============備忘錄模式測試============");

		
		// boss一開始的狀態
		GameRole boss = new GameRole();
		boss.stateDisplay();

		
		// 使用複雜的神秘小技巧，可以降低boss攻擊力
		System.out.println("使用複雜的神秘小技巧");
		boss.setAtk(60);
		
		// 趕快存檔
		RoleStateCareTaker rsc = new RoleStateCareTaker();
		rsc.setSave(boss.save());
		boss.stateDisplay();
		System.out.println("使用備忘錄存檔，存檔後開始戰鬥");
		System.out.println();
		// 開打了
		boss.fight();
		boss.stateDisplay();
		
		// 隊伍血沒先回滿，倒了一半，快讀取剛才的存檔
		boss.load(rsc.getSave());
		System.out.println("不行不行，那個時間點先該先回滿血，讀檔重打");
		boss.stateDisplay();
	}
	
}
