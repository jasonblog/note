package c07.command.coffeShop;

/**
 * 點心廚師(ConcreteReceiver)
 */
public class Chef implements KitchenWorker {

	@Override
	public void finishOrder() {
		System.out.print("取出麵包->美乃滋塗上滿滿的麵包->丟進烤箱->灑上可以吃的裝飾->點心完成");
		System.out.println();
	}

}
