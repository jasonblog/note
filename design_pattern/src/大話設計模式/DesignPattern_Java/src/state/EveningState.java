package state;

public class EveningState extends State
{

    @Override
    public void writeProgram(Work w)
    {
        if (w.isFinish()) {
            w.setState(new RestState());
            w.writeprogram();
        } else {
            if (w.getHour() < 21) {
                System.out.println("the current time:" + w.getHour() +
                                   " work overtime!how sad!");
            } else {
                w.setState(new SleepState());
                w.writeprogram();
            }
        }
    }

}
