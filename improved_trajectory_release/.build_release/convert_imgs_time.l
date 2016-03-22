-include .build_release/convert_imgs_time.d

.build_release/convert_imgs_time.l: \
	.build_release/convert_imgs_time.d \

release/convert_imgs_time: .build_release/convert_imgs_time.l \
	.build_release/convert_imgs_time.o \

