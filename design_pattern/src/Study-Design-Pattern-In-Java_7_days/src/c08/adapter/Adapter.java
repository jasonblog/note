package c08.adapter;
/**
 * 轉接器(Adapter)
 * 將弓箭手當作法師來用
 */
public class Adapter implements Wizard {
	private Archer archer;
	
	public Adapter(Archer archer){
		this.archer = archer;
	}
	
	@Override
	public void fireBall() {
		System.out.print("在弓箭上包一層布 -> 淋上花生油 -> 點火");
		archer.shot();
		System.out.println("火球飛出去了");
	}
}
