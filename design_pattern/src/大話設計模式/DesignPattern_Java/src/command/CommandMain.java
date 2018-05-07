package command;

public class CommandMain
{

    /**
     * @param args
     */
    public static void main(String[] args)
    {
        Barbecuer boy = new Barbecuer();
        Command com1 = new BakeMuttonCommand(boy);
        Command com2 = new BakeMuttonCommand(boy);
        Command com3 = new BakeChickenWingCommand(boy);

        Waiter waiter = new Waiter();
        waiter.setOrder(com1);
        waiter.setOrder(com2);
        waiter.setOrder(com3);
        waiter.excute();

        waiter.cancelOrder(com2);
        waiter.excute();
    }

}
