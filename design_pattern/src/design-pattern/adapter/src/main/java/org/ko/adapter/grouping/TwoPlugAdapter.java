package org.ko.adapter.grouping;

/**
 * 二项插座转三项插座的适配器
 */
public class TwoPlugAdapter implements ThreePlugIf {

	private GBTowPlug plug;
	
	public TwoPlugAdapter (GBTowPlug plug) {
		this.plug = plug;
	}
	
	public void powerWithThree() {
		System.out.println("通过转化");
		plug.powerWithTwo();
	}

}
