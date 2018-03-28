package org.ko.observer.demo;

import java.util.ArrayList;
import java.util.List;


/**
 * 目标对象，它知道观察它的观察者
 * 并提供注册（添加）和删除观察者的接口
 * @author Administrator
 *
 */
public class WeatherSubject {

	// 用来保存注册的观察者对象
	private List<Observer> observers = new ArrayList<Observer>();
	
	
	/**
	 * 把订阅天气的人添加到订阅列表中
	 * @param observer
	 * @return
	 */
	public boolean attach (Observer observer) {
		return observers.add(observer);
	}
	
	/**
	 * 删除集合中的指定观察者
	 * @param observer
	 * @return
	 */
	public boolean detach (Observer observer) {
		return observers.remove(observer);
	}
	
	/**
	 * 通知所有已经订阅了天气的人
	 */
	protected void notifyObservers (String content) {
		for (Observer observer : observers) {
			observer.update(this);
			observer.update(content);
		}
	}
}
