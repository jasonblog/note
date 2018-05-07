package state;

public class SleepState extends State
{

    @Override
    public void writeProgram(Work w)
    {
        System.out.println("the current time:" + w.getHour() + "Sleeping!");
    }

}
