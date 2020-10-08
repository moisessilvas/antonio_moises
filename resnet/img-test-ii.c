#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <jerror.h>
#include <setjmp.h>

struct my_error_mgr {
  struct jpeg_error_mgr pub;    /* "public" fields */

  jmp_buf setjmp_buffer;        /* for return to caller */
};
typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void) my_error_exit (j_common_ptr cinfo)
{
  my_error_ptr myerr = (my_error_ptr) cinfo->err;
  (*cinfo->err->output_message) (cinfo);
  longjmp(myerr->setjmp_buffer, 1);
}

unsigned char * image_buffer;  // Points to large array of R,G,B-order data
int image_height;     // Number of rows
int image_width;      // Number of columns

int read_JPEG_file(char * filename);

int main()
{
    char * filename = "../figura2.jpg";
    read_JPEG_file(filename);
    return 0;
}

GLOBAL(int)
read_JPEG_file (char * filename)
{

  struct jpeg_decompress_struct cinfo;

  struct my_error_mgr jerr;
  FILE * infile;        /* source file */
  JSAMPARRAY buffer;        /* Output row buffer */
  int row_stride;       /* physical row width in output buffer */
  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return 0;
  }

  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 0;
  }
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);
  (void) jpeg_read_header(&cinfo, TRUE);

  (void) jpeg_start_decompress(&cinfo);
  row_stride = cinfo.output_width * cinfo.output_components;
  buffer = (*cinfo.mem->alloc_sarray)
        ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
  while (cinfo.output_scanline < cinfo.output_height) {
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    put_scanline_someplace(buffer[0], row_stride);
  }

  (void) jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(infile);

  return 1;
}