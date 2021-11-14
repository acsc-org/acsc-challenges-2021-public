all:
	gcc -Wl,-z,relro,-z,lazy main.c -o ../distfiles/filtered -no-pie -fno-stack-protector -O0
	cp main.c ../distfiles/filtered.c
