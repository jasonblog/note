package c13.proxy;

import org.junit.Test;

import c13.proxy.aop.FightManager;
import c13.proxy.aop.ProxyFightManager;
/**
 * 代理模式(APO)-測試
 */
public class AOPtest {
	@Test
	public void test() throws Throwable{
		System.out.println("====代理模式(AOP)測試====");

		System.out.println("---沒使用代理----");
		FightManager fm =  new FightManager();
		fm.doFight("煞氣A阿龐");
		
		System.out.println();
		
		System.out.println("---使用代理----");
		FightManager proxyFM =  new ProxyFightManager(fm);
		proxyFM.doFight("煞氣A阿龐");
	}
}
