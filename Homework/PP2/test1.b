implement Test1;

include "sys.m";
        sys: Sys;
include "draw.m";

Test1: module
{
        init:fn(nil: ref Draw->Context, nil:list of string);
};

init(nil: ref Draw->Context, nil: list of string)
{
    sys = load Sys Sys->PATH;

    i := 0;
    while(1) {
        for(j := 0; j < 100000000; ++j);
            ++i;
            sys->print("%d\n", i);
    }
}

