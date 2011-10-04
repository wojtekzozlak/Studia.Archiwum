struct intpair {
	int a;
	int b;
};

program SAD_PROGRAM {
	version SAD_VERS {
		intpair SAD(intpair) = 1;
	} = 1;
} = 0x30000000;
