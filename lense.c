#include <gst/gst.h>
struct lense_ctx
{
	int mpty;
};
int main(int argc,char **argv)
{
	gst_init (&argc, &argv);
        GstElement *video_source;
        GstElement *deinterlace;
        GstElement *nv_video_mixer;
        GstElement *video_sink;

	video_source = gst_element_factory_make ("v4l2src", "video_source_live");
	if (!video_source) {
		fprintf(stderr,"Unable to create element v4l2src\n");
		return -1;
	}

	deinterlace = gst_element_factory_make ("deinterlace", "deinterlace_live");
	if (!deinterlace) {
		fprintf(stderr,"Unable to create element dinterlace\n");
		return -1;
	}


	nv_video_mixer = gst_element_factory_make ("nv_omx_videomixer", "nv_video_mixer_live");
	if(!nv_video_mixer) {
		fprintf(stderr,"Unable to create element nv_video_mixer\n");
		return -1;
	}

	video_sink = gst_element_factory_make ("nv_gl_eglimagesink", "video_sink_live");
	if (!video_sink) {
		fprintf(stderr,"Unable to create element video sink live\n");
		return -1;
	}
	/* Create the empty pipeline */
	GstElement *pipeline = gst_pipeline_new ("pipeline");
	if( !pipeline )
	{
		fprintf(stderr, "created Empty pipeline\n");
	}

	return 0;
}
