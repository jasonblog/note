package org.ko.observer;

import org.ko.designPattern.observer.demo.ConcreteObserver;
import org.ko.designPattern.observer.demo.ConcreteWeatherSubject;

public class ObserverTest {

	public static void main(String[] args) {
		
		//1-创建目标
		ConcreteWeatherSubject weather = new ConcreteWeatherSubject();
		
		//2-创建观察者
		ConcreteObserver observerGirl = new ConcreteObserver("女朋友", "是我们第一次约会，地点街心公园，不见不散噢");
		ConcreteObserver observerMum = new ConcreteObserver("丈母娘", "是个购物的好天气，要去天河购物");
		
		//3-注册观察者
		weather.attach(observerGirl);
		weather.attach(observerMum);
		
		//4-目标发布天气
		weather.setWeatherContent("明天天气晴朗，蓝天白云，气温28度");
		
		//PS：
		System.out.println("程序员哪来的女朋友和丈母娘，这例子搞笑- -");
	}
}
