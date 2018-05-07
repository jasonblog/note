package strategy;

public class CashReturn extends CashSuper
{
    private double achieveNum;
    private double returnNum;

    public CashReturn(double achieveNum, double returnNum)
    {
        this.achieveNum = achieveNum;
        this.returnNum = returnNum;
    }

    @Override
    public double acceptCash(double money)
    {
        if (money > achieveNum) {
            return money - Math.floor(money / this.achieveNum) * returnNum;
        }

        return money;
    }

    public double getAchieveNum()
    {
        return achieveNum;
    }

    public void setAchieveNum(double achieveNum)
    {
        this.achieveNum = achieveNum;
    }

    public double getReturnNum()
    {
        return returnNum;
    }

    public void setReturnNum(double returnNum)
    {
        this.returnNum = returnNum;
    }

}
