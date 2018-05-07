package chainOfResponsibility;

public class ProjectMannager extends RequestHandler
{

    @Override
    public void handleRequest(Request que)
    {
        if (que.getType().equals("leave")) {
            System.out.println("PM  agree the leave request");
        } else {
            System.out.println("PM has no right to handle it, Pls ask help from Director!");
            this.getNextHandler().handleRequest(que);
        }
    }
}
