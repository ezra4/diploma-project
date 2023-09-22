package utcn.licenta.simulator.services;

import javafx.collections.ObservableList;
import utcn.licenta.simulator.models.Device;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

public class TextWriterService {

    public void writeToFile(ObservableList<Device> list, String algorithm, String filename)
    {
        try{
            File file = new File(filename);
            if(!file.exists()){
                file.createNewFile();
            }

            FileWriter fw = new FileWriter(file.getAbsoluteFile());
            BufferedWriter bw = new BufferedWriter(fw);

            bw.write(algorithm);
            bw.newLine();

            Device station = getStation(list);
            bw.write(station.point.getX()+ " " +station.point.getY());
            bw.newLine();

            bw.write(String.valueOf(list.size()-1));
            bw.newLine();

            int ct = 0;
            for (Device ap: list
                 ) {
                if(!ap.equals(station))
                {
                    bw.write(ap.point.getX() + " " + ap.point.getY());
                    ct++;
                    if(ct != list.size()-1)
                        bw.newLine();
                }
            }

            bw.close();
            System.out.println("Write to file complete");
        }catch(IOException e)
        {
            e.printStackTrace();
        }
    }
    public Device getStation(ObservableList<Device> list)
    {
        for (Device ap : list
             ) {
            if(ap.point.getDeviceType().equals("STA"))
                return ap;
        }
        return null;
    }
}
