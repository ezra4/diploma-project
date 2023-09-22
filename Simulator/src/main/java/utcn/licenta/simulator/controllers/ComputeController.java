package utcn.licenta.simulator.controllers;

import javafx.animation.*;
import javafx.application.Platform;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.TextArea;
import utcn.licenta.simulator.models.Device;
import utcn.licenta.simulator.models.Point;
import utcn.licenta.simulator.services.TextWriterService;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class ComputeController {
    @FXML
    public TextArea outputTextArea;
    @FXML
    public Button computeButton;

    MainController mainController;
    public ArrayList<Point> points;
    public String algorithm;
    ExecutorService executorService = Executors.newSingleThreadExecutor();
    private AnimationTimer animationTimer;

    private boolean staExists = false;

    private void startWaitingAnimation() {
        if(staExists)
        {
            outputTextArea.setText("""
                    No STA was added.
                    Default STA will be added instead (position [X_MAX/2 ,Y_MAX/2])
                    """);
            outputTextArea.appendText("Devices' positions:\n");
            for (Device ap : mainController.getDevicesList().getItems()
            ) {
                outputTextArea.appendText("\t"+ap + "\n");
            }
            outputTextArea.appendText("""
                Write to file is complete!
                Beginning simulation...
                """);
            staExists = false;
        }
        else
        {
            outputTextArea.setText("Devices' positions:\n");
            for (Device ap : mainController.getDevicesList().getItems()
            ) {
                outputTextArea.appendText("\t"+ap + "\n");
            }
            outputTextArea.appendText("""
                Write to file is complete!
                Beginning simulation...
                """);
        }

        animationTimer = new AnimationTimer() {
            private long startTime = System.nanoTime();
            private int dotCount = 0;
            private boolean startCond = false;

            @Override
            public void handle(long now) {
                long elapsedNanos = now - startTime;
                long elapsedMillis = elapsedNanos / 1_000_000;

                if (elapsedMillis > 6000)
                    startCond = true;

                if (elapsedMillis > 500 && startCond) {
                    String dots = ".".repeat(dotCount);
                    outputTextArea.setText("Please wait" + dots);

                    startTime = now;
                    dotCount = (dotCount + 1) % 4;
                }
            }
        };
        animationTimer.start();
    }

    public void initialize() {

    }

    public boolean verifyIfNotDouble(String text)
    {
        try{
            Double.parseDouble(text);
            return false;
        }catch(NumberFormatException e)
        {
            return true;
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

    public Point getCenter(ObservableList<Device> list)
    {
        double maxX=0;
        double maxY=0;
        for (Device ap: list
             ) {
            double currentX = ap.point.getX();
            double currentY = ap.point.getY();
            if(currentX>maxX)
                maxX = currentX;
            if(currentY>maxY)
                maxY = currentY;
        }
        return new Point(maxX/2,maxY/2,"STA");
    }
    public void setAlgorithm(String algorithm) {
        this.algorithm = algorithm;
    }

    public void setPoints(ArrayList<Point> points) {
        this.points = points;
    }

    public void setMainController(MainController mainController) {
        this.mainController = mainController;
    }

    public boolean verifyThings() {
        boolean OK = true;
        ObservableList<Device> list = mainController.devicesList.getItems();
        if (list.isEmpty()) {
            mainController.errorList.setText("There is no device defined!");
            OK = false;
        }
        else {
            Device device = getStation(list);
            Point center = getCenter(list);
            if(device == null)
            {
                //list.add(new Device("0",new Point(5,5,"STA")));
                Device station = new Device("0",center);
                list.add(station);
                staExists = true;
                mainController.points.add(station.getPoint());
                mainController.ac.setPoints(points);
                mainController.ac.draw();
            }
        }

        String algorithm = mainController.menuButton.getText();
        if (algorithm.equals("Select an algorithm")) {
            mainController.errorAlgoritm.setText("Please select an algorithm to compute!");
            mainController.menuButton.setStyle("-fx-background-color: red");
            OK=false;
        }
        return OK;
    }
    public void computeButtonAction() {
        if (!verifyThings())
            return;
        TextWriterService writer = new TextWriterService();
        writer.writeToFile(mainController.getDevicesList().getItems(), mainController.menuButton.getText().toLowerCase(), "output.txt");

        startWaitingAnimation();

        executorService.submit(() -> {

            String ns3Directory = "C:/Uni/AN4/Licenta/NS3/ns-allinone-3.37/ns-3.37";
            String wslCommand = "wsl";
            String ns3Command = "./ns3 run";
            String parameterFile = "\"scratch/alg1.cc --inputFile=/mnt/c/Uni/AN4/Licenta/Simulator/output.txt\"";
            String command = "cd " + ns3Directory + " && " + wslCommand + " " + ns3Command + " " + parameterFile;

            try {
                ProcessBuilder processBuilder = new ProcessBuilder("cmd.exe", "/c", command);
                processBuilder.directory(new File(ns3Directory));

                Process process = processBuilder.start();
                int exitCode = process.waitFor();
                StringBuilder output = new StringBuilder();
                try (BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()))) {
                    String line;
                    while ((line = reader.readLine()) != null) {
                        output.append(line).append("\n");
                    }
                    animationTimer.stop();
                }
                Platform.runLater(() -> outputTextArea.setText(output.toString()));
            } catch (InterruptedException | IOException e) {
                e.printStackTrace();
            }
        });
    }
}
