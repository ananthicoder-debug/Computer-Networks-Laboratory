#include <stdio.h>

// Scenario macros
#define SCENARIO_FRAME_LOST 1
#define SCENARIO_ACK_LOST   2
#define SCENARIO_DUPLICATE  3
#define SCENARIO_TIMEOUT    4

void stopAndWait(int n, int error_frame, int scenario)
{
    int i, total = 0;
    printf("\n===== STOP-AND-WAIT ARQ =====\n");
    printf("Window Size = 1\n");

    for (i = 0; i < n; i++)
    {
        printf("\n[Sender] Transmitting Frame %d\n", i);
        total++;

        if (i == error_frame)
        {
            if (scenario == SCENARIO_FRAME_LOST)
            {
                printf("[Network] Frame %d LOST/CORRUPTED in transit\n", i);
                printf("[Sender] Timer Expired! TRIGGERING TIMEOUT for Frame %d\n", i);
                printf("[Sender] Retransmitting Frame %d\n", i);
                total++;
                printf("[Receiver] Frame %d correctly received\n", i);
                printf("[Sender] ACK %d received\n", i);
            }
            else if (scenario == SCENARIO_ACK_LOST)
            {
                printf("[Receiver] Frame %d correctly received\n", i);
                printf("[Network] ACK %d generated but LOST in transit\n", i);
                printf("[Sender] Timer Expired! TRIGGERING TIMEOUT (No ACK received)\n", i);
                printf("[Sender] Retransmitting Frame %d\n", i);
                total++;
                printf("[Receiver] Frame %d detected as DUPLICATE! Discarding frame...\n", i);
                printf("[Sender] ACK %d received successfully\n", i);
            }
            else if (scenario == SCENARIO_DUPLICATE)
            {
                printf("[Receiver] Frame %d correctly received\n", i);
                printf("[Sender] ACK %d received\n", i);
                printf("[Sender] Simulating network anomaly: Sending DUPLICATE Frame %d anyway\n", i);
                total++;
                printf("[Receiver] Frame %d detected as DUPLICATE! Discarding frame...\n", i);
            }
            else if (scenario == SCENARIO_TIMEOUT)
            {
                printf("[Receiver] Frame %d delayed heavily\n", i);
                printf("[Sender] Timer Expired! TRIGGERING TIMEOUT due to slow network\n", i);
                printf("[Sender] Retransmitting Frame %d\n", i);
                total++;
                printf("[Receiver] Frame %d correctly received\n", i);
                printf("[Sender] ACK %d received\n", i);
            }
            error_frame = -1; // Clear error after handling
        }
        else
        {
            printf("[Receiver] Frame %d correctly received\n", i);
            printf("[Sender] ACK %d received\n", i);
        }
    }
    printf("\nAll frames delivered successfully.\n");
    printf("Total transmissions = %d\n", total);
}

void goBackN(int n, int w, int error_frame, int scenario)
{
    int i = 0, j, k;
    int total = 0;
    printf("\n===== GO-BACK-N ARQ =====\n");
    printf("Window Size = %d\n", w);

    while (i < n)
    {
        printf("\n--- Current Window starting at Frame %d ---\n", i);

        for (j = i; j < i + w && j < n; j++)
        {
            printf("[Sender] Transmitting Frame %d\n", j);
            total++;

            if (j == error_frame)
            {
                if (scenario == SCENARIO_FRAME_LOST)
                {
                    printf("[Network] Frame %d LOST/CORRUPTED\n", j);
                    printf("[Sender] TRIGGERING TIMEOUT for Frame %d\n", j);
                    printf("\n[Go-Back-N] Rolling back window. Retransmitting from Frame %d:\n", j);
                    for (k = j; k < i + w && k < n; k++)
                    {
                        printf("[Sender] Retransmitting Frame %d\n", k);
                        total++;
                        printf("[Sender] ACK %d received\n", k);
                    }
                }
                else if (scenario == SCENARIO_ACK_LOST)
                {
                    printf("[Receiver] Frame %d correctly received\n", j);
                    printf("[Network] ACK %d LOST\n", j);
                    printf("[Sender] TRIGGERING TIMEOUT (No ACK for Frame %d)\n", j);
                    printf("\n[Go-Back-N] Rolling back window. Retransmitting from Frame %d:\n", j);
                    for (k = j; k < i + w && k < n; k++)
                    {
                        printf("[Sender] Retransmitting Frame %d (DUPLICATE)\n", k);
                        total++;
                        printf("[Receiver] Frame %d detected as DUPLICATE! Discarding...\n", k);
                        printf("[Sender] ACK %d received\n", k);
                    }
                }
                else if (scenario == SCENARIO_DUPLICATE)
                {
                    printf("[Receiver] Frame %d correctly received\n", j);
                    printf("[Sender] ACK %d received\n", j);
                    printf("[Sender] Injecting DUPLICATE Frame %d into network\n", j);
                    total++;
                    printf("[Receiver] Frame %d detected as DUPLICATE! Discarding...\n", j);
                }
                else if (scenario == SCENARIO_TIMEOUT)
                {
                    printf("[Receiver] Frame %d received but delayed\n", j);
                    printf("[Sender] TRIGGERING TIMEOUT due to delay\n", j);
                    printf("\n[Go-Back-N] Retransmitting window from Frame %d:\n", j);
                    for (k = j; k < i + w && k < n; k++)
                    {
                        printf("[Sender] Retransmitting Frame %d\n", k);
                        total++;
                        printf("[Sender] ACK %d received\n", k);
                    }
                }
                error_frame = -1;
                break;
            }
            else
            {
                printf("[Receiver] Frame %d correctly received\n", j);
                printf("[Sender] ACK %d received\n", j);
            }
        }
        i = i + w;
    }
    printf("\nAll frames delivered successfully.\n");
    printf("Total transmissions = %d\n", total);
}

