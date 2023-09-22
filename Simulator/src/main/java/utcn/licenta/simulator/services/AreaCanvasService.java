package utcn.licenta.simulator.services;

import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Paint;
import lombok.Getter;
import lombok.Setter;
import utcn.licenta.simulator.models.Point;

import java.util.ArrayList;
import java.util.List;


@Getter
@Setter
public class AreaCanvasService {

    private final Canvas canvas;
    private final GraphicsContext gc;

    private final double canvasCenterXPixel;
    private final double canvasCenterYPixel;
    private final double canvasXPixelSize;
    private final double canvasYPixelSize;

    private double areaXPixelSize = -1;
    private double areaYPixelSize = -1;
    private double[] areaOutlinePolyXPoints = null;
    private double[] areaOutlinePolyYPoints = null;

    private double realXSize = -1;
    private double realYSize = -1;
    private double realXDivision = -1;
    private double realYDivision = -1;

    private List<Double> divisionXPoints = null;
    private List<Double> divisionYPoints = null;

    private List<Point> points = null;
    private final double anchorDotDiameter = 10.0;

    private double magnification = 1.0;
    private double xOffset = 0.0;
    private double yOffset = 0.0;


    public AreaCanvasService(Canvas canvas, GraphicsContext gc) {
        this.canvas = canvas;
        this.gc = gc;

        canvasCenterXPixel = canvas.getWidth() / 2;
        canvasCenterYPixel = canvas.getHeight() / 2;

        double tempWidth = canvas.getWidth() * 0.9;
        double tempHeight = canvas.getHeight() * 0.9;

        canvasXPixelSize = Math.min(tempWidth, tempHeight);
        canvasYPixelSize = Math.min(tempWidth, tempHeight);
    }

    public void setArea(double x, double y) {
        if (x <= 0 && y <= 0) {
            this.areaXPixelSize = -1;
            this.areaYPixelSize = -1;
            this.areaOutlinePolyXPoints = null;
            this.areaOutlinePolyYPoints = null;
            this.realXSize = -1;
            this.realYSize = -1;
            return;
        }
        this.realXSize = x;
        this.realYSize = y;

        if (x < y) {
            this.areaYPixelSize = canvasYPixelSize;
            this.areaXPixelSize = x / y * canvasXPixelSize;
        } else {
            this.areaXPixelSize = canvasXPixelSize;
            this.areaYPixelSize = y / x * canvasYPixelSize;
        }

        areaOutlinePolyXPoints = new double[]{-areaXPixelSize / 2 + canvasCenterXPixel,
                areaXPixelSize / 2 + canvasCenterXPixel,
                areaXPixelSize / 2 + canvasCenterXPixel,
                -areaXPixelSize / 2 + canvasCenterXPixel
        };

        areaOutlinePolyYPoints = new double[]{-areaYPixelSize / 2 + canvasCenterYPixel,
                -areaYPixelSize / 2 + canvasCenterYPixel,
                areaYPixelSize / 2 + canvasCenterYPixel,
                areaYPixelSize / 2 + canvasCenterYPixel
        };

        if (divisionYPoints != null) {
            setXDivisions(realXDivision);
        }
        if (divisionYPoints != null) {
            setYDivisions(realYDivision);
        }
    }

    public void setXDivisions(double divisionXWidth) {
        if (divisionXWidth <= 0) {
            this.realXDivision = -1.0;
            divisionXPoints = null;
            return;
        }
        this.realXDivision = divisionXWidth;
        double divisionXPixelSizeWidth = (divisionXWidth / realXSize) * areaXPixelSize;
        double startX = areaOutlinePolyXPoints[0];
        double endX = areaOutlinePolyXPoints[2];
        divisionXPoints = new ArrayList<>();
        for (double x = startX + divisionXPixelSizeWidth; x < endX; x += divisionXPixelSizeWidth) {
            divisionXPoints.add(x);
        }
    }

