class Solution:
    def countBits(self, n: int) -> List[int]:
        k=num=0
        lis=[]
        for i in range(n+1):
            if i==0:
                lis.append(0)
            elif i==1:
                lis.append(1)
            else:
                num=i
                c=0
                while num>0:
                    k=num%2
                    if k==1:
                        c+=1
                    num=num//2
                lis.append(c)
        return lis
