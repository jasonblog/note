package publishSubscribe;

public class LincolnObserver extends Observer
{
    private String name;
    private String status;
    private ScretarySubject sub;

    public LincolnObserver(String name, ScretarySubject sub)
    {
        this.name = name;
        this.sub = sub;
    }

    public String getName()
    {
        return name;
    }

    public void setName(String name)
    {
        this.name = name;
    }

    public String getStatus()
    {
        return status;
    }

    public void setStatus(String status)
    {
        this.status = status;
    }

    public ScretarySubject getSub()
    {
        return sub;
    }

    public void setSub(ScretarySubject sub)
    {
        this.sub = sub;
    }

    @Override
    public void update()
    {
        status = sub.getSubjectState();
        System.out.println("The new status of " + this.name + " is " + this.status);
    }

}
