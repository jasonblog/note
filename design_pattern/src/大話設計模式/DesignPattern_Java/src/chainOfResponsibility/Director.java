package chainOfResponsibility;

public class Director extends RequestHandler
{

    @Override
    public void handleRequest(Request que)
    {
        if (que.getType().equals("finance")) {
            System.out.println("Director  agree the finance request");
        } else {
            System.out.println("Director has no right to handle it, Pls ask help from Manager!");
            this.getNextHandler().handleRequest(que);
        }
    }

}
