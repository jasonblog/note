package org.ko.observer.demo;

/**
 * 这是一个观察者接口
 * 定义一个更新的接口给那些在目标发生改变的时候被通知的对象
 * @author Administrator
 *
 */
public interface Observer {

	/**
	 * 拉模型
	 * 更新的接口
	 * @param subject 传入目标的对象，方便获取相应的目标对象状态条
	 */
	void update(WeatherSubject subject);

	/**
	 * 推模型
	 * @param content
	 */
	void update(String content);
}
