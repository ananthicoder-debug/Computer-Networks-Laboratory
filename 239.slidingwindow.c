#include <stdlib.h>

/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
int* maxSlidingWindow(int* nums, int numsSize, int k, int* returnSize) {
    if (numsSize == 0 || k == 0) {
        *returnSize = 0;
        return NULL;
    }

    int* result = (int*)malloc((numsSize - k + 1) * sizeof(int));
    *returnSize = numsSize - k + 1;

    // A deque to store indices of the array elements
    int* deque = (int*)malloc(numsSize * sizeof(int));
    int head = 0; // Front of the queue
    int tail = 0; // Back of the queue

    for (int i = 0; i < numsSize; i++) {
        // 1. Remove indices of elements smaller than the current element from the back
        while (tail > head && nums[deque[tail - 1]] < nums[i]) {
            tail--;
        }

        // Add the current element's index to the back
        deque[tail++] = i;

        // 2. Remove the index from the front if it is outside the current window
        if (deque[head] <= i - k) {
            head++;
        }

        // 3. The front element of the deque is the maximum of the current window
        if (i >= k - 1) {
            result[i - k + 1] = nums[deque[head]];
        }
    }

    free(deque);
    return result;
}
