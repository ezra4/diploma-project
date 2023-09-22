package utcn.licenta.simulator.controllers;

import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.canvas.Canvas;
import javafx.scene.control.*;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;
import lombok.Getter;
import lombok.Setter;
import utcn.licenta.simulator.models.Device;
import utcn.licenta.simulator.models.Point;
import utcn.licenta.simulator.services.AreaCanvasService;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Objects;

@Getter
@Setter
public class MainController {

    // primary stage from MainApplication
    private Stage primaryStage;

    // parent node
    @FXML
    public HBox rootBox;
    @FXML
    public VBox panelPrincipal;

    /* Compute panel */
    ////////////////////////////////////////////////////////
    ComputeController computeController = new ComputeController();
    public VBox computePanel;
    ////////////////////////////////////////////////////////


    /* Select algorithm */
    ////////////////////////////////////////////////////////
    @FXML
    public MenuButton menuButton;
    @FXML
    public Label errorAlgoritm;

    public MenuItem algorithm1;
    public MenuItem algorithm2;
    public MenuItem algorithm3;
    ////////////////////////////////////////////////////////

    /* Access Points */
    ////////////////////////////////////////////////////////
    @FXML
    public VBox devices;
    @FXML
    public ListView<Device> devicesList;
    @FXML
    public TextField deviceID;
    @FXML
    public TextField deviceX;
    @FXML
    public TextField deviceY;
    @FXML
    public Label errorList;

    public Button addDeviceButton;
    public Button editDeviceButton;
    public Button deleteDeviceButton;
    ////////////////////////////////////////////////////////

    /* Grid Layout */
    ////////////////////////////////////////////////////////
    @FXML
    public VBox gridAndCanvas;
    @FXML
    public TextField areaSizeX;
    @FXML
    public TextField areaSizeY;
    @FXML
    public TextField areaDivisionX;
    @FXML
    public TextField areaDivisionY;
    @FXML
    public Button drawGrid;
    @FXML
    public Button clearGrid;
    @FXML
    public Label errorGrid;
    ////////////////////////////////////////////////////////

    /* Canvas utilities */
    ////////////////////////////////////////////////////////
    public Canvas canvas = new Canvas();

    ArrayList<Point> points = new ArrayList<>();
    public AreaCanvasService ac;
    ////////////////////////////////////////////////////////


    // numerical values

    double gridLayout_currentHeight;
    double canvasHeight = 300;
    double canvasWidth = 450;
    double computePanelHeight = 180;

    public MainController() {
    }

    public void initialize() {
        clearGrid.setVisible(false);

        canvas = new Canvas();
        canvas.setHeight(canvasHeight);
        canvas.setWidth(canvasWidth);
        ac = new AreaCanvasService(canvas, canvas.getGraphicsContext2D());

        menuItemsListener();
        listViewListeners();

        clearListValues();
    }

    public void clearListValues()
    {
        deviceID.setOnMouseClicked((e) -> {
            deviceID.setStyle("");
            deviceX.setStyle("");
            deviceY.setStyle("");
            errorList.setText("");
        });
        deviceX.setOnMouseClicked((e) -> {
            deviceID.setStyle("");
            deviceX.setStyle("");
            deviceY.setStyle("");
            errorList.setText("");
        });
        deviceY.setOnMouseClicked((e) -> {
            deviceID.setStyle("");
            deviceX.setStyle("");
            deviceY.setStyle("");
            errorList.setText("");
        });
    }
    public void clearError()
    {
        errorList.setText("");
        errorList.setStyle(null);
    }
    public void menuItemsListener() {
        menuButton.setOnAction(e->{
            if(!menuButton.getText().equals("Select an algorithm"))
                menuButton.setStyle(null);
        });
        algorithm1.setOnAction((e) -> {
            menuButton.setText(algorithm1.getText());
            menuButton.setStyle("");
            errorAlgoritm.setText("");
        });
        algorithm2.setOnAction((e) -> {
            menuButton.setText(algorithm2.getText());
            menuButton.setStyle("");
            errorAlgoritm.setText("");
        });
        algorithm3.setOnAction((e) -> {
            menuButton.setText(algorithm3.getText());
            menuButton.setStyle("");
            errorAlgoritm.setText("");
        });
    }

    public void listViewListeners() {
        devicesList.getSelectionModel().selectedItemProperty().addListener((observable, oldValue, newValue) -> {
            if (newValue != null) {
                deviceID.setText(newValue.id);
                deviceX.setText(String.valueOf(newValue.point.getX()));
                deviceY.setText(String.valueOf(newValue.point.getY()));
            }
        });
    }

    public boolean accessPointExists(String id) {
        for (Device ap : devicesList.getItems()) {
            if (ap.getId().equals(id)) {
                return true;
            }
        }
        return false;
    }

