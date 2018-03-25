package c08.adapter;
/**
 * 具體的弓箭手
 */
public class NormalArcher implements Archer{
	@Override
	public void shot() {
		System.out.println("射箭");
	}
}
