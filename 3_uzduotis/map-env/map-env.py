import serial
import matplotlib.pyplot as plt
import numpy as np

SERIAL_ID = '/dev/ttyACM0'  # serial arduino port
BAUD = 9600                 # baud value for serial port
WORKING_DEGREES = 180       # number of degrees that are supported
WORKING_DISTANCE = 600      # distance in mm for the ToF sensor to work on
TIMES_TO_REPEAT = 1         # times to repeat the scan in case of missed values

# data received from arduino through serial
servo_degrees = []
tof_distances = []
# data to put on the graph
x_coords = []
y_coords = []


def plot_graph():
    theta = np.linspace(0, 2 * np.pi, 100)
    r = np.sqrt(1.0)
    x1 = r * np.cos(theta)
    x2 = r * np.sin(theta)

    fig, ax = plt.subplots(1)
    plt.scatter(x_coords, y_coords)

    ax.plot(x1, x2)
    ax.set_aspect(1)

    plt.xlim(-1.25, 1.25)
    plt.ylim(-1.25, 1.25)

    plt.grid(linestyle='--')
    plt.show()


def main():
    arduino_connection = serial.Serial(SERIAL_ID, BAUD)  # connect to arduino through serial
    arduino_connection.flushInput()                      # flush junk left in the serial buffer
    iter_no = WORKING_DEGREES * TIMES_TO_REPEAT          # we'll scan the environment this many times

    while iter_no > 0:
        read_bytes = arduino_connection.readline()
        decoded_bytes = str(read_bytes[0:len(read_bytes) - 2].decode("utf-8"))

        if ' ' not in decoded_bytes:
            continue

        # servo's angle and measured distance
        value_read = decoded_bytes.split(' ')
        print(value_read)

        degrees_read = int(value_read[0])
        distance_read = int(value_read[1])

        if 0 <= degrees_read <= WORKING_DEGREES and 0 < distance_read <= WORKING_DISTANCE:
            # remove old values since we're interested in updated values
            # useful if the environment is scan in 180 degrees twice (iter_no = 360)
            for i in range(0, len(servo_degrees)):
                if servo_degrees[i] == degrees_read:
                    servo_degrees.pop(i)
                    tof_distances.pop(i)
                    x_coords.pop(i)
                    y_coords.pop(i)
                    break

            # append newly parsed value
            servo_degrees.append(degrees_read)
            tof_distances.append(distance_read)
            # calculate the position of (x, y) point on the circle of radius 1
            x_coords.append(np.cos(degrees_read * np.pi / 180) * (distance_read / WORKING_DISTANCE))
            y_coords.append(np.sin(degrees_read * np.pi / 180) * (distance_read / WORKING_DISTANCE))

        iter_no = iter_no - 1

    print("points added: ", len(x_coords))
    plot_graph()


if __name__ == '__main__':
    main()
