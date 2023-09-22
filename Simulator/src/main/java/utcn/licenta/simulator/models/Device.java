package utcn.licenta.simulator.models;

import lombok.*;

@Getter
@Setter
@AllArgsConstructor
public class Device {
    public String id;
    public Point point;

    @Override
    public String toString()
    {
        if(id.equals("0"))
            return "STA"+" ( "+point.x+" , "+point.y+" )";
        return "AP"+id+" ( "+point.x+" , "+point.y+" )";
    }
}
