package chainOfResponsibility;

public class Request
{
    private String type;
    private String content;
    private int quanlity;

    public Request(String type)
    {
        this.type = type;
    }

    public String getType()
    {
        return type;
    }

    public void setType(String type)
    {
        this.type = type;
    }

    public String getContent()
    {
        return content;
    }

    public void setContent(String content)
    {
        this.content = content;
    }

    public int getQuanlity()
    {
        return quanlity;
    }

    public void setQuanlity(int quanlity)
    {
        this.quanlity = quanlity;
    }
}
