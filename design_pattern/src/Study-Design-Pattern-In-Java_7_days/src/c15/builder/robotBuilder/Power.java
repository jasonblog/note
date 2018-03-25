package c15.builder.robotBuilder;

/**
 * 機器人組件-動力(Product Part)
 */
public class Power {
	private String mainPower; // 主動力
	private String subPower; // 副動力
	private String battery; // 電池

	public Power(String mainPower, String subPower, String battery) {
		this.mainPower = mainPower;
		this.subPower = subPower;
		this.battery = battery;
	}

	@Override
	public String toString() {
		return "{主動力：" + mainPower + " , 副動力:" + subPower + " ,電池：" + battery + "}";
	}
}
