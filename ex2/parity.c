#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1000
int countTotalOnes(char *str, int len) {
    int count = 0, i, j;
    for (i = 0; i < len; i++) {
        char ch = str[i];
        for (j = 7; j >= 0; j--) {
            if ((ch >> j) & 1)
                count++;
        }
    }
    return count;
}
void printBinary(char *str, int len) {
    int i, j;
    for (i = 0; i < len; i++) {
        char ch = str[i];
        for (j = 7; j >= 0; j--) {
            printf("%d", (ch >> j) & 1);
        }
        printf(" ");
    }
}
void generateParity(char *data, char *dataWithParity, int parityType) {
    int len = strlen(data);
    int ones = countTotalOnes(data, len);
    strcpy(dataWithParity, data);
    if (parityType == 1) {
        dataWithParity[len] = (ones % 2 == 0) ? 0 : 1;
    } else {
        dataWithParity[len] = (ones % 2 != 0) ? 0 : 1;
    }
    dataWithParity[len + 1] = '\0';
}
void injectError(char *frame, int dataLen, int errorChoice) {
    if (errorChoice == 1) {
        frame[0] ^= 1;
    } else if (errorChoice == 2) {
        int foundOne = 0, foundZero = 0;
        int oneByte = -1, oneBit = -1;
        int zeroByte = -1, zeroBit = -1;
        int i, j;
        for (i = 0; i < dataLen; i++) {
            for (j = 0; j < 8; j++) {
                if (((frame[i] >> j) & 1) && !foundOne) {
                    oneByte = i;
                    oneBit = j;
                    foundOne = 1;
                } else if (!((frame[i] >> j) & 1) && !foundZero) {
                    zeroByte = i;
                    zeroBit = j;
                    foundZero = 1;
                }
            }
        }
        if (foundOne && foundZero) {
            frame[oneByte] ^= (1 << oneBit);
            frame[zeroByte] ^= (1 << zeroBit);
        }
    }
}
void checkParity(char *received, int senderParity, int receiverParity) {
    int len = strlen(received);
    if (senderParity != receiverParity) {
        printf("Error detected: Parity type mismatch between Sender and Receiver.\n");
        return;
    }
    char receivedData[MAX];
    strncpy(receivedData, received, len - 1);
    receivedData[len - 1] = '\0';
    printf("Transmitted Data Stream at Receiver Side: ");
    printBinary(receivedData, len - 1);
    printf("\n");
    int dataOnes = countTotalOnes(receivedData, len - 1);
    char receivedParityBit = received[len - 1];
    int expectedParityBit;
    if (receiverParity == 1) {
        expectedParityBit = (dataOnes % 2 == 0) ? 0 : 1;
    } else {
        expectedParityBit = (dataOnes % 2 != 0) ? 0 : 1;
    }
    if (receivedParityBit == expectedParityBit) {
        printf("No error detected in the received frame.\n");
        //printf("Original data: %s\n", receivedData);
    } else {
        printf("Error detected in the received frame.\n");
    }
}
int main() {
    char data[MAX];
    char fullFrame[MAX] = {0};
    int choice, senderParity = 0, receiverParity = 0, errorChoice = 0, frameGenerated = 0;
    while (1) {
        printf("\n=== MENU ===\n");
        printf("1. Sender (Generate Parity Frame)\n");
        printf("2. Inject Error into Transmitted Data\n");
        printf("3. Receiver (Check Parity)\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: {
                printf("Enter DATA (Alphanumeric or Binary): ");
                scanf("%s", data);
                printf("Select Sender Parity Type (1: Even, 2: Odd): ");
                scanf("%d", &senderParity);
                if (senderParity != 1 && senderParity != 2) {
                    printf("Invalid parity choice.\n");
                    break;
                }
                generateParity(data, fullFrame, senderParity);
                printf("Transmitted Data Stream at Sender Side: ");
                printBinary(data, strlen(data));
                printf("%d\n", fullFrame[strlen(data)]);
                frameGenerated = 1;
                break;
            }
            case 2: {
                if (!frameGenerated) {
                    printf("No transmitted data found! Run Sender option first.\n");
                    break;
                }
                printf("Choose Error Injection Option (1: 1-bit error, 2: 2-bit error): ");
                scanf("%d", &errorChoice);
                if (errorChoice == 1 || errorChoice == 2) {
                    injectError(fullFrame, strlen(data), errorChoice);
                    printf("Error successfully injected into the frame payload data.\n");
                } else {
                    printf("Invalid choice. No errors injected.\n");
                }
                break;
            }
            case 3: {
                if (!frameGenerated) {
                    printf("No transmitted data found! Run Sender option first.\n");
                    break;
                }
                printf("Select Receiver Expected Parity Type (1: Even, 2: Odd): ");
                scanf("%d", &receiverParity);
                if (receiverParity != 2 && receiverParity != 1) {
                    printf("Invalid parity choice.\n");
                    break;
                }
                checkParity(fullFrame, senderParity, receiverParity);
                break;
            }
            case 4: {
                printf("Exit\n");
                exit(0);
            }
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
    return 0;
}
