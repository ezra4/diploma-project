package utcn.licenta.simulator.models;

import lombok.*;

@Getter
@Setter
@AllArgsConstructor
public class Point {
    double x;
    double y;
    String deviceType;

    public void setLocation(double x, double y)
    {
        this.x = x;
        this.y = y;
    }

    public void setLocation(String x, String y)
    {
        this.x = Double.parseDouble(x);
        this.y = Double.parseDouble(y);
    }

    public void setLocation(double x, double y, double z)
    {
        this.x = x;
        this.y = y;
    }
    public String toString() {
        return "Point("+x+","+y+")";
    }
}
