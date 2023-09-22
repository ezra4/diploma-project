import numpy as np
import math
import sys

def calculate_distance(point1, point2):
    return math.sqrt((point1[0]-point2[0])**2 + (point1[1]-point2[1])**2)

def RSSI(APs, device):
    estimated_positions = []
    for AP in APs:
        distance = calculate_distance(AP, device)
        estimated_positions.append([AP[0] - distance, AP[1] - distance])
    return np.mean(estimated_positions, axis=0)

def ToF(APs, device):
    estimated_positions = []
    for AP in APs:
        distance = calculate_distance(AP, device)
        estimated_positions.append([AP[0] + distance, AP[1] + distance])
    return np.mean(estimated_positions, axis=0)

def TDoA(APs, device):
    n = len(APs)
    A = np.zeros((n-1, 2))
    b = np.zeros((n-1, 1))
    for i in range(1, n):
        A[i-1, :] = 2 * (APs[i] - APs[0])
        b[i-1] = (calculate_distance(device, APs[i])**2 - calculate_distance(device, APs[0])**2)
    return np.linalg.lstsq(A, b, rcond=None)[0]

def main():
    # Test with an example
    width = 100  # The width of the area
    length = 100  # The length of the area
    APs = np.array([[20, 20], [80, 20], [50, 80]])  # Locations of the access points
    device = np.array([50, 50])  # Location of the device
    algorithms = {"RSSI": RSSI, "ToF": ToF, "TDoA": TDoA}

    algorithm_name = sys.argv[1]  # get the algorithm name from command line argument
    if algorithm_name not in algorithms:
        print("Invalid algorithm name")
        return

    # Test the algorithm
    algorithm = algorithms[algorithm_name]
    estimated_position = algorithm(APs, device)
    print(f"Estimated position: {estimated_position}")

    # Calculate the accuracy and precision
    errors = [calculate_distance(AP, estimated_position) - calculate_distance(AP, device) for AP in APs]
    accuracy = np.mean(errors)
    precision = np.std(errors)
    print(f"Accuracy: {accuracy}")
    print(f"Precision: {precision}")

if __name__ == "__main__":
    main()
