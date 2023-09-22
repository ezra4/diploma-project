module Simulator {
    requires javafx.controls;
    requires javafx.fxml;
    requires lombok;
    exports utcn.licenta.simulator to javafx.graphics;
    exports utcn.licenta.simulator.controllers to javafx.fxml;
    opens utcn.licenta.simulator.controllers to javafx.fxml;
    exports utcn.licenta.simulator.models to javafx.fxml;
    opens utcn.licenta.simulator.models to javafx.fxml;
    exports utcn.licenta.simulator.services to javafx.fxml;
    opens utcn.licenta.simulator.services to javafx.fxml;
}