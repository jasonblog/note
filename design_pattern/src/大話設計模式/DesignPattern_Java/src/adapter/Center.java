package adapter;

public class Center extends Player
{

    public Center(String name)
    {
        this.setName(name);
    }

    public void jinggong()
    {
        System.out.println(this.getName() + " jinggong");
    }

    public void fangshou()
    {
        System.out.println(this.getName() + " fangshou");
    }
}
