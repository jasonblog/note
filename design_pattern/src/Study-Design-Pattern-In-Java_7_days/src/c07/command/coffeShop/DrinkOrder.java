package c07.command.coffeShop;

/**
 * 飲料訂單(ConcreteCommand)
 */
public class DrinkOrder extends Order {
	public DrinkOrder(KitchenWorker receiver) {
		super(receiver);
		super.name = "drinkOrder";
	}
}
