package org.ko.strategy.promotion;

import java.lang.reflect.InvocationTargetException;
import java.math.BigDecimal;

public class PromotionContext {

    /**
     * 策略实现类包
     */
    private static final String PACKAGE_NAME = "org.ko.strategy.promotion";

    /**
     * 组合策略类
     */
    private IPromotionStrategy promotionStrategy;

    /**
     * 获取促销后价钱
     * @return
     */
    public BigDecimal getPrice () {
        return this.promotionStrategy.promotionAlgorithm();
    }

    /**
     * 创建促销手段
     * @param code 对应促销模式编码
     * @param args 对应促销参数
     */
    public void newPromotion (Integer code, Object... args) {
        //根据促销模式编码获取促销模式对应类名称
        String clazz = PromotionType.findClazz(code);
        try {
            //通过反射获取促销模式的对象
            this.promotionStrategy = (IPromotionStrategy)Class.forName(PACKAGE_NAME + "." + clazz)
                    .getDeclaredConstructor(getClasses(args)).newInstance(args);
        } catch (InstantiationException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }
    }

    /**
     * 设置打折前的价格
     * @param price
     */
    public void setPrice (BigDecimal price) {
        this.promotionStrategy.setPrice(price);
    }

    /**
     * 获取Class
     * @param args
     * @return
     */
    private Class[] getClasses (Object... args) {
        Class[] classes = new Class[args.length];
        for (int i = 0; i < args.length; i ++) {
            classes[i] = args[i].getClass();
        }
        return classes;
    }

}
