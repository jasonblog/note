package prototype;

public class Resume implements Cloneable
{
    private String name;
    private String sex;
    private String age;

    public Resume(String name)
    {
        this.name = name;
    }

    public String getName()
    {
        return name;
    }

    public void setName(String name)
    {
        this.name = name;
    }

    public String getSex()
    {
        return sex;
    }

    public void setSex(String sex)
    {
        this.sex = sex;
    }

    public String getAge()
    {
        return age;
    }

    public void setAge(String age)
    {
        this.age = age;
    }

    public String toString()
    {
        return name + " " + sex + " " + age;
    }

    public Resume clone()
    {
        try {
            return (Resume)super.clone();
        } catch (CloneNotSupportedException e) {
            System.out.println("Clone has error");
            e.printStackTrace();
        }

        return null;
    }
}
