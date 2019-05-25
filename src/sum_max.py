n = int(input())
l = [ list(map(int, input().split())) for i in range(n) ]

num_of_sum = []
for line in range(n):
    num_of_sum.append(sum( l[line] ))

print( max(num_of_sum) )

#print(max_value)
