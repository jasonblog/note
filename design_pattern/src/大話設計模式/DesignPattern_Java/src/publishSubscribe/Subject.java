package publishSubscribe;

import java.util.ArrayList;
import java.util.List;

public class Subject
{

    private List<Observer> observerList = new ArrayList<Observer>();

    public void attach(Observer ob)
    {
        this.observerList.add(ob);
    }

    public void detach(Observer ob)
    {
        this.observerList.remove(ob);
    }

    public void notifyObservers()
    {
        for (Observer o : observerList) {
            o.update();
        }
    }
}