    protected void clearDeviceValues() {
        deviceID.setText("");
        deviceID.setStyle("");

        deviceX.setText("");
        deviceX.setStyle("");

        deviceY.setText("");
        deviceY.setStyle("");
    }

    public boolean checkDeviceValues()
    {
        boolean OK = true;
        if(deviceID.getText().isEmpty())
        {
            errorList.setText("ID is null!");
            deviceID.setStyle("-fx-background-color: #ff726f");
            OK=false;
        }

        if(deviceX.getText().isEmpty())
        {
            errorList.setText("X value is null!");
            deviceX.setStyle("-fx-background-color: #ff726f");
            OK=false;
        }

        if(deviceY.getText().isEmpty())
        {
            errorList.setText("Y value is null!");
            deviceY.setStyle("-fx-background-color: #ff726f");
            OK=false;
        }

        if(deviceID.getText().isEmpty() && deviceX.getText().isEmpty() && deviceY.getText().isEmpty())
        {
            errorList.setText("Please add values!");
            deviceID.setStyle("-fx-background-color: #ff726f");
            deviceX.setStyle("-fx-background-color: #ff726f");
            deviceY.setStyle("-fx-background-color: #ff726f");
            OK=false;
        }

        double id = -1;
        double x = -1;
        double y = -1;
        if(verifyIfNotDouble(deviceID.getText()))
        {
            errorList.setText("ID must be double!");
            deviceID.setStyle("-fx-background-color: #ff726f");
            OK=false;
        }
        else id = Double.parseDouble(deviceID.getText());
        if(verifyIfNotDouble(deviceX.getText()))
        {
            errorList.setText("X value must be double!");
            deviceX.setStyle("-fx-background-color: #ff726f");
            OK=false;
        }
        else x = Double.parseDouble(deviceX.getText());
        if(verifyIfNotDouble(deviceY.getText()))
        {
            errorList.setText("Y value must be double!");
            deviceY.setStyle("-fx-background-color: #ff726f");
            OK=false;
        }
        else y = Double.parseDouble(deviceY.getText());

        if(verifyIfNotDouble(deviceID.getText()) && verifyIfNotDouble(deviceX.getText()) && verifyIfNotDouble(deviceY.getText()))
        {
            errorList.setText("Values must be double!");
            deviceID.setStyle("-fx-background-color: #ff726f");
            deviceX.setStyle("-fx-background-color: #ff726f");
            deviceY.setStyle("-fx-background-color: #ff726f");
            OK=false;
        }
        if(x<0)
        {
            errorList.setText("X value must be positive double!");
            deviceX.setStyle("-fx-background-color: #ff726f");
            OK=false;
        }
        if(y<0)
        {
            errorList.setText("Y value must be positive double");
            deviceY.setStyle("-fx-background-color: #ff726f");
            OK=false;
        }
        if(id<0)
        {
            errorList.setText("ID must be positive integer!");
            deviceID.setStyle("-fx-background-color: #ff726f");
            OK=false;
        }
        if(x<0 && y<0 && id<0)
        {
            errorList.setText("Values must be positive values!");
            deviceID.setStyle("-fx-background-color: #ff726f");
            deviceX.setStyle("-fx-background-color: #ff726f");
            deviceY.setStyle("-fx-background-color: #ff726f");
            OK=false;
        }
        return OK;
    }
    @FXML
    protected void addDevice() {

        if(!checkDeviceValues())
            return;
        Device device = new Device
                (
                        deviceID.getText(),
                        new Point
                                (
                                        Double.parseDouble(deviceX.getText()),
                                        Double.parseDouble(deviceY.getText()),
                                        "AP"
                                )
                );
        if (accessPointExists(device.id)) {
            errorList.setVisible(true);
            deviceID.setStyle("-fx-background-color: red");
            errorList.setText("Device with id: " + device.id + " already exists!");
            return;

        } else {
            deviceID.setStyle(null);
            devicesList.getItems().add(device);
            points.add(device.getPoint());
        }

        if(device.id.equals("0"))
            device.point.setDeviceType("STA");

        clearDeviceValues();
        ac.setPoints(points);
        ac.draw();
        System.out.println("Add: " + points);
    }

