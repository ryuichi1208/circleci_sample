n = int(input())
l = [list(map(int, input().split())) for i in range(n)]

ans=0

for i in range(n):
    ans += max(l[i])


print(ans)
