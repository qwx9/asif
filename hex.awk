#!/bin/awk -f
function hex(s, v){
	if(s ~ /^0x/)
		s = substr(s, 3)
	for(n=1; n<=length(s); n++)
		v = v * 16 + h[substr(s, n, 1)]
	return v
}
BEGIN{
	for(n=0; n<16; n++){
		h[sprintf("%x", n)] = n
		h[sprintf("%X", n)] = n
	}
}
