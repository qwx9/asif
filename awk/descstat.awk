#!/bin/awk -f
# assumption: no missing values/indices in input data

function swap(X, a, b,    t)
{
	t = X[a]
	X[a] = X[b]
	X[b] = t
}

# from numerical recipes 3rd ed; rearranges X
function select(X, k,    i, j, n, m, l, ir)
{
	l = 1
	ir = length(X)
	for(;;){
		if(ir <= l+1){
			if(ir == l+1 && X[ir] < X[l])
				swap(X, l, ir)
			return int(X[k])
		}else{
			m = (l + ir) / 2
			swap(X, m, l+1)
			if(X[l] > X[ir])
				swap(X, l, ir)
			if(X[l+1] > X[ir])
				swap(X, l+1, ir)
			if(X[l] > X[l+1])
				swap(X, l, l+1)
			i = l + 1
			j = ir
			m = X[l+1]
			for(;;){
				do i++; while(X[i] < m)
				do j--; while(X[j] > m)
				if(j < i)
					break
				swap(X, i, j)
			}
			X[l+1] = X[j]
			X[j] = m
			if(j >= k)
				ir = j - 1
			if(j <= k)
				l = i
		}
	}
}

function max(X,    n)
{
	n = "-inf"
	for(i in X)
		if(X[i] > n)
			n = X[i]
	return n
}

function min(X,    n)
{
	n = "inf"
	for(i in X)
		if(X[i] < n)
			n = X[i]
	return n
}

function sum(X,    i, n)
{
	for(i in X)
		n += X[i]
	return n
}

function mean(X)
{
	return sum(X) / length(X)
}

function var(X,    i, n, m)
{
	m = mean(X)
	for(i in X)
		n += (X[i] - m) ^ 2
	return n / (length(X) - 1)
}

function sd(X)
{
	return sqrt(var(X))
}

# FIXME: this is wrong and produces wrong results in subsequent stuff
# select is busted
# rearranges X
function median(X,    n)
{
	n = select(X, int(length(X) / 2 + 1))
	if(length(X) % 2 != 0)
		return n
	else
		return (select(X, int(length(X) / 2)) + n) / 2
}

function freq(X)
{
	delete ans
	for(i in X)
		ans[X[i]]++
}
