package utcn.licenta.simulator;


import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.stage.Stage;
import lombok.Getter;
import lombok.Setter;
import utcn.licenta.simulator.controllers.MainController;

import java.io.IOException;

@Getter
@Setter
public class MainApplication extends Application {

    private static Stage primaryStage;
    @Override
    public void start(Stage stage) throws IOException {
        FXMLLoader fxmlLoader = new FXMLLoader(MainApplication.class.getResource("view.fxml"));

        Scene scene = new Scene(fxmlLoader.load(), 450 , 420);
        stage.setTitle("Simulation environment");
        stage.setScene(scene);
        stage.show();

        MainController controller = fxmlLoader.getController();
        controller.setPrimaryStage(stage);
        controller.getPrimaryStage().setOnCloseRequest((e) -> System.exit(0));
        controller.getPrimaryStage().setResizable(false);
    }

    public static void main(String[] args) {
        launch();
    }
}