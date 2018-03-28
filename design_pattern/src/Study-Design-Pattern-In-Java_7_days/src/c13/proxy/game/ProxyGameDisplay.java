package c13.proxy.game;
/**
 * 代理類別
 */
public class ProxyGameDisplay implements GameDisplay{
	private RealGameDisplay realGameDisplay;
	
	public ProxyGameDisplay(RealGameDisplay realGameDisplay){
		this.realGameDisplay = realGameDisplay;
	}
	
	@Override
	public void display() {
		System.out.println("遊戲讀取中...");
		realGameDisplay.display();
	}
}
