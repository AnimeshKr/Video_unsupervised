-include .build_release/VideoHR.d

.build_release/VideoHR.l: \
	.build_release/VideoHR.d \

release/VideoHR: .build_release/VideoHR.l \
	.build_release/VideoHR.o \

