implement Arrays;

include "sys.m";
include "draw.m";

sys: Sys;
print: import sys;

Arrays: module {
	init: fn(nil: ref Draw->Context, nil: list of string);
};

nums1: array of int;
nums2: array of int;
nums3: array of int;
nums4: array of int;

width: int = 2000000;

init(nil: ref Draw->Context, nil: list of string) {
	sys = load Sys Sys->PATH;

	# Nums

	nums1 = array[width] of int;
	print("Len nums1: %d\n", len nums1);
	nums2 = array[width] of int;
	print("Len nums2: %d\n", len nums2);
	nums3 = array[width] of int;
	print("Len nums3: %d\n", len nums3);
	nums4 = array[width] of int;
	print("Len nums4: %d\n", len nums4);

	for(i := 0; i < width; i++){
		print("nums1[%d]: %d\n", i, nums1[i] = 1*i);
		print("nums2[%d]: %d\n", i, nums2[i] = 2*i);
		print("nums3[%d]: %d\n", i, nums3[i] = 3*i);
		print("nums4[%d]: %d\n", i, nums4[i] = 4*i);
    }

	exit;
}