    @FXML
    public void editDevice() {
        /*if(!checkDeviceValues())
            return;*/

        Device selectedDevice = devicesList.getSelectionModel().getSelectedItem();
        if(selectedDevice == null)
        {
            errorList.setText("Please select a device first!");
            return;
        }

        if (accessPointExists(deviceID.getText()) && !Objects.equals(deviceID.getText(), selectedDevice.id)) {
            errorList.setVisible(true);
            deviceID.setStyle("-fx-background-color: red");
            errorList.setText("Access Point with id: " + deviceID.getText() + " already exists!");
            return;
        } else deviceID.setStyle(null);

        int indexToEdit = points.indexOf(selectedDevice.getPoint());
        points.get(indexToEdit).setLocation(deviceX.getText(), deviceY.getText());
        System.out.println(points.get(points.indexOf(selectedDevice.getPoint())));

        selectedDevice.setId(deviceID.getText());
        selectedDevice.getPoint().setLocation(Double.parseDouble(deviceX.getText()), Double.parseDouble(deviceY.getText()));
        devicesList.refresh();

        ac.setPoints(points);
        ac.draw();
        errorList.setText("");
        System.out.println("Edit: " + points);
    }

    @FXML
    public void deleteDevice() {

        Device selectedDevice = devicesList.getSelectionModel().getSelectedItem();
        if (selectedDevice != null) {

            devicesList.getItems().remove(selectedDevice);
            points.remove(selectedDevice.getPoint());
        }
        else
        {
            errorList.setText("Please select a device first!");
            return;
        }
        ac.setPoints(points);
        ac.draw();
        errorList.setText("");
        System.out.println("Delete: " + points);
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
    public boolean verifyIfPositive(double value)
    {
        if(value<0)
            return false;
        return true;
    }
    private boolean verifyTextfield(boolean proceed, TextField tf) {
        if (tf.getText() == null || tf.getText().isEmpty() || verifyIfNotDouble(tf.getText())) {
            tf.setStyle("-fx-background-color: red");
            errorGrid.setText("Values must be positive double!");
            proceed = false;
        }
        else tf.setStyle("-fx-background-color: green");

        if(!verifyIfNotDouble(tf.getText()))
        {
            double value = Double.parseDouble(tf.getText());
            if(!verifyIfPositive(value))
            {
                proceed = false;
                errorGrid.setText("Values must be positive double");
                tf.setStyle("-fx-background-color: red");
            }
        }
        return proceed;
    }
    public boolean checkTextFieldsGrid() {
        boolean proceed = true;
        proceed = verifyTextfield(proceed, areaSizeX);
        proceed = verifyTextfield(proceed, areaSizeY);
        proceed = verifyTextfield(proceed, areaDivisionX);
        proceed = verifyTextfield(proceed, areaDivisionY);
        return proceed;
    }

    @FXML
    public void drawGridAction() {
        if (!checkTextFieldsGrid())
            return;
        errorGrid.setText("");

        areaSizeX.setStyle("-fx-background-color: green");
        areaSizeY.setStyle("-fx-background-color: green");
        areaDivisionX.setStyle("-fx-background-color: green");
        areaDivisionY.setStyle("-fx-background-color: green");

        drawGrid.setVisible(false);
        clearGrid.setVisible(true);


        primaryStage.setHeight(primaryStage.getHeight() + canvasHeight + computePanelHeight);
        gridAndCanvas.getPrefHeight();
        gridAndCanvas.getChildren().add(canvas);
        try {
            FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource("/utcn/licenta/simulator/computePanel.fxml"));
            computePanel = fxmlLoader.load();
            panelPrincipal.getChildren().add(computePanel);
            computeController = fxmlLoader.getController();
        }catch(IOException e)
        {
            e.printStackTrace();
        }

        computeController.setPoints(points);
        computeController.setAlgorithm(menuButton.getText());
        computeController.setMainController(this);

        double areaWidth = Double.parseDouble(areaSizeX.getText());
        double areaHeight = Double.parseDouble(areaSizeY.getText());
        double divisionWidth = Double.parseDouble(areaDivisionX.getText());
        double divisionHeight = Double.parseDouble(areaDivisionY.getText());

        ac.setArea(areaWidth, areaHeight);
        ac.setXDivisions(divisionWidth);
        ac.setYDivisions(divisionHeight);

        ac.draw();
    }

    public void resetGridTextfields()
    {
        areaSizeX.setStyle("");
        areaSizeX.setText("");

        areaSizeY.setStyle("");
        areaSizeY.setText("");

        areaDivisionX.setStyle("");
        areaDivisionX.setText("");

        areaDivisionY.setStyle("");
        areaDivisionY.setText("");
    }
    @FXML
    public void clearGridAction() {
        clearGrid.setVisible(false);
        drawGrid.setVisible(true);

        panelPrincipal.getChildren().remove(computePanel);
        gridAndCanvas.getChildren().remove(canvas);
        gridAndCanvas.setPrefHeight(gridAndCanvas.getHeight() - canvasHeight);
        primaryStage.setHeight(primaryStage.getHeight() - canvasHeight - computePanelHeight);

        resetGridTextfields();
    }

    public void menuSelection() {
    }
}