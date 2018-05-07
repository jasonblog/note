package state;

public class Work
{
    private int hour;
    private boolean isFinish;
    private State state;

    public Work()
    {
        this.state = new BeforeNoonState();
    }
    public int getHour()
    {
        return hour;
    }

    public void setHour(int hour)
    {
        this.hour = hour;
    }

    public boolean isFinish()
    {
        return isFinish;
    }

    public State getState()
    {
        return state;
    }

    public void setState(State state)
    {
        this.state = state;
    }

    public void setFinish(boolean isFinish)
    {
        this.isFinish = isFinish;
    }

    public void writeprogram()
    {
        this.state.writeProgram(this);
    }
}
