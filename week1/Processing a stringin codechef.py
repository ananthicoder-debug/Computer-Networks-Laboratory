import sys
def solve():
    input_data = sys.stdin.read().split()
    if not input_data:
        return
    t = int(input_data[0])
    for i in range(1, t + 1):
        s = input_data[i]
        digit_sum = sum(int(char) for char in s if char.isdigit())
        print(digit_sum)
if __name__ == '__main__':
    solve()
