package strategy;

public class CashNormal extends CashSuper
{

    @Override
    public double acceptCash(double money)
    {
        return money;
    }

}
