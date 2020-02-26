# this file implements the module interface "fib"
implement Fibonacci;


# include the files sys.m & draw.m (from /module)
include "sys.m";
include "draw.m";


# "fib" is a module, it has the function ("fn") "init" with two parameters.
# both parameters are unnamed ("nil") because they are not used.
Fibonacci: module {
	init:fn(nil: ref Draw->Context, nil: list of string);
};


# implementation of the function "init", as described in the
# module interface "fib" above.  "init" in limbo is like "main" in C.
init(nil: ref Draw->Context, nil: list of string)
{
	# declare "sys" by assigning it the result of loading the Sys module.
	sys := load Sys Sys->PATH;
    for( n := 0; n < 35; n++){
        # call function "print" from the loaded module "sys"
        sys->print("fib %d: %d\n", n, fib(n));
    }
}

fib(n : int) : int
{
    if( n < 2){
        return n;
    }
    return fib(n-2) + fib(n-1);
}

