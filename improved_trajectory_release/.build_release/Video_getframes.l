-include .build_release/Video_getframes.d

.build_release/Video_getframes.l: \
	.build_release/Video_getframes.d \

release/Video_getframes: .build_release/Video_getframes.l \
	.build_release/Video_getframes.o \

