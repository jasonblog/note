package memento;

public class Memento
{

    private int vitality;
    private int attrack;
    private int defence;

    public int getVitality()
    {
        return vitality;
    }

    public int getAttrack()
    {
        return attrack;
    }

    public int getDefence()
    {
        return defence;
    }

    public Memento()
    {
    }

    public Memento(int vit, int attrack, int defence)
    {
        this.vitality = vit;
        this.attrack = attrack;
        this.defence = defence;
    }
}