void selectiveRepeat(int n, int w, int error_frame, int scenario)
{
    int i, j;
    int total = 0;
    printf("\n===== SELECTIVE REPEAT ARQ =====\n");
    printf("Window Size = %d\n", w);

    for (i = 0; i < n; i += w)
    {
        printf("\n--- Current Window starting at Frame %d ---\n", i);

        for (j = i; j < i + w && j < n; j++)
        {
            printf("[Sender] Transmitting Frame %d\n", j);
            total++;

            if (j == error_frame)
            {
                if (scenario == SCENARIO_FRAME_LOST)
                    printf("[Network] Frame %d LOST/CORRUPTED\n", j);
                else if (scenario == SCENARIO_ACK_LOST)
                {
                    printf("[Receiver] Frame %d correctly received\n", j);
                    printf("[Network] ACK %d LOST\n", j);
                }
                else if (scenario == SCENARIO_DUPLICATE)
                {
                    printf("[Receiver] Frame %d correctly received\n", j);
                    printf("[Sender] ACK %d received\n", j);
                    printf("[Sender] Injecting DUPLICATE Frame %d\n", j);
                    total++;
                    printf("[Receiver] Frame %d checked against buffer: DUPLICATE! Discarding...\n", j);
                }
                else if (scenario == SCENARIO_TIMEOUT)
                {
                    printf("[Receiver] Frame %d accepted with heavy delay\n", j);
                }
            }
            else
            {
                printf("[Receiver] Frame %d correctly received\n", j);
                printf("[Sender] ACK %d received\n", j);
            }
        }

        if (error_frame >= i && error_frame < i + w && scenario != SCENARIO_DUPLICATE)
        {
            printf("\n[Sender] TRIGGERING TIMEOUT for Frame %d\n", error_frame);
            printf("[Selective Repeat] Retransmitting ONLY Frame %d\n", error_frame);
            printf("[Sender] Retransmitting Frame %d\n", error_frame);
            total++;

            if (scenario == SCENARIO_ACK_LOST || scenario == SCENARIO_TIMEOUT)
                printf("[Receiver] Frame %d checked against buffer: DUPLICATE! Discarding...\n", error_frame);
            else
                printf("[Receiver] Frame %d safely stored in buffer\n", error_frame);

            printf("[Sender] ACK %d received\n", error_frame);
        }
    }
    printf("\nAll frames delivered successfully.\n");
    printf("Total transmissions = %d\n", total);
}

int main()
{
    int choice, sce;
    int n, w = 1, error_frame;

    do {
        printf("========================================\n");
        printf("       SLIDING WINDOW PROTOCOLS\n");
        printf("========================================\n");
        printf("1. Stop-and-Wait ARQ\n2. Go-Back-N ARQ\n3. Selective Repeat ARQ\n4. Exit\n");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        if (choice == 4 || choice < 1 || choice > 3) {
            printf("\nExiting main menu. Program terminated.\n");
            return 0;
        }

        printf("Enter total number of frames: ");
        scanf("%d", &n);

        if (choice == 2 || choice == 3)
        {
            printf("Enter window size: ");
            scanf("%d", &w);
        }

        do {
            printf("\n--- Select Case Scenario ---\n");
            printf("1. Frame Lost\n2. ACK Lost\n3. Duplicate Frame\n4. Timeout (Delayed Action)\n5. Exit Scenario Menu\n");
            printf("Enter scenario (1-5): ");
            scanf("%d", &sce);

            // FIXED: Instantly break out if 5 is selected before prompting for frame numbers
            if (sce == 5) {
                printf("\nReturning to Sliding Window Protocols Menu...\n\n");
                break;
            }

            if (sce < 1 || sce > 5) {
                printf("Invalid scenario choice! Try again.\n");
                continue;
            }

            printf("Enter frame number to inject this scenario into: ");
            scanf("%d", &error_frame);

            if (error_frame < 0 || error_frame >= n)
            {
                printf("Invalid frame number! Please try again.\n");
                continue;
            }

            switch (choice)
            {
                case 1: stopAndWait(n, error_frame, sce); break;
                case 2: goBackN(n, w, error_frame, sce); break;
                case 3: selectiveRepeat(n, w, error_frame, sce); break;
            }

        } while (sce != 5);

    } while (choice != 4);

    return 0;
}
