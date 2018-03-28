package org.ko.observer.subject;

import org.ko.designPattern.observer.observer.Observer;

import java.util.ArrayList;
import java.util.List;

/**
 * 目标对象，它知道观察它的观察者
 * 并提供注册（添加）和删除观察者的接口
 * @author Administrator
 *
 */
public class Subject {

	// 用来保存注册的观察者对象
	private List<Observer> observers = new ArrayList<Observer>();
	
//	attach detach notifyObservers
	
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
	 * 向所有注册的观察者发送消息
	 */
	protected void notifyObservers () {
		for (Observer observer : observers) {
			observer.update(this);
		}
	}
}
