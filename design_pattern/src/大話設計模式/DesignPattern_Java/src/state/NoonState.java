package state;

public class NoonState extends State
{

    @Override
    public void writeProgram(Work w)
    {
        if (w.getHour() < 13) {
            System.out.println("the current time:" + w.getHour() +
                               " fell tired ,want to sleep!");
        } else {
            w.setState(new AfternoonState());
            w.writeprogram();
        }
    }

}
