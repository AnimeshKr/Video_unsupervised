-include .build_release/convert_imgs.d

.build_release/convert_imgs.l: \
	.build_release/convert_imgs.d \

release/convert_imgs: .build_release/convert_imgs.l \
	.build_release/convert_imgs.o \

