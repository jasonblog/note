package c13.proxy.aop;

/**
 * 戰鬥管理類別
 */
public class FightManager {
	public void doFight(String userName){
		//  可以直接插入這行記錄時間用的程式碼，不過這樣就會汙染了本來只負責戰鬥管理的doFight方法
		// 	system.out.println("開始時間:"  +  new Date().toLocaleString());
		
		System.out.println(userName + "帶領冒險者們與無辜的怪物戰鬥");
		System.out.println("....以下省略戰鬥過程");
		System.out.println(userName + "帶領冒險者們洗劫了怪物的家，結束一場慘無妖道的屠殺");
	}	
	
	public void doEscape(){		
		System.out.println("打不贏，冒險者決定撤退");
		System.out.println("....丟骰子決定是否能撤退成功");
		System.out.println("打不贏，大家塊陶阿!!");
	}	
}
