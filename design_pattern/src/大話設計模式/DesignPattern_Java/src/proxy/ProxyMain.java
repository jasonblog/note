package proxy;

public class ProxyMain
{

    /**
     * @param args
     */
    public static void main(String[] args)
    {
        SchoolGirl mm = new SchoolGirl("jin ");
        Proxy agent = new Proxy(mm);

        agent.giveChocolate();
        agent.giveFlowers();
    }

}
