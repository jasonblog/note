package state;

public class AfternoonState extends State
{

    @Override
    public void writeProgram(Work w)
    {
        if (w.getHour() < 17) {
            System.out.println("the current time:" + w.getHour() + "fell better now!");
        } else {
            w.setState(new EveningState());
            w.writeprogram();
        }
    }

}
