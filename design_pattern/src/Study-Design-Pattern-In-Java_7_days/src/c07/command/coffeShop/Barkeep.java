package c07.command.coffeShop;
/**
 * 搖飲料小弟(ConcreteReceiver)
 */
public class Barkeep implements KitchenWorker{

	@Override
	public void finishOrder() {
		System.out.print("拿出杯子->加滿冰塊->把飲料倒進杯子->飲料完成");
		System.out.println();
	}

}
