package mediator;

public class UnionNationsSecurityCouncil extends UnionNations
{

    private USA usa;
    private Iraq iraq;

    public USA getUsa()
    {
        return usa;
    }

    public void setUsa(USA usa)
    {
        this.usa = usa;
    }

    public Iraq getIraq()
    {
        return iraq;
    }

    public void setIraq(Iraq iraq)
    {
        this.iraq = iraq;
    }

    @Override
    public void declare(String message, Country country)
    {
        if (country instanceof USA) {
            iraq.getMessage(message);
        } else {
            usa.getMessage(message);
        }
    }

}
