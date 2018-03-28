package c13.proxy.aop;

import java.util.Date;

/**
 * 戰鬥管理類別(代理)
 */
public class ProxyFightManager extends FightManager{
	private FightManager source;
	public ProxyFightManager(FightManager source){
		this.source = source;
	}
	@SuppressWarnings("deprecation")
	public void doFight(String userName){
		//這段完全就只是記錄用，與戰鬥過程沒關係
		System.out.println(">開始時間:"  +  new Date().toLocaleString()); 		
		source.doFight(userName);
	}	
}
