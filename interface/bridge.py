import socket
import os

# The path to our "virtual" communication file
SOCKET_PATH = "/tmp/blackbox.sock"

def start_bridge(ui_callback):
    # Ensure a clean start
    if os.path.exists(SOCKET_PATH):
        os.remove(SOCKET_PATH)

    # Create a Unix Domain Socket
    server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    server.bind(SOCKET_PATH)
    server.listen(1)
    
    # Give the file permissions so the C++ engine can write to it
    os.chmod(SOCKET_PATH, 0o666)

    print("Bridge is active. Waiting for C++ engine...")
    
    while True:
        conn, _ = server.accept()
        try:
            while True:
                data = conn.recv(1024)
                if not data: break
                
                # Send the data (voltage/temp) to your UI
                message = data.decode().strip()
                ui_callback(message) 
        finally:
            conn.close()
