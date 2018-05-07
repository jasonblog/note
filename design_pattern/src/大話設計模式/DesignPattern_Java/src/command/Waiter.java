package command;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

public class Waiter
{

    public List<Command> orders = new ArrayList<Command>();

    public void setOrder(Command com)
    {
        orders.add(com);
        System.out.println("add an order " + new Date());
    }

    public void cancelOrder(Command com)
    {
        orders.remove(com);
        System.out.println("Cancel the order " + new Date());
    }
    public void excute()
    {
        for (Command c : orders) {
            c.excuteCommand();
        }
    }
}
