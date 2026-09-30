[24bcs004@mepcolinux ex6]$cat server3.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 45231          // Custom port to avoid collisions
#define BUFFER_SIZE 1024

// Helper function to check if username and password match completely
int authenticate(const char *username, const char *password) {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) {
        perror("Error opening users.txt");
        return 0;
    }
    char u[50], p[50];
    while (fscanf(fp, "%s %s", u, p) != EOF) {
        if (strcmp(u, username) == 0 && strcmp(p, password) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

// Helper function to check if username exists independently
int user_exists(const char *username) {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) {
        perror("Error opening users.txt");
        return 0;
    }
    char u[50], p[50];
    while (fscanf(fp, "%s %s", u, p) != EOF) {
        if (strcmp(u, username) == 0) {
            fclose(fp);
            return 1; // Username found
        }
    }
    fclose(fp);
    return 0; // Username not found
}

// Helper function to evaluate palindromes
int is_palindrome(const char *str) {
    int l = 0;
    int h = strlen(str) - 1;
    while (h > l) {
        if (str[l++] != str[h--]) {
            return 0;
        }
    }
    return 1;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    int opt = 1;

    // 1. Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // FIX: Enable Port Re-use immediately so the port is released on exit/crashes
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 2. Bind socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Binding unsuccessful");
        exit(EXIT_FAILURE);
    }

    // 3. Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // 4. Accept connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    // --- TWO-STEP AUTHENTICATION ---
    char username[50], password[50];

    // Step A: Read username from client
    read(new_socket, username, sizeof(username));

    // Step B: Check if username exists
    if (user_exists(username)) {
        send(new_socket, "USER_OK", 7, 0);
        printf("[SERVER] Username '%s' validated. Requesting password...\n", username);
    } else {
        send(new_socket, "USER_NOT_FOUND", 14, 0);
        printf("[SERVER] Authentication rejected: Username '%s' not found.\n", username);
        close(new_socket);
        close(server_fd);
        return 0;
    }

    // Step C: Read password from client
    read(new_socket, password, sizeof(password));

    // Step D: Validate full login credentials
    if (authenticate(username, password)) {
        send(new_socket, "SUCCESS", 7, 0);
        printf("[SERVER] User '%s' completely logged in.\n", username);
    } else {
        send(new_socket, "FAIL", 4, 0);
        printf("[SERVER] Authentication rejected: Incorrect password for '%s'.\n", username);
        close(new_socket);
        close(server_fd);
        return 0;
    }

    // --- INTERACTIVE MENU LOOP ---
    char choice;
    while (1) {
        // Read 1-byte character choice
        if (read(new_socket, &choice, 1) <= 0) break;

        if (choice == '1') { // Echo Operation
            memset(buffer, 0, BUFFER_SIZE);
            read(new_socket, buffer, BUFFER_SIZE);
            printf("[SERVER] Echoing string: %s\n", buffer);
            send(new_socket, buffer, strlen(buffer) + 1, 0);

        } else if (choice == '2') { // Palindrome Operation
            memset(buffer, 0, BUFFER_SIZE);
            read(new_socket, buffer, BUFFER_SIZE);
            printf("[SERVER] Testing palindrome for: %s\n", buffer);

            if (is_palindrome(buffer)) {
                send(new_socket, "Palindrome", 11, 0);
            } else {
                send(new_socket, "Not a Palindrome", 17, 0);
            }

        } else if (choice == '3') { // Safe Termination
            printf("[SERVER] Client requested exit. Closing session.\n");
            break;
        }
    }

    close(new_socket);
    close(server_fd);
    return 0;
}

[24bcs004@mepcolinux ex6]$cat client3.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <termios.h> // Required for password masking

#define PORT 45231
#define BUFFER_SIZE 1024

