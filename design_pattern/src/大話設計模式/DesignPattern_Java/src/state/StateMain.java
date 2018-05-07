package state;

public class StateMain
{

    /**
     * @param args
     */
    public static void main(String[] args)
    {
        Work today = new Work();
        today.setHour(10);
        today.writeprogram();

        today.setHour(12);
        today.writeprogram();

        today.setHour(13);
        today.writeprogram();

        today.setHour(16);
        today.writeprogram();

        today.setHour(20);
        today.setFinish(true);
        today.writeprogram();


        today.setFinish(false);
        today.setHour(20);
        today.writeprogram();

        today.setHour(22);
        today.writeprogram();

    }

}
