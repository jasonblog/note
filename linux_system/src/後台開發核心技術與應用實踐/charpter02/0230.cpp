class Animail
{
public:
    virtual void GetColor()  = 0;
};
class Dog : public Animail
{
public:
    virtual void GetColor()
    {
        cout << "Yellow"endl;
    };
};
class Pig : public Animail
{
public:
    virtual void GetColor()
    {
        cout << "White" << endl;
    };
};
int main()
{
    Animail cAnimail;
    return 0;
}
