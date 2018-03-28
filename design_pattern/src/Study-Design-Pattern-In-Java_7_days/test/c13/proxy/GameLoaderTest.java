package c13.proxy;

import org.junit.Test;

import c13.proxy.game.ProxyGameDisplay;
import c13.proxy.game.RealGameDisplay;
/**
 * 代理模式(動態代理)-測試
 */
public class GameLoaderTest {
	@Test
	public void test(){
		System.out.println("============代理模式(動態代理)測試============");

		
		// 沒使用代理
		System.out.println("---沒使用代理---");
		new RealGameDisplay().display();
		System.out.println();
		// 使用代理
		System.out.println("---使用代理---");
		new ProxyGameDisplay(new RealGameDisplay()).display();
	}
}
