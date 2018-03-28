package org.ko.adapter;

import org.ko.designPattern.adapter.extend.TwoPlugAdapterExtends;
import org.ko.designPattern.adapter.grouping.GBTowPlug;
import org.ko.designPattern.adapter.grouping.NoteBook;
import org.ko.designPattern.adapter.grouping.ThreePlugIf;
import org.ko.designPattern.adapter.grouping.TwoPlugAdapter;

public class AdapterTest {

	public static void main(String[] args) {
		GBTowPlug two = new GBTowPlug();
		ThreePlugIf three = new TwoPlugAdapter(two);
		
		NoteBook book = new NoteBook(three);
		book.charge();
		
		three = new TwoPlugAdapterExtends();
		book = new NoteBook(three);
		book.charge();
	}
}
