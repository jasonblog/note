package publishSubscribe;

public class BossSubject extends Subject
{
    private String subjectState;

    public String getSubjectState()
    {
        return subjectState;
    }

    public void setSubjectState(String subjectState)
    {
        this.subjectState = subjectState;
    }
}
