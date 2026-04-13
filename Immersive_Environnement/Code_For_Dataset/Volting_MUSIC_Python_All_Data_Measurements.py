# Import required libraries
import serial              # For serial communication
import csv                 # For writing data to a CSV file
import time                # To get the timestamp (seconds since Epoch)
from datetime import datetime  # To create a timestamped filename

# Open serial port COM8 with a baud rate of 115200
ser = serial.Serial('COM8', 115200, timeout=1)

# Create CSV file path with a name based on date and time
timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
path = rf"C:\Users\userlisv\Desktop\Projet Volting Instrument\Prototype V2\Results\Data Measurements\Master\Mechanical Chair\volting_data_{timestamp}.csv"

# Open CSV file for writing
with open(path, 'w', newline='') as csvfile:
    # Define CSV headers
    fieldnames = [
        'time',
        'O_x', 'O_y', 'O_z',           # Orientation (Euler angles)
        'L_x', 'L_y', 'L_z',           # Linear acceleration
        'A_x', 'A_y', 'A_z',           # Raw acceleration
        'G_x', 'G_y', 'G_z',           # Gyroscope
        'M_x', 'M_y', 'M_z',           # Magnetometer
        'Q_w', 'Q_x', 'Q_y', 'Q_z',    # Quaternion
        'T',                           # Temperature
        'D1',                          # Distance sensor 1
        'D2'                           # Distance sensor 2
    ]
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
    writer.writeheader()  # Write header row to the file

    data = {}  # Temporary dictionary to store one cycle of measurements

    try:
        while True:  # Main reading loop
            # Read a line from the serial port
            line = ser.readline().decode('utf-8').strip()
            if not line:
                continue  # Ignore empty lines

            # Process data depending on prefix
            if line.startswith('O:'):
                parts = line[2:].strip().split(',')
                if len(parts) == 3:
                    data['O_x'], data['O_y'], data['O_z'] = [p.strip() for p in parts]

            elif line.startswith('L:'):
                parts = line[2:].strip().split(',')
                if len(parts) == 3:
                    data['L_x'], data['L_y'], data['L_z'] = [p.strip() for p in parts]

            elif line.startswith('A:'):
                parts = line[2:].strip().split(',')
                if len(parts) == 3:
                    data['A_x'], data['A_y'], data['A_z'] = [p.strip() for p in parts]

            elif line.startswith('G:'):
                parts = line[2:].strip().split(',')
                if len(parts) == 3:
                    data['G_x'], data['G_y'], data['G_z'] = [p.strip() for p in parts]

            elif line.startswith('M:'):
                parts = line[2:].strip().split(',')
                if len(parts) == 3:
                    data['M_x'], data['M_y'], data['M_z'] = [p.strip() for p in parts]

            elif line.startswith('Q:'):
                parts = line[2:].strip().split(',')
                if len(parts) == 4:
                    data['Q_w'], data['Q_x'], data['Q_y'], data['Q_z'] = [p.strip() for p in parts]

            elif line.startswith('T:'):
                temp = line[2:].strip()
                data['T'] = temp

            elif line.startswith('D1:'):
                distance1 = line[3:].strip()
                data['D1'] = distance1

            elif line.startswith('D2:'):
                distance2 = line[3:].strip()
                data['D2'] = distance2

            # When "---" is received, it indicates the end of a full cycle
            elif line == '---':
                data['time'] = time.time()  # Timestamp (seconds since Epoch)
                # If all expected data is present, write to file
                if all(key in data for key in fieldnames):
                    writer.writerow(data)   # Save to CSV file
                    csvfile.flush()         # Immediate disk write
                data = {}  # Reset dictionary for next cycle

    except KeyboardInterrupt:
        print("Stopped by user")  # End script with Ctrl+C

    finally:
        ser.close()  # Properly close the serial port