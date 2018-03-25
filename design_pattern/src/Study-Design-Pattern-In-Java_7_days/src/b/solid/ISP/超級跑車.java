package b.solid.ISP;

public class 超級跑車  extends 阿文的車 implements 可以上路的車{
	@Override
	public void 炫富(){
		System.out.println("我是一台很酷很酷的車，我好棒");
	}
	public void 佔車位(){
		System.out.println("車庫不是買來好看的，一定要放一台車");
	}
	public void 路上跑(){
		System.out.println("我身輕如燕，跑起來像飛一樣");
	}
}
