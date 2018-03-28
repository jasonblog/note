package org.ko.observer.demo;

/**
 * 具体的目标对象
 * 负责把有关状态存入相应的观察者对象中
 * @author Administrator
 *
 */
public class ConcreteWeatherSubject extends WeatherSubject{

	/**
	 * 目标对象的状态
	 */
	private String weatherContent;

	public String getWeatherContent() {
		return weatherContent;
	}

	public void setWeatherContent(String weatherContent) {
		this.weatherContent = weatherContent;
		//内容有了 说明天气更新了，通知所有订阅的人
		this.notifyObservers(weatherContent);
	}

	
	
}
