package c13.proxy.game;
/**
 * 被代理的類別
 */
public class RealGameDisplay implements GameDisplay{
	@Override
	public void display() {
	
		System.out.println("顯示遊戲畫面");
	}
}
