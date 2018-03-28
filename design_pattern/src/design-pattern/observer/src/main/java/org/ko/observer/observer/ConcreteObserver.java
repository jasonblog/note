package org.ko.observer.observer;

import org.ko.designPattern.observer.subject.ConcreteSubject;
import org.ko.designPattern.observer.subject.Subject;

/**
 * 具体的观察者对象，实现更新的方法
 * 使自身的状态和目标的状态保持一致
 * @author K.O
 *
 */
public class ConcreteObserver implements Observer{

	/**
	 * 观察者的状态
	 */
	private String observerState;
	
	/**
	 * 获取目标类的状态同步到观察者的状态中
	 */
	public void update(Subject subject) {
		ConcreteSubject concreteSubject = (ConcreteSubject)subject;
		this.observerState = concreteSubject.getSubjectState();
		System.out.println(concreteSubject.getSubjectState());
	}

	public String getObserverState() {
		return observerState;
	}

	public void setObserverState(String observerState) {
		this.observerState = observerState;
	}

}
