#!/bin/awk -f
# 1988, aho, kernighan and weinberger: the awk programming language, pp 153-174

function swap(X, a, b,    t)
{
	t = X[a]
	X[a] = X[b]
	X[b] = t
}

# quicksort
function qsort(X, left, right,    i, last){
	if(left >= right)
		return
	swap(X, left, left + int((right - left + 1) * rand()))
	last = left
	for(i=left+1; i<=right; i++)
		if(X[i] < X[left])
			swap(X, ++last, i)
	swap(X, left, last)
	qsort(X, left, last-1)
	qsort(X, last+1, right)
}

function heapify(X, left, right,	p, c){
	for(p=left; (c=2*p)<=right; p=c){
		if(c < right && X[c+1] > X[c])
			c++
		if(X[p] < X[c])
			swap(X, c, p)
	}
}

# heapsort
function hsort(X, n,	i){
	for(i=int(n/2); i>=1; i--)	# phase 1
		heapify(X, i, n)
	for(i=n; i>1; i--){		# phase 2
		swap(X, 1, i)
		heapify(X, 1, i-1)
	}
}

# insertion sort
function isort(X, n,	i, j, t){
	for(i=2; i<=n; i++)
		for(j=i; j>1 && X[j-1] > X[j]; j--)
			swap(X, j-1, j)
}

# graph topological sorting
# input: predecessor successor pairs
# representation: 3 tables:
# pcnt, scnt: predecessor and successor counts for a node
# slist[a,i]: ith successor of a node
function addnode(a, b){
	if(!(a in pcnt))
		pcnt[a] = 0		# add a to pcnt
	pcnt[b]++
	slist[a, ++scnt[a]] = b		# add b to a's successors
}

# breadth-first search
# no topological sort is possible if graph has cycles
function bfs(){
	# queue nodes with no predecessors
	for(node in pcnt){
		nodecnt++
		if(pcnt[node] == 0)
			X[++back] = node
	}
	# pop nodes, queue new nodes with no predecessors
	for(front=1; front<=back; front++){
		node = X[front]
		for(i=1; i<=scnt[node]; i++)
			if(--pcnt[slist[node,i]] == 0)
				q[++back] = slist[node,i]
	}
	# nodes never queued up are involved in a cycle
	if(back != nodecnt)
		print "error: cyclic graph"
}

# depth-first search for cycles
function dfs(node,	i, s){
	visited[node] = 1
	for(i=1; i<=scnt[node]; i++)
		if(visited[s = slist[node,i]] == 0)
			dfs(s)
		else if(visited[s] == 1)
			print "cycle with back edge (" node ", " s ")"
	visited[node] = 2
	X[pncnt++] = node
}

# depth-first (reverse) topological sort
function rtsort(){
	for(node in pcnt){
		nodecnt++
		if(pcnt[node] == 0)
			dfs(node)
	}
	if(pncnt != nodecnt)
		print "error: cyclic graph"
}
