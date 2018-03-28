package org.ko.cor.handler;

public class PriceHandlerFactory {

    /**
     * 获取PriceHandler 对象
     * @return PriceHandler
     */
    public static PriceHandler createPriceHandler() {

        PriceHandler sales = new Sales();
        PriceHandler lead = new Lead();
        PriceHandler manager = new Manager();
        PriceHandler director = new Director();
        PriceHandler vp = new VicePresident();
        PriceHandler ceo = new CEO();

        sales.setSuccessor(lead);
        lead.setSuccessor(manager);
        manager.setSuccessor(director);
        director.setSuccessor(vp);
        vp.setSuccessor(ceo);
        return sales;
    }
}
