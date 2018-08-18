package net.macdidi.hellofriends;

public class Friend
{

    // 郵件帳號、緯度、經度、是否分享
    private String email;
    private double latitude;
    private double longitude;
    private boolean online;

    // 提供給 Firebase 使用的類別，一定要加入這個沒有參數的建構子
    public Friend() {}

    public Friend(String email, double latitude, double longitude,
                  boolean online)
    {
        this.email = email;
        this.latitude = latitude;
        this.longitude = longitude;
        this.online = online;
    }

    public String getEmail()
    {
        return email;
    }

    // 轉換郵件帳號，因為 Firebase 結點名稱不可以包含@與.
    public String emailToNode()
    {
        return email.replaceAll("@", "_").replaceAll("\\.", "*");
    }

    public static String nodeToEmail(String node)
    {
        return node.replaceAll("_", "@").replaceAll("\\*", ".");
    }

    public void setEmail(String email)
    {
        this.email = email;
    }

    public double getLongitude()
    {
        return longitude;
    }

    public void setLongitude(double longitude)
    {
        this.longitude = longitude;
    }

    public double getLatitude()
    {
        return latitude;
    }

    public void setLatitude(double latitude)
    {
        this.latitude = latitude;
    }

    public boolean isOnline()
    {
        return online;
    }

    public void setOnline(boolean online)
    {
        this.online = online;
    }

    @Override
    public boolean equals(Object o)
    {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        Friend friend = (Friend) o;
        return email.equals(friend.email);
    }

    @Override
    public int hashCode()
    {
        return email.hashCode();
    }

    @Override
    public String toString()
    {
        return "Friend{" +
               "email='" + email + '\'' +
               ", longitude=" + longitude +
               ", latitude=" + latitude +
               ", online=" + online +
               '}';
    }

}
