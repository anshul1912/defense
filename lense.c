#include <gst/gst.h>
struct lense_ctx
{
	int mpty;
};
int main(int argc,char **argv)
{
	gst_init (&argc, &argv);
        GstElement *video_source = gst_element_factory_make ("v4l2src", "video_source_live");
        GstElement *deinterlace = gst_element_factory_make ("deinterlace", "deinterlace_live");
        GstElement *nv_video_mixer = gst_element_factory_make ("nv_omx_videomixer", "nv_video_mixer_live");
        GstElement *video_sink = gst_element_factory_make ("nv_gl_eglimagesink", "video_sink_live");

	/* Create the empty pipeline */
	GstElement *pipeline = gst_pipeline_new ("pipeline");
	if( !video_source || !deinterlace || !nv_video_mixer || !video_sink || !pipeline )
	{
		fprintf(stderr,"Unable to create empty pipline\n");
		return -1;
	}

	return 0;
}