// Helper function to read password with asterisk masking (****)
void get_masked_password(char *password, int max_len) {
    struct termios old_flags, new_flags;
    int i = 0;
    char ch;

    // Get current terminal settings
    tcgetattr(STDIN_FILENO, &old_flags);
    new_flags = old_flags;

    // Disable standard echoing and line-buffered input
    new_flags.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_flags);

    // Read characters one by one
    while (i < max_len - 1) {
        ch = getchar();

        // If user hits Enter
        if (ch == '\n' || ch == '\r') {
            break;
        }
        // If user hits Backspace
        else if (ch == 127 || ch == 8) {
            if (i > 0) {
                i--;
                printf("\b \b"); // Erase asterisk from screen
                fflush(stdout);
            }
        }
        // Regular typing characters
        else {
            password[i++] = ch;
            printf("*"); // Print asterisk instead of character
            fflush(stdout);
        }
    }
    password[i] = '\0'; // Null-terminate string

    // Restore original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &old_flags);
    printf("\n");
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char username[50], password[50];

    // Create Socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to Server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // --- Authentication: Step 1 (Username Verification) ---
    printf("--- Login Required ---\n");
    printf("Username: ");
    scanf("%s", username);
    getchar(); // Clear the dangling newline

    // Send username buffer to server
    send(sock, username, sizeof(username), 0);

    // Read existence validation from server
    memset(buffer, 0, BUFFER_SIZE);
    read(sock, buffer, BUFFER_SIZE);

    // If username is not available on the server, drop out immediately!
    if (strcmp(buffer, "USER_OK") != 0) {
        printf("[CLIENT] Error: Username not found. Exiting.\n");
        close(sock);
        return 0;
    }

    // --- Authentication: Step 2 (Password Entry with Masking) ---
    printf("Password: ");
    get_masked_password(password, 50);

    // Send password buffer to server
    send(sock, password, sizeof(password), 0);

    // Read final authentication confirmation
    memset(buffer, 0, BUFFER_SIZE);
    read(sock, buffer, BUFFER_SIZE);

    if (strcmp(buffer, "SUCCESS") != 0) {
        printf("[CLIENT] Access Denied. Incorrect password.\n");
        close(sock);
        return 0;
    }
    printf("[CLIENT] Authentication Successful!\n\n");

    // --- Interactive Operations Menu Loop ---
    char choice;
    while (1) {
        printf("=======================\n");
        printf("1. Echo a message\n");
        printf("2. Check a Palindrome\n");
        printf("3. Exit (Or press any other key to close)\n");
        printf("Enter your choice: ");

        char choice_str[10];
        scanf("%s", choice_str);
        choice = choice_str[0];

        send(sock, &choice, 1, 0);

        if (choice == '1') {
            char msg[BUFFER_SIZE];
            printf("Enter text to echo: ");
            scanf(" %[^\n]s", msg);

            send(sock, msg, strlen(msg) + 1, 0);

            memset(buffer, 0, BUFFER_SIZE);
            read(sock, buffer, BUFFER_SIZE);
            printf("[SERVER RESPONSE]: %s\n\n", buffer);
        }
        else if (choice == '2') {
            char msg[BUFFER_SIZE];
            printf("Enter string to test: ");
            scanf("%s", msg);

            send(sock, msg, strlen(msg) + 1, 0);

            memset(buffer, 0, BUFFER_SIZE);
            read(sock, buffer, BUFFER_SIZE);
            printf("[SERVER RESPONSE]: %s\n\n", buffer);
        }
        else {
            if (choice != '3') {
                char exit_cmd = '3';
                send(sock, &exit_cmd, 1, 0);
            }
            printf("[CLIENT] Exiting application.\n");
            break;
        }
    }

    close(sock);
    return 0;
}
[24bcs004@mepcolinux ex6]$./client
--- Login Required ---
Username: anu
Password: ****
[CLIENT] Access Denied. Incorrect password.
[24bcs004@mepcolinux ex6]$cat users.txt
aarthi pass123
anan 1234465666
amirtha 12345677
abi 0987654
anu 1029834
[24bcs004@mepcolinux ex6]$./client

Connection Failed
[24bcs004@mepcolinux ex6]$./client
--- Login Required ---
Username: aarasi
[CLIENT] Error: Username not found. Exiting.
[24bcs004@mepcolinux ex6]$./client
--- Login Required ---
Username: aarthi
Password: *******
[CLIENT] Authentication Successful!

=======================
1. Echo a message
2. Check a Palindrome
3. Exit (Or press any other key to close)
Enter your choice: 1
Enter text to echo: hello how are you
[SERVER RESPONSE]: hello how are you

=======================
1. Echo a message
2. Check a Palindrome
3. Exit (Or press any other key to close)
Enter your choice: 2
Enter string to test: mom
[SERVER RESPONSE]: Palindrome

=======================
1. Echo a message
2. Check a Palindrome
3. Exit (Or press any other key to close)
Enter your choice: 2
Enter string to test: give
[SERVER RESPONSE]: Not a Palindrome

=======================
1. Echo a message
2. Check a Palindrome
3. Exit (Or press any other key to close)
Enter your choice: 3
[CLIENT] Exiting application.

[24bcs004@mepcolinux ex6]$./server
Server listening on port 45231...
[SERVER] Username 'anu' validated. Requesting password...
[SERVER] Authentication rejected: Incorrect password for 'anu'.
[24bcs004@mepcolinux ex6]$./server
Server listening on port 45231...
[SERVER] Authentication rejected: Username 'aarasi' not found.
[24bcs004@mepcolinux ex6]$./server
Server listening on port 45231...
[SERVER] Username 'aarthi' validated. Requesting password...
[SERVER] User 'aarthi' completely logged in.
[SERVER] Echoing string: hello how are you
[SERVER] Testing palindrome for: mom
[SERVER] Testing palindrome for: give
[SERVER] Client requested exit. Closing session.

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
