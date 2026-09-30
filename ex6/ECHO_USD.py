=====================Echo UDS Server rogram in Hackerrank======================

import os
import socket
import threading

# HackerRank specific path configuration for the UDS Server socket
SERVER_SOCKET_PATH = "./socket"
BUF_SIZE = 4096
BACKLOG = 10

def process_client_connection(connection):
    """
    Handles communication for an individual connected client in its own thread.
    """
    try:
        while True:
            # Receive text data from the client
            data = connection.recv(BUF_SIZE)
            if not data:
                break

            # Decode binary payload to string
            message = data.decode('utf-8')

            # Echo the received data directly back to the client
            connection.sendall(data)

            # Protocol specification: "END" marks the end of communication
            if message.strip() == "END":
                break
    except Exception as e:
        pass
    finally:
        # Gracefully disconnect the client
        connection.close()

def main():
    # Clean up the socket file if it already exists from a previous run
    if os.path.exists(SERVER_SOCKET_PATH):
        os.remove(SERVER_SOCKET_PATH)

    # Initialize a Unix Domain Socket (AF_UNIX) using stream-oriented protocol (SOCK_STREAM)
    server_sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

    try:
        # Bind and start listening for inbound client configurations
        server_sock.bind(SERVER_SOCKET_PATH)
        server_sock.listen(BACKLOG)

        while True:
            # Accept a connection from a client
            connection, client_address = server_sock.accept()

            # Multi-threaded requirement: Spin up a parallel thread for each client
            client_thread = threading.Thread(
                target=process_client_connection,
                args=(connection,)
            )
            client_thread.daemon = True
            client_thread.start()

    except KeyboardInterrupt:
        pass
    finally:
        server_sock.close()
        if os.path.exists(SERVER_SOCKET_PATH):
            os.remove(SERVER_SOCKET_PATH)

if __name__ == "__main__":
      main()
