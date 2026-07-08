class Solution:
    def findComplement(self, num: int) -> int:
        l = list()
        while num > 0:
            r = num % 2
            l.append(r)
            num = num // 2  
        m = l[::-1]
        for i in range(len(m)):
            if m[i] == 0:
                m[i] = 1
            else:
                m[i] = 0
        n = m[::-1]
        h = 0 
        for i in range(len(n)):
            h += n[i] * (2 ** i) 
    
        return h
