package strategy;

public class CashRebate extends CashSuper
{
    private double rebate;

    public CashRebate(double rebate)
    {
        this.rebate = rebate;
    }

    public double getRebate()
    {
        return rebate;
    }

    public void setRebate(double rebate)
    {
        this.rebate = rebate;
    }

    @Override
    public double acceptCash(double money)
    {
        return money * rebate / 10;
    }

}
