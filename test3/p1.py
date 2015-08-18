#!/usr/bin/python
# coding: utf-8
# 
# @file: p1.py
# @date: 2015-08-18
# @brief:
# @detail:
#
#################################################################


import random

random.seed()
random.randrange(-1, 1)

def forwardMultiplyGate(x, y):
    return x * y;

def forwardAdd(x, y):
    return x + y

def forwardCircuit(x, y, z):
    q = forwardAdd(x, y)
    return forwardMultiplyGate(q, z)


x, y = -2, 3

print forwardMultiplyGate(x, y)

print '==== step 1 ===='

tweak_amount = 0.001
best = -100000

best_x, best_y = x, y

for i in xrange(1, 100):
    try_x = best_x + tweak_amount * random.random() # 随即走一步
    try_y = best_y + tweak_amount * random.random()

    out = forwardMultiplyGate(try_x, try_y)
    if out > best:
        best_x, best_y = try_x, try_y   # 如果有效，则更新
        best = out

print forwardMultiplyGate(best_x, best_y)

print '=== step 2 ==='
dx = y # f = x * y，则 对于 f 求 x 的偏导就是y
dy = x

next_x = x + tweak_amount * dx  # 下一个值就是沿着导数方向累加一小步
next_y = y + tweak_amount * dy

print forwardMultiplyGate(next_x, next_y)

print '=== step 3 ==='
x, y, z = -2, 3, -4
print forwardCircuit(x, y, z)

# 对于组合函数，需要利用链式法则，逐步求导
# f = q * z, q = x + y
# fdz = q, fdq = z
# qdx = 1, qdy = 1
# fdx = fdq * qdx = z * 1
# fdy = fdq * qdy = z * 1
fdx = z * 1
fdy = z * 1
fdz = x + y

print forwardCircuit(x + tweak_amount*fdx, y + tweak_amount*fdy, z + tweak_amount*fdz)



# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

