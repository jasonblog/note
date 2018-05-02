lis1=list(range(1,15,2))
lis1=lis1[slice(0,5,1)]
lis1=sorted(lis1,reverse=True)
lis1=lis1[slice(0,3,1)]
lis1=enumerate(lis1)
print(next(lis1)+next(lis1)+next(lis1))
