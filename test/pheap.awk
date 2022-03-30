#!/bin/awk -f
# "(open|pop|decreasekey|remain|occ)" memaddr "["x y"]" "g" g
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
$1 == "open" {
	v = $3 " " $4
	if(!(v in g))
		g[v] = $6
	if(!(v in n))
		n[v] = ni++
	print \
"Pairheap *a" n[v] " = pushqueue(" $6 ", estrdup(\"" v "\"), &queue);"
}
$1 == "pop" {
	v = $3 " " $4
	if(g[v] != $6)
		print "warning: cost g mismatch: " g[v] " vs " $0 >"/fd/2"
	if(n[v] == "")
		print "warning: empty name: " $0 >"/fd/2"
	print \
"a = popqueue(&queue);\n" \
	"\ti = strcmp((char*)a->aux, \"" v "\");\n" \
	"\tassert(i == 0);\n" \
	"\tassert(a->n == " $6 ");"
}
$1 == "decreasekey" {
	v = $3 " " $4
	d = g[v] - $6
	if(g[v] == 0)
		print "warning: undefined node \"" v "\" is decreased!" >"/fd/2"
	g[v] = $6
	print \
"decreasekey(a" n[v] ", " d ", &queue);"
}
$1 == "remain" {
	v = $3 " " $4
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
	print \
"a = popqueue(&queue);\n" \
"assert(a == nil);\n" \
"}"
}
