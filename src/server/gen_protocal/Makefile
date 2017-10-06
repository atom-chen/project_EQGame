all:compile

compile:
	@rm -f *flymake*
	@svn up
	erl -noshell -pz ../smart_exceptions/ebin -make
	erl -pz ebin -noshell -s gen_protocal start -s gen_enum_def start -s gen_protocal_cpp start -s init stop 
	@mv  protocal.erl ../src; 
	@mv  packet_def.hrl ../include; 
	@mv  enum_def.hrl ../include; 
	@mv  EnumDef.h ../../client/Client/EQComm/EQComm; 
	@mv  NetMsgType.h ../../client/Client/EQComm/EQComm; 
	@mv  NetPacket.h ../../client/Client/EQComm/EQComm

clean:
	rm -rf ebin/*.beam erl_crash.dump