    public void setYDivisions(double divisionYHeight) {
        if (divisionYHeight <= 0) {
            this.realYDivision = -1.0;
            divisionYPoints = null;
            return;
        }
        this.realYDivision = divisionYHeight;
        double divisionYPixelSizeHeight = (divisionYHeight / realYSize) * areaYPixelSize;
        double startY = areaOutlinePolyYPoints[0];
        double endY = areaOutlinePolyYPoints[2];
        divisionYPoints = new ArrayList<>();
        for (double y = startY + divisionYPixelSizeHeight; y < endY; y += divisionYPixelSizeHeight) {
            divisionYPoints.add(y);
        }
    }

    public void modifyScale(double modifier) {
        if (magnification == 1.0 && modifier <= 0.0) {
            return;
        }
        double newMag = Math.max(magnification + modifier, 1.0);

        double compensating_offset = (canvasXPixelSize * (newMag - magnification)) / 2;
        xOffset -= compensating_offset;
        yOffset -= compensating_offset;

        magnification = newMag;
    }

    public void modifyOffsets(double x, double y) {
        xOffset += x;
        yOffset += y;
    }

    public void resetTransforms() {
        magnification = 1.0;
        yOffset = 0.0;
        xOffset = 0.0;
    }
    public boolean isStation(Point listPoint, Point point)
    {
        return listPoint.equals(point);
    }

    public void draw() {
        gc.setTransform(magnification, 0.0, 0.0, magnification, xOffset, yOffset);
        gc.clearRect(-canvas.getWidth(), -canvas.getHeight(), 3 * canvas.getWidth(), 3 * canvas.getHeight());

        //draw the polygon
        if (areaOutlinePolyXPoints == null || areaOutlinePolyYPoints == null) {
            return;
        }
        gc.strokePolygon(areaOutlinePolyXPoints, areaOutlinePolyYPoints, 4);
        if (divisionXPoints != null) {
            for (double divisionXPoint : divisionXPoints) {
                gc.strokeLine(divisionXPoint, areaOutlinePolyYPoints[2], divisionXPoint, areaOutlinePolyYPoints[0]);
            }
            gc.strokeText("" + realXSize + "(" + ((divisionXPoints.size()) + 1) + ")",
                    canvasCenterXPixel - 5,
                    areaYPixelSize / 2 + canvasCenterYPixel + 12);
        } else
            gc.strokeText("" + realXSize, canvasCenterXPixel - 5, areaYPixelSize / 2 + canvasCenterYPixel + 12);

        if (divisionYPoints != null) {
            for (double divisionYPoint : divisionYPoints) {
                gc.strokeLine(areaOutlinePolyXPoints[0], divisionYPoint, areaOutlinePolyXPoints[1], divisionYPoint);
            }
            gc.strokeText("" + realYSize + "(" + ((divisionYPoints.size()) + 1) + ")",
                    areaXPixelSize / 2 + canvasCenterXPixel + 4,
                    canvasCenterYPixel - 2);
        } else
            gc.strokeText("" + realYSize, areaXPixelSize / 2 + canvasCenterXPixel + 4, canvasCenterYPixel - 2);

        //drawing the points
        if (points == null || points.size() == 0) {
            return;
        }

        gc.setFill(Paint.valueOf("#FF0000"));
        for (Point p : points) {
            double x = ((p.getX() / realXSize) - 0.5) * areaXPixelSize + canvasCenterXPixel;
            double y = ((p.getY() / realYSize) - 0.5) * areaYPixelSize + canvasCenterYPixel;

            if(p.getDeviceType().equals("STA"))
                gc.setFill(Paint.valueOf("#00FF00"));
            else gc.setFill(Paint.valueOf("#FF0000"));

            gc.fillOval(x - anchorDotDiameter / 2,
                    y - anchorDotDiameter / 2,
                    anchorDotDiameter,
                    anchorDotDiameter);
        }
        gc.setFill(Paint.valueOf("#000000"));
    }
}
