
#include "bitmap.h"

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

//compute_image(bm,xcenter-scale,xcenter+scale,ycenter-scale,ycenter+scale,max);

typedef struct {
	struct bitmap *my_bm;
	struct bitmap *global_bm;
	int xmin;
	int xmax;
	int ymin;
	int ymax;
	int max;
	int lines;
	int minline;
} split_bm;

int iteration_to_color( int i, int max );
int iterations_at_point( double x, double y, int max );
// void compute_image( struct bitmap *bm, double xmin, double xmax, double ymin, double ymax, int max );
void *compute_image(void *arg);

void show_help()
{
	printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>  X coordinate of image center point. (default=0)\n");
	printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>  Scale of the image in Mandlebrot coordinates. (default=4)\n");
	printf("-W <pixels> Width of the image in pixels. (default=500)\n");
	printf("-H <pixels> Height of the image in pixels. (default=500)\n");
	printf("-o <file>   Set output file. (default=mandel.bmp)\n");
	printf("-n <threads>Set number of threads. (default=1)\n");
	printf("-h          Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}

int main( int argc, char *argv[] )
{
	char c;

	// These are the default configuration values used
	// if no command line arguments are given.

	const char *outfile = "mandel.bmp";
	double xcenter = 0;
	double ycenter = 0;
	double scale = 4;
	int    image_width = 500;
	int    image_height = 500;
	int    max = 1000;
	int	 num_threads = 1;

	// For each command line argument given,
	// override the appropriate configuration value.

	while((c = getopt(argc,argv,"x:y:s:W:H:m:o:n:h"))!=-1) {
		switch(c) {
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				scale = atof(optarg);
				break;
			case 'W':
				image_width = atoi(optarg);
				break;
			case 'H':
				image_height = atoi(optarg);
				break;
			case 'm':
				max = atoi(optarg);
				break;
			case 'o':
				outfile = optarg;
				break;
			case 'n':
				num_threads = atoi(optarg);
				break;
			case 'h':
				show_help();
				exit(1);
				break;
		}
	}

	// Display the configuration of the image.
	printf("mandel: x=%lf y=%lf scale=%lf max=%d outfile=%s threads=%i\n",xcenter,ycenter,scale,max,outfile,num_threads);

	// Create a bitmap of the appropriate size.
	struct bitmap *bm = bitmap_create(image_width,image_height);

	// Fill it with a dark blue, for debugging
	bitmap_reset(bm,MAKE_RGBA(0,0,255,0));

	// Compute the Mandelbrot image
	//compute_image(bm,xcenter-scale,xcenter+scale,ycenter-scale,ycenter+scale,max);
	
	pthread_t threadID[num_threads];
	//pthread_mutex_t *mutex;
	//threadID = malloc(num_threads*sizeof(pthread_t));
	split_bm *split_bm_data;
	split_bm_data = malloc(num_threads*sizeof(split_bm));
	//mutex = malloc(sizeof(pthread_mutex_t));
	//pthread_mutex_init(mutex, NULL);

	int i;
	int rows = image_height / num_threads;
	for (i = 0; i < num_threads; i++) {
		if (image_height % num_threads == 0) {
			split_bm_data[i].lines = rows;
		}
		else {
			if (i < image_height % num_threads) {
				split_bm_data[i].lines = rows + 1;
			}
			else {
				split_bm_data[i].lines = rows;
			}
		}
	}

	split_bm_data[0].minline = 0;
	for (i = 0; i < num_threads; i++) {
		split_bm_data[i].global_bm = bm;
		split_bm_data[i].xmin = xcenter - scale;
		split_bm_data[i].xmax = xcenter + scale;
		split_bm_data[i].ymin = ycenter - scale;
		split_bm_data[i].ymax = ycenter + scale;
		split_bm_data[i].max = max;
		if (i != 0) {
			split_bm_data[i].minline = split_bm_data[i-1].minline + split_bm_data[i-1].lines;
		}
		pthread_create(&threadID[i], NULL, compute_image, (void*)&split_bm_data[i]);
		/* printf("rows for thread %i: %i\n", i, split_bm_data[i].lines);
		printf("row: %i - %i\n", split_bm_data[i].minline, split_bm_data[i].minline + split_bm_data[i].lines - 1); */
	}

	void *status;
	for (i = 0; i < num_threads; i++) {
		pthread_join(threadID[i], &status);
	}

	// Save the image in the stated file.
	if(!bitmap_save(bm,outfile)) {
		fprintf(stderr,"mandel: couldn't write to %s: %s\n",outfile,strerror(errno));
		return 1;
	}
	//free(threadID);
	free(split_bm_data);
	//pthread_mutex_destroy(mutex);
	//free(mutex);
	return 0;
}

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/

//void compute_image( struct bitmap *bm, double xmin, double xmax, double ymin, double ymax, int max )

/*typedef struct {
	struct bitmap *my_bm;
	int xmin;
	int xmax;
	int ymin;
	int ymax;
	int max;
	int lines;
	int minline;
} split_bm; */
void *compute_image(void *arg)
{
	split_bm *arg_bm;
	arg_bm = arg;
	int i,j;
	struct bitmap *bm = arg_bm->global_bm;
	int xmin = arg_bm->xmin;
	int xmax = arg_bm->xmax;
	int ymin = arg_bm->ymin;
	int ymax = arg_bm->ymax;
	int max = arg_bm->max;
	int lines = arg_bm->lines;
	int minline = arg_bm->minline;

	int width = bitmap_width(bm);
	int height = lines;

	// For every pixel in the image...

	for(j=minline;j<minline+lines;j++) {
	//for (j=0;j<height;j++) {	
		for(i=0;i<width;i++) {

			// Determine the point in x,y space for that pixel.
			double x = xmin + i*(xmax-xmin)/width;
			double y = ymin + j*(ymax-ymin)/height;
	
			// Compute the iterations at that point.
			int iters = iterations_at_point(x,y,max);
			//printf("x:%f y:%f i:%i \n", x, y, iters);
	
			// Set the pixel in the bitmap.
			//printf("%i\n", iters);
			bitmap_set(bm,i,j,iters);
		}
	}
	pthread_exit(NULL);
}

/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while( (x*x + y*y <= 4) && iter < max ) {

		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;

		x = xt;
		y = yt;

		iter++;
	}

	return iteration_to_color(iter,max);
}

/*
Convert a iteration number to an RGBA color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/

int iteration_to_color( int i, int max )
{
	int gray = 255*i/max;
	return MAKE_RGBA(gray,gray,gray,0);
}




