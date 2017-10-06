all:
	rm -f *flymake*
	erl -noshell -pz ../smart_exceptions/ebin -make

dialyzer:
	dialyzer --src -DNOTEST -DDIALYZER -c ../../src

clean:
	rm -f ../../ebin/*.beam