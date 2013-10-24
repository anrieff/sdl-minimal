#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif

#include <SDL/SDL.h>
#include <math.h>

Uint32* frameBuffer; // a pointer to the frame buffer; an array of Uint32's, which represents pixels
int frameBufferPitch; // the size of a single image row, in number of pixels

// convert a HDR color (floatingpoint 0..1) into a LDR pixel value (0..255)
Uint32 toInt(float x) // [0..1] -> [0..255]
{
	return (int) floor(x * 255.0f + 0.5f);
}

Uint32 makeColor(float r, float g, float b)
{
	if (r > 1) r = 1;
	if (g > 1) g = 1;
	if (b > 1) b = 1;
	
	/*
	 * create a 32-bit color out of the individual channels.
	 * the lowermost 8 bits are Blue, the next 8 bits are Green,
	 * the next bits (16..23) are Red, and the topmost bits are zero (which is ignored by the videocard)
	 */
	Uint32 result = (toInt(r) << 16) + (toInt(g) << 8) + toInt(b);
	return result;
}

void putPixel(int x, int y, Uint32 color)
{
	// implement 2D array indexing in a single-dimension array "frameBuffer":
	frameBuffer[y * frameBufferPitch + x] = color;
}

int main ( int argc, char** argv )
{
	// initialize SDL video
	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "Unable to init SDL: %s\n", SDL_GetError() );
		return 1;
	}

	// make sure SDL cleans up before exit
	atexit(SDL_Quit);

	// create a new window
	SDL_Surface* screen = SDL_SetVideoMode(640, 480, 32, 0); // 640x480, 32-bit color
	if ( !screen )
	{
		printf("Unable to set 640x480 video: %s\n", SDL_GetError());
		return 1;
	}

	/*
	 * 1. The screen->pixels data is a void*. We'll imagine that it is an array of pixels,
	 *   each of Uint32-type (32-bit pixels, 4 bytes per pixel)
	 */
	frameBuffer = (Uint32*) screen->pixels;
	
	/*
	 * 2. The screen->pitch member is the size of a single framebuffer row in *BYTES*. Since we
	 *    want to quickly multiply by the row size in *PIXELS* (which are 4 bytes (which is sizeof(Uint32))),
	 *    we divide the bytes-per-row to get the pixels-per-row. On most videocards and setups, this
	 *    number will be 640 (since that's the pixel width we requested earlier)
	 */
	frameBufferPitch = screen->pitch / sizeof(Uint32);
	
	// fill the framebuffer with a gradient
	for (int y = 0; y < 480; y++)
		for (int x = 0; x < 640; x++)
			putPixel(x, y, makeColor(x/640.0, y/480.0, 0));

	// finally, update the screen :)
	SDL_Flip(screen);
	
	SDL_Delay(5000);
	// all is well ;)
	printf("Exited cleanly\n");
	return 0;
}
