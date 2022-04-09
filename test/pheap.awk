#!/bin/awk -f
BEGIN{
	print \
"#include <u.h>\n" \
"#include <libc.h>\n" \
"#include \"asif.h\"\n" \
"debuglevel = Logtrace;\n\n" \
"void main(void) {\n" \
"Pairheap *queue, *a;\n" \
"int i;\n" \
"queue = nil;\n"
}
# pheap::pushqueue addr v
$1 == "pheap::pushqueue" {
	v = $2
	c = int($3 * 1e3)
	if(v in g)
		print "warning: duplicate open node: " $0 >"/fd/2"
	g[v] = c
	if(!(v in n))
		n[v] = ni++
	print \
"Pairheap *a" n[v] " = pushqueue(" g[v] ", estrdup(\"" v "\"), &queue);"
}
# pheap::popqueue addr v
$1 == "pheap::popqueue" {
	v = $2
	c = int($3 * 1e3)
	if(g[v] != c)
		print "warning: cost g mismatch: " g[v] " vs " $0 >"/fd/2"
	if(n[v] == "")
		print "warning: empty name: " $0 >"/fd/2"
	print \
"a = popqueue(&queue);" \
	#"\n\ti = strcmp((char*)a->aux, \"" v "\");\n" \
	#"\tassert(i == 0);\n" \
	#"\tassert(a->n == " c ");"
}
# pheap::decreasekey addr old diff\nold
$1 == "pheap::decreasekey" {
	v = $2
	c = int($3 * 1e3)
	d = g[v] - c
	if(g[v] == 0)
		print "warning: undefined node \"" v "\" is decreased!" >"/fd/2"
	g[v] = c
	print \
"decreasekey(a" n[v] ", " d ", &queue);"
}
$1 == "remain" {
	v = $2
	print \
"a = popqueue(&queue);\n" \
	"\ti = strcmp((char*)a->aux, \"" v "\");\n" \
	"\tassert(i == 0);"
}
$1 == "occ" {
	print \
"// " $0
}
END {
#	print \
#"a = popqueue(&queue);\n" \
#assert(a == nil);\n" \
#"}"
print \
"printqueue(&queue);\n" \
"nukequeue(&queue);\n" \
"exits(nil);\n" \
"}"
}
