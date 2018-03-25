package c08.adapter;

import org.junit.Test;

import c08.adapter.Adapter;
import c08.adapter.NormalArcher;
import c08.adapter.Wizard;

/**
 * 轉接器模式測試-測試(Client)
 */
public class AdapterClient {
	@Test	public void test(){
		System.out.println("============轉接器模式測試============");
		
		System.out.println("我們需要火球才能把樹上的蜂窩砸爛，糟糕的是隊伍中沒有法師");
		System.out.println("幸好隊伍中有一個弓箭手跟馬蓋先工具包，讓弓箭手也能發火球：");
		Wizard wizard = new Adapter(new NormalArcher());
		wizard.fireBall();
	}
}
