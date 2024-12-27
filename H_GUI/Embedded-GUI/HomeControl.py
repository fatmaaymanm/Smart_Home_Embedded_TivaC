import os
import sys
import serial
import threading
from datetime import datetime
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QLabel, QVBoxLayout, QTableWidget, QTableWidgetItem,
    QWidget, QPushButton, QHBoxLayout, QGridLayout, QMessageBox
)
from PyQt5.QtGui import QPixmap, QIcon
from PyQt5.QtCore import Qt, QTimer

def send_command(serial_port, command):
    """Send a command to the Tiva board and print the response."""
    serial_port.write(f"{command}\n".encode())  # Send the command with newline
    response = serial_port.readline().decode('utf-8').strip()  # Read the response
    if response and command == "DOOR":
        print(f"Door State (0 if Closed, 1 if Opened): {response}")
    elif response and command == "COUNT":
        print(f"Door Open Count: {response}")
    elif response and command == "TEMP":
        print(f"Temperature: {response}")
        try:
            temperature = int(response)  # Try converting the response to an integer
            if temperature > 30:
                print("Alert! Temperature exceeds 40!")
        except ValueError:
            print("Invalid temperature data received.")
    # else:
    #     print("No response from Tiva.")
    return response


class HomeControlApp(QMainWindow):
    def __init__(self):
        super().__init__()

        # Initialize lamp and plug states
        self.lamp_on = False
        self.plug_on = False

        # GUI Setup
        self.setWindowTitle("Smart Home Control")
        self.setWindowIcon(QIcon("utils/app_icon.png"))
        self.setGeometry(100, 100, 800, 600)

        # Image file paths (assuming images are in the same folder as the script)
        self.lamp_on_image = "utils/lamp_on.jpg"
        self.lamp_off_image = "utils/lamp_off.jpg"
        self.plug_on_image = "utils/plug_on.jpg"
        self.plug_off_image = "utils/plug_off.jpg"
        self.switch_icon_image = "utils/switch_icon.png"

        # Lamp Controls
        self.lamp_label = QLabel(self)
        self.lamp_label.setPixmap(QPixmap(self.lamp_off_image).scaled(100, 100, Qt.KeepAspectRatio))
        self.lamp_label.setAlignment(Qt.AlignCenter)
        self.lamp_button = QPushButton("Toggle Lamp", self)
        self.lamp_button.setIcon(QIcon(self.switch_icon_image))
        self.lamp_button.clicked.connect(self.toggle_lamp)

        # Plug Controls
        self.plug_label = QLabel(self)
        self.plug_label.setPixmap(QPixmap(self.plug_off_image).scaled(100, 100, Qt.KeepAspectRatio))
        self.plug_label.setAlignment(Qt.AlignCenter)
        self.plug_button = QPushButton("Toggle Plug", self)
        self.plug_button.setIcon(QIcon(self.switch_icon_image))
        self.plug_button.clicked.connect(self.toggle_plug)

        # Door Status
        self.status_label = QLabel("Waiting for door status...", self)
        self.status_label.setStyleSheet("font-size: 20px; color: blue;")

        # Temperature Visualization
        self.count_label = QLabel("Door Open Count: --", self)
        self.count_label.setStyleSheet("font-size: 20px; color: blue;")

        # Temperature Visualization
        self.temp_label = QLabel("Temperature: -- °C", self)
        self.temp_label.setStyleSheet("font-size: 16px; color: green;")

        # Log Table
        self.log_table = QTableWidget(self)
        self.log_table.setColumnCount(2)
        self.log_table.setHorizontalHeaderLabels(["Timestamp", "Door Status"])
        self.log_table.setStyleSheet("font-size: 14px;")

        # Layouts

        # Controls Layout
        control_layout = QGridLayout()
        control_layout.addWidget(self.lamp_label, 0, 0, alignment=Qt.AlignCenter)
        control_layout.addWidget(self.lamp_button, 1, 0, alignment=Qt.AlignCenter)
        control_layout.addWidget(self.plug_label, 0, 1, alignment=Qt.AlignCenter)
        control_layout.addWidget(self.plug_button, 1, 1, alignment=Qt.AlignCenter)

        # Temperature Layout
        temp_layout = QVBoxLayout()
        temp_layout.addWidget(self.temp_label)

        # Main Layout
        main_layout = QVBoxLayout()
        main_layout.addLayout(control_layout)
        main_layout.addLayout(temp_layout)
        main_layout.addWidget(self.status_label)
        main_layout.addWidget(self.count_label)
        main_layout.addWidget(self.log_table)

        container = QWidget()
        container.setLayout(main_layout)
        self.setCentralWidget(container)

        # UART Setup
        self.uart_port = serial.Serial("COM3", baudrate=9600, timeout=1)  # Replace with your port
        self.start_uart_listener()

        # Timer to send data periodically
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.send_data_to_tiva)
        self.timer.start(3000)  # 1000 ms = 1 second interval

    def start_uart_listener(self):
        """Start a thread to listen for UART data."""
        self.uart_thread = threading.Thread(target=self.uart_listener, daemon=True)
        self.uart_thread.start()

    def uart_listener(self):
        """Read data from UART and update the GUI."""
        while True:
            if self.uart_port.in_waiting > 0:
                data = self.uart_port.readline().decode('utf-8').strip()
                self.handle_uart_data(data)

    def handle_uart_data(self, data):
        """Process UART data received from the TivaC."""
        if data.startswith("DOOR "):
            status = data.split(":")[1]
            self.update_door_status(status)
        elif data.startswith("TEMP:"):
            temperature = float(data.split(":")[1])
            self.update_temperature(temperature)

    def send_data_to_tiva(self):
        """Send periodic data to Tiva C board."""
        send_command(self.uart_port, "DOOR")
        send_command(self.uart_port, "COUNT")
        send_command(self.uart_port, "TEMP")

    def update_door_status(self, status):
        """Update the door status and log the event."""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        self.status_label.setText(f"Door Status: {status}")

        # Add to log table
        row_position = self.log_table.rowCount()
        self.log_table.insertRow(row_position)
        self.log_table.setItem(row_position, 0, QTableWidgetItem(timestamp))
        self.log_table.setItem(row_position, 1, QTableWidgetItem(status))

    def update_door_open_count(self, count):
        """Update the temperature display."""
        self.temp_label.setText(f"Door Open Count: {count}")

    def update_temperature(self, temp):
        """Update the temperature display."""
        self.temp_label.setText(f"Temperature: {temp:.1f} °C")

    def toggle_lamp(self):
        """Toggle lamp state and update the visual."""
        self.uart_port.write("LAMP\n".encode('utf-8'))
        if self.lamp_on:
            self.lamp_label.setPixmap(QPixmap(self.lamp_off_image).scaled(100, 100, Qt.KeepAspectRatio))
            self.lamp_on = False
        else:
            self.lamp_label.setPixmap(QPixmap(self.lamp_on_image).scaled(100, 100, Qt.KeepAspectRatio))
            self.lamp_on = True

    def toggle_plug(self):
        """Toggle plug state and update the visual."""
        self.uart_port.write("PLUG\n".encode('utf-8'))
        if self.plug_on:
            self.plug_label.setPixmap(QPixmap(self.plug_off_image).scaled(100, 100, Qt.KeepAspectRatio))
            self.plug_on = False
        else:
            self.plug_label.setPixmap(QPixmap(self.plug_on_image).scaled(100, 100, Qt.KeepAspectRatio))
            self.plug_on = True

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = HomeControlApp()
    window.show()
    sys.exit(app.exec_())
