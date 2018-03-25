package org.ko.observer.base;

public class ConcreteWeatherSubject extends WeatherSubject {

    //晴天 下雨 下雪

    //目标对象的状态
    private String weatherContent;

    @Override
    protected void notifyObservers() {
        //循环所有注册的观察者
        for (Observer obServer: observers) {
            //规则是女友有下雨通知

            //老妈 下雨下雪通知

            //如果天气是晴天
            //do nothing

            //如果天气是下雨
            if ("下雨".equals(this.getWeatherContent())) {
                if ("女朋友".equals(obServer.getObserverName())) {
                    obServer.update(this);
                }
                if ("老妈".equals(obServer.getObserverName())) {
                    obServer.update(this);
                }
            }

            if ("下雪".equals(this.getWeatherContent())) {
                if ("老妈".equals(obServer.getObserverName())) {
                    obServer.update(this);
                }
            }

        }

    }

    public String getWeatherContent() {
        return weatherContent;
    }

    public void setWeatherContent(String weatherContent) {
        this.weatherContent = weatherContent;
        this.notifyObservers();
    }
}
