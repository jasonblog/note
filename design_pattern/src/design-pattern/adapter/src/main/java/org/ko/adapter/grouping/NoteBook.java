package org.ko.adapter.grouping;

public class NoteBook {
	
	private ThreePlugIf plug;
	
	public NoteBook (ThreePlugIf plug) {
		this.plug = plug;
	}
	
	/**
	 * 使用插座充电
	 */
	public void charge () {
		plug.powerWithThree();
	}

}
