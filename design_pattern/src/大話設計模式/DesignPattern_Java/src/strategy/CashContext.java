package strategy;

public class CashContext
{
    private CashSuper cs;

    public CashContext(String type)
    {
        if (type.equals("normal")) {
            this.cs = new CashNormal();
        } else if (type.equals("debate")) {
            this.cs = new CashRebate(8.0);
        } else {
            this.cs = new CashReturn(300.0, 100.0);
        }
    }

    public CashSuper getCs()
    {
        return cs;
    }

    public void setCs(CashSuper cs)
    {
        this.cs = cs;
    }

    public double getCash(double money)
    {
        return this.cs.acceptCash(money);
    }
}
