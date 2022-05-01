import serial
import time
import sqlite3

# Get input from serial monitor
arduino = serial.Serial(port='COM9', baudrate=9600, timeout=.1)

# connect to database
conn = sqlite3.connect('vehicle.db')

# Create cursor
cur = conn.cursor()

cur.execute("SELECT * FROM toll")
items = cur.fetchall()
print(items)

for item in items:
    print(item[0])

while True:
    if arduino.in_waiting:
        arduino_data = arduino.readline()
        decoded_data = arduino_data.decode().strip()  # decode byte string into Unicode

        rfid_card = decoded_data  # remove \n and \r
        print(rfid_card)  # printing the value

        cur.execute("SELECT * FROM toll WHERE vehicle_id = ?", (rfid_card,))
        balance = cur.fetchall()
        print(balance)

        if len(balance) == 0:
            arduino.write("U".encode("utf-8"))
            time.sleep(2)

        else:
            arduino.write("R".encode("utf-8"))
            time.sleep(2)

            i = 0
            while i == 0:
                if arduino.in_waiting:
                    ard = arduino.readline()
                    dec = ard.decode().strip()

                    print(dec)

                    if dec == "bal":
                        bal = balance[0][2]
                        break

            arduino.write(str(bal).encode("utf-8"))
            time.sleep(1)

            while i == 0:
                if bal < 50:
                    break
                if arduino.in_waiting:
                    new_bal = arduino.readline()
                    acc = new_bal.decode().strip()

                    print(acc)

                    cur.execute("UPDATE toll SET acc_balance = ? WHERE vehicle_id = ?", (acc, rfid_card))

                    conn.commit()

                    break
