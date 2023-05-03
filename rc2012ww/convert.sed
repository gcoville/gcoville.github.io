s=\.include=include=
s/*=/org/
s=\.byte=fcb=
s=\.word=fdb=
s=#<\([^ 	]*\)=#lo(\1)=
s=#>\([^ 	]*\)=#hi(\1)=
s=#\[\(.*\)\]=(\1)=
