import sys
import socket
import os
from PySide6.QtWidgets import QApplication, QMainWindow, QPushButton, QVBoxLayout, QWidget, QLabel
from PySide6.QtCore import QThread, Signal

class BridgeThread(QThread):
    data_received = Signal(str)

    def run(self):
        socket_path = "/tmp/blackbox.sock"
        if os.path.exists(socket_path): os.remove(socket_path)
        
        server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        server.bind(socket_path)
        os.chmod(socket_path, 0o666)
        server.listen(1)

        while True:
            conn, _ = server.accept()
            data = conn.recv(1024).decode()
            if data: self.data_received.emit(data)
            conn.close()

class BlackBoxUI(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Project BlackBox")
        self.setFixedSize(800, 480) # Equinox Dash Size

        self.label = QLabel("Waiting for Ignition...", self)
        self.stay_on_btn = QPushButton("STAY ON", self)
        self.stay_on_btn.hide()

        layout = QVBoxLayout()
        layout.addWidget(self.label)
        layout.addWidget(self.stay_on_btn)
        
        container = QWidget()
        container.setLayout(layout)
        self.setCentralWidget(container)

        # Start the Bridge
        self.thread = BridgeThread()
        self.thread.data_received.connect(self.update_ui)
        self.thread.start()

    def update_ui(self, message):
        if "VOLT" in message:
            voltage = float(message.split(":")[1])
            self.label.setText(f"Battery: {voltage:.2f}V")
            if voltage < 13.0: # Ignition is off
                self.stay_on_btn.show()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = BlackBoxUI()
    window.show()
    sys.exit(app.exec())
