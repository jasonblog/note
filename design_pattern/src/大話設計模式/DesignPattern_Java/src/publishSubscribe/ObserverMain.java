package publishSubscribe;

public class ObserverMain
{

    /**
     * @param args
     */
    public static void main(String[] args)
    {
        ScretarySubject scretary = new ScretarySubject();
        scretary.setSubjectState("the boss is come back,work hardly!");
        LincolnObserver lin = new LincolnObserver("Lincoln", scretary);

        scretary.attach(lin);
        scretary.notifyObservers();
    }

}
