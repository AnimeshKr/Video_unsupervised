-include .build_release/VideoHR_scale.d

.build_release/VideoHR_scale.l: \
	.build_release/VideoHR_scale.d \

release/VideoHR_scale: .build_release/VideoHR_scale.l \
	.build_release/VideoHR_scale.o \

