import sys;
import os;
def parseParam(argv):
    i = 1
    args=""
    shell = 0
    ctname=""
    master = 0
    browser = ""
    vts = 0
    while i < len(argv):
        if argv[i] == "-config" :
            args += " -ct_config";
        elif argv[i] == "-vts" :
            vts=1;
            args += " " + argv[i];
        elif argv[i] == "-browser" :
            browser=argv[i+1];
            args += " " +argv[i];
        elif argv[i] == "-shell" :
            shell=1;
            args += " " +argv[i];
        elif argv[i] == "-ctname" :
            ctname=argv[i+1];
            #args="$args";
        elif argv[i] == "-ctmaster" :
            master=1;
            #args="$args";
        else:
            args += " " +argv[i]
        i += 1
    return (args, vts, browser, shell, ctname, master);

def makeStr((args, vts, browser, shell, ctname, master)):
    common_test_path = "D:/erl5.7.5/lib/common_test-1.4.7/ebin"
    test_server_path = "D:/erl5.7.5/lib/test_server-3.3.6/ebin"
    str = ""
    if vts :
        str = "erl -noshell -pz ..\smart_exceptions\ebin " + common_test_path + " " + test_server_path \
              + " -sname ct " + args + " -s webtool script_start vts " + browser + "-s ct_run script_start "
    elif shell :
        str = "erl -noshell -pz ..\smart_exceptions\ebin " + common_test_path + " " + test_server_path \
              + " -sname ct " + args + " -s ct_run script_start "
    elif ctname :
        str = "erl -noshell -pz ..\smart_exceptions\ebin " + common_test_path + " " + test_server_path \
              + " -sname " + ctname + " " + args
    elif master :
        str = "erl -noshell -pz ..\smart_exceptions\ebin " + common_test_path + " " + test_server_path \
              + " -sname ct_master " + args
    else:
        str = "erl -pz ..\smart_exceptions\ebin " + common_test_path + " " + test_server_path \
              + " -sname ct " + args + " -s ct_run script_start" # -s erlang halt"
    return str;

if __name__ == "__main__":
    ret = parseParam(sys.argv)
    str = makeStr(ret)
    os.system(str)
