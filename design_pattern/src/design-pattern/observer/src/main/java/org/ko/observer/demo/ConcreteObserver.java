package org.ko.observer.demo;


/**
 * 具体的观察者对象，实现更新的方法
 * 使自身的状态和目标的状态保持一致
 * @author K.O
 *
 */
public class ConcreteObserver implements Observer{

	//观察者的名字，是谁收到了信息
	private String observerName;
	
	//天气内容的情况，这个消息从目标处获取
	private String weatherContent;
	
	//提醒的内容
	private String remindThing;
	
	/**
	 * 获取目标类的状态同步到观察者的状态中
	 */
	public void update(WeatherSubject subject) {
		ConcreteWeatherSubject concreteSubject = (ConcreteWeatherSubject)subject;
		this.weatherContent = concreteSubject.getWeatherContent();
		System.out.println(this.observerName + "收到了"+ this.weatherContent + "," + this.remindThing);
	}

	/**
	 * 推模型 按需取内容
	 * @param content
	 */
	@Override
	public void update(String content) {
		this.weatherContent = content;
		System.out.println(this.observerName + "收到了"+ this.weatherContent + "," + this.remindThing);
	}

	public String getObserverName() {
		return observerName;
	}

	public void setObserverName(String observerName) {
		this.observerName = observerName;
	}

	public String getWeatherContent() {
		return weatherContent;
	}

	public void setWeatherContent(String weatherContent) {
		this.weatherContent = weatherContent;
	}

	public String getRemindThing() {
		return remindThing;
	}

	public void setRemindThing(String remindThing) {
		this.remindThing = remindThing;
	}

	public ConcreteObserver(String observerName, String remindThing) {
		super();
		this.observerName = observerName;
		this.remindThing = remindThing;
	}

}
