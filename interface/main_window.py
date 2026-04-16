import sys, socket, os
from PySide6.QtWidgets import QApplication, QMainWindow, QPushButton, QVBoxLayout, QWidget, QLabel
from PySide6.QtCore import QThread, Signal

class BridgeThread(QThread):
    data_received = Signal(str)
    def run(self):
        path = "/tmp/bb_tx.sock"
        if os.path.exists(path): os.remove(path)
        s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        s.bind(path); os.chmod(path, 0o666); s.listen(1)
        while True:
            c, _ = s.accept()
            msg = c.recv(1024).decode()
            if msg: self.data_received.emit(msg)
            c.close()

class BlackBoxUI(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setFixedSize(800, 480)
        self.setStyleSheet("background-color: black; color: #00ff00;")
        
        self.label = QLabel("SYSTEM ONLINE")
        self.btn = QPushButton("CANCEL SHUTDOWN")
        self.btn.hide()
        self.btn.clicked.connect(self.cancel_shutdown)

        layout = QVBoxLayout()
        layout.addWidget(self.label); layout.addWidget(self.btn)
        c = QWidget(); c.setLayout(layout); self.setCentralWidget(c)

        self.thread = BridgeThread()
        self.thread.data_received.connect(self.handle_msg)
        self.thread.start()

    def handle_msg(self, msg):
        if "WARN" in msg:
            self.label.setText(f"IGNITION OFF - SHUTDOWN IN {msg.split(':')[1]}s")
            self.btn.show()

    def cancel_shutdown(self):
        # The Reverse Bridge: Send command to C++
        try:
            s = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
            s.sendto(b"STAY_ON", "/tmp/bb_rx.sock")
            self.btn.hide()
            self.label.setText("BATTERY MODE ACTIVE")
        except:
            self.label.setText("ERROR: ENGINE NOT RESPONDING")

if __name__ == "__main__":
    app = QApplication(sys.argv); w = BlackBoxUI(); w.show(); sys.exit(app.exec())
