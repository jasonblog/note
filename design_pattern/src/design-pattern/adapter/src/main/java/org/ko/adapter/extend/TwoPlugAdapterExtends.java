package org.ko.adapter.extend;

import org.ko.designPattern.adapter.grouping.GBTowPlug;
import org.ko.designPattern.adapter.grouping.ThreePlugIf;

/**
 * 采用继承方式的插座适配器
 * @author Administrator
 *
 */
public class TwoPlugAdapterExtends extends GBTowPlug implements ThreePlugIf {

	public void powerWithThree() {
		System.out.println("借助继承适配器");
		this.powerWithTwo();
	}
}
