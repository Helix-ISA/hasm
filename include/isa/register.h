#ifndef HX_REGISTER_H
#define HX_REGISTER_H

typedef enum {
	HX_WIDTH_8,
	HX_WIDTH_16,
	HX_WIDTH_32,
	HX_WIDTH_64
} hx_width;

const char *width_name(hx_width width);

#endif
