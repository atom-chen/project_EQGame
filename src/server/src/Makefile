
all:
	cd ../gen_protocal;make
	cd task;make
	cd npc;make
	make -f Makefile.mk


dialyzer:
	dialyzer --src -DNOTEST -DDIALYZER -c ../src
	
test:
	make -f Makefile.mk test

clean:
	rm -f ../ebin/*.beam