/*
 * Copyright (C) 2015 Anshul Maheshwari er.anshul.maheshwari@gmail.com
 */

/**
 * SECTION:element-plugin
 *
 * This plugin do the correction of distortion done by lense.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! lensecorrection ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>
#include <gst/controller/gstcontroller.h>
#include <gst/video/video.h>
#include "lensecorrection.h"
#include "math.h"
#include <time.h>
GST_DEBUG_CATEGORY_STATIC (gst_lense_correction_debug);
#define GST_CAT_DEFAULT gst_lense_correction_debug
#define sqr(x) ( ( (x)*(x) ) )
#define FRAME_OFFSET 500
/* Lense Correction properties */
enum
{
	PROP_0,
	PROP_SILENT
	/* FILL ME */
};

enum
{
  /* FILL ME */
  LAST_SIGNAL
};

/* the capabilities of the inputs and outputs.
 *
 * FIXME:describe the real formats here.
 */
static GstStaticPadTemplate sink_template =
GST_STATIC_PAD_TEMPLATE (
  "sink",
  GST_PAD_SINK,
  GST_PAD_ALWAYS,
  GST_STATIC_CAPS ("ANY")
);

static GstStaticPadTemplate src_template =
GST_STATIC_PAD_TEMPLATE (
  "src",
  GST_PAD_SRC,
  GST_PAD_ALWAYS,
  GST_STATIC_CAPS ("ANY")
);

/* debug category for fltering log messages
 *
 * FIXME:exchange the string 'Template plugin' with your description
 */
#define DEBUG_INIT(bla) \
  GST_DEBUG_CATEGORY_INIT (gst_lense_correction_debug, "plugin", 0, "Template plugin");

GST_BOILERPLATE_FULL (GstLenseCorrection, gst_lense_correction, GstBaseTransform,
    GST_TYPE_BASE_TRANSFORM, DEBUG_INIT);

static void gst_lense_correction_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_lense_correction_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);


/* GObject vmethod implementations */

static void
gst_lense_correction_base_init (gpointer klass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (klass);

  gst_element_class_set_details_simple (element_class,
    "Plugin",
    "Generic/Filter",
    "FIXME:Generic Template Filter",
    "AUTHOR_NAME AUTHOR_EMAIL");

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&src_template));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&sink_template));
}

/* initialize the new element
 * initialize instance structure
 */
static void
gst_lense_correction_init (GstLenseCorrection *filter, GstLenseCorrectionClass * klass)
{
	filter->silent = FALSE;
	filter->map_eq_y = NULL;
	filter->map_eq_u = NULL;
	filter->map_eq_v = NULL;
	filter->frame_count=1;
}

static void
gst_lense_correction_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstLenseCorrection *filter = GST_LENSE_CORRECTION (object);

  switch (prop_id) {
    case PROP_SILENT:
      filter->silent = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void gst_lense_correction_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
	GstLenseCorrection *filter = GST_LENSE_CORRECTION (object);

	switch (prop_id) {
	case PROP_SILENT:
		g_value_set_boolean (value, filter->silent);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
		break;
	}
}


static void set_lense_eq_map(GstLenseCorrection *filter)
{
	GstVideoFormat format = filter->in_format;
	gint sw = filter->from_width;
	gint sh = filter->from_height;
	gint dw = filter->to_width;
	gint dh = filter->to_height;
	gint x, y;
	float k1 = -0.1;
	float k2 = 0.0095;
	float k3 = 0.0;
	const long double Cx = 0;
	const long double Cy = 0;
	const long double p1  = 0;
	const long double p2  = 0;
	const long double s = 1.2;
	//int radius;


	filter->src_y_stride = gst_video_format_get_row_stride (format, 0, sw);
	filter->src_u_stride = gst_video_format_get_row_stride (format, 1, sw);
	filter->src_v_stride = gst_video_format_get_row_stride (format, 2, sw);
#ifdef DEBUG
	g_print ("filter->src_y_stride %d\n",filter->src_y_stride);
	g_print ("filter->src_u_stride %d\n",filter->src_u_stride);
	g_print ("filter->src_v_stride %d\n",filter->src_v_stride);
#endif

	filter->dest_y_stride = gst_video_format_get_row_stride (format, 0, dw);
	filter->dest_u_stride = gst_video_format_get_row_stride (format, 1, dw);
	filter->dest_v_stride = gst_video_format_get_row_stride (format, 2, dw);
#ifdef DEBUG
	g_print ("filter->dest_y_stride %d\n", filter->dest_y_stride);
	g_print ("filter->dest_u_stride %d\n", filter->dest_u_stride);
	g_print ("filter->dest_v_stride %d\n", filter->dest_v_stride);
#endif

	filter->src_y_offset = gst_video_format_get_component_offset (format, 0, sw, sh);
	filter->src_u_offset = gst_video_format_get_component_offset (format, 1, sw, sh);
	filter->src_v_offset = gst_video_format_get_component_offset (format, 2, sw, sh);
#ifdef DEBUG
	g_print ("src_y_offset %d\n", filter->src_y_offset);
	g_print ("src_u_offset %d\n", filter->src_u_offset);
	g_print ("src_v_offset %d\n", filter->src_v_offset);
#endif

	filter->dest_y_offset = gst_video_format_get_component_offset (format, 0, dw, dh);
	filter->dest_u_offset = gst_video_format_get_component_offset (format, 1, dw, dh);
	filter->dest_v_offset = gst_video_format_get_component_offset (format, 2, dw, dh);
#ifdef DEBUG
	g_print ("dest_y_offset %d\n", filter->dest_y_offset);
	g_print ("dest_u_offset %d\n", filter->dest_u_offset);
	g_print ("dest_v_offset %d\n", filter->dest_v_offset);
#endif

	filter->src_y_width = gst_video_format_get_component_width (format, 0, sw);
	filter->src_u_width = gst_video_format_get_component_width (format, 1, sw);
	filter->src_v_width = gst_video_format_get_component_width (format, 2, sw);
#ifdef DEBUG
	g_print ("src_y_width %d\n", filter->src_y_width);
	g_print ("src_u_width %d\n", filter->src_u_width);
	g_print ("src_v_width %d\n", filter->src_v_width);
#endif

	filter->dest_y_width = gst_video_format_get_component_width (format, 0, dw);
	filter->dest_u_width = gst_video_format_get_component_width (format, 1, dw);
	filter->dest_v_width = gst_video_format_get_component_width (format, 2, dw);
#ifdef DEBUG
	g_print ("dest_y_width %d\n", filter->dest_y_width);
	g_print ("dest_u_width %d\n", filter->dest_u_width);
	g_print ("dest_v_width %d\n", filter->dest_v_width);
#endif

	filter->src_y_height = gst_video_format_get_component_height (format, 0, sh);
	filter->src_u_height = gst_video_format_get_component_height (format, 1, sh);
	filter->src_v_height = gst_video_format_get_component_height (format, 2, sh);
#ifdef DEBUG
	g_print ("src_y_height %d\n", filter->src_y_height);
	g_print ("src_u_height %d\n", filter->src_u_height);
	g_print ("src_v_height %d\n", filter->src_v_height);
#endif

	filter->dest_y_height = gst_video_format_get_component_height (format, 0, dh);
	filter->dest_u_height = gst_video_format_get_component_height (format, 1, dh);
	filter->dest_v_height = gst_video_format_get_component_height (format, 2, dh);
#ifdef DEBUG
	g_print ("dest_y_height %d\n", filter->dest_y_height);
	g_print ("dest_u_height %d\n", filter->dest_u_height);
	g_print ("dest_v_height %d\n", filter->dest_v_height);
#endif
	if (filter->map_eq_y == NULL)
		filter->map_eq_y = malloc(filter->dest_y_height * filter->dest_y_width * sizeof(*filter->map_eq_y));
	if (filter->map_eq_y == NULL)
		g_print ("Unable to allocate map_eq_y\n");

	if (filter->map_eq_u == NULL)
		filter->map_eq_u = malloc(filter->dest_u_height * filter->dest_u_width * sizeof(*filter->map_eq_u));
	if (filter->map_eq_u == NULL)
		g_print ("Unable to allocate map_eq_u\n");

	if (filter->map_eq_v == NULL)
		filter->map_eq_v = malloc(filter->dest_v_height * filter->dest_v_width * sizeof(*filter->map_eq_v));
	if (filter->map_eq_v == NULL)
		g_print ("Unable to allocate map_eq_v\n");

	//radius = sqrt( powl(filter->dest_y_height, 2) + powl(filter->dest_y_width, 2) ) / 2;
	for (y = 0; y < filter->dest_y_height; y++) {
		for (x = 0; x < filter->dest_y_width; x++) {
			//normalized index
			long double ni = (long double)(y+1)*2/(filter->dest_y_height) -1 + Cy;
			long double nj = (long double)(x+1)*2/(filter->dest_y_width) -1 + Cx;
			int nx;
			int ny;

			long double sni = sqr(ni);
			long double snj = sqr(nj);
			long double r = sqrt( sqr(ni) + sqr(nj) );
			long double plot_coffX = 1 + k1*powl(r,2) + k2*powl(r,4) + k3*powl(r,6);
			long double plot_coffY = 1 + (k1/s)*powl(r,2) + (k2/s)*powl(r,4) + (k3/s)*powl(r,6);

			if(plot_coffY != plot_coffY)
				break;

			ni = ni * plot_coffX;
			nj = nj * plot_coffY;
			ni = ni*(1+p1*sni);
			nj = nj*(1+p2*snj);


			nx = round( (nj + 1) * (filter->dest_y_width/2)) - 1;
			ny = round( (ni + 1) * (filter->dest_y_height/2)) - 1;

			if( nx < 0 || ny < 0 || nx > filter->src_y_stride || ny > filter->dest_y_height )  {
				filter->map_eq_y[y * filter->dest_y_stride + x] = 0;
				continue;
			}

			filter->map_eq_y[y * filter->dest_y_stride + x] = ny * filter->src_y_stride + nx;
		}
	}
	for (y = 0; y < filter->dest_u_height; y++) {
		for (x = 0; x < filter->dest_u_width; x++) {
			//normalized index
			long double ni = (long double)(y+1)*2/(filter->dest_u_height) -1 + Cy;
			long double nj = (long double)(x+1)*2/(filter->dest_u_width) -1 + Cx;
			int nx;
			int ny;

			long double sni = sqr(ni);
			long double snj = sqr(nj);
			long double r = sqrt( sqr(ni) + sqr(nj) );
			long double plot_coffX = 1 + k1*powl(r,2) + k2*powl(r,4) + k3*powl(r,6);
			long double plot_coffY = 1 + (k1/s)*powl(r,2) + (k2/s)*powl(r,4) + (k3/s)*powl(r,6);

			if(plot_coffY != plot_coffY)
				break;

			ni = ni * plot_coffX;
			nj = nj * plot_coffY;
			ni = ni*(1+p1*sni);
			nj = nj*(1+p2*snj);


			nx = round( (nj + 1) * (filter->dest_u_width/2)) - 1;
			ny = round( (ni + 1) * (filter->dest_u_height/2)) - 1;

			if( nx < 0 || ny < 0 || nx > filter->src_u_stride || ny > filter->dest_u_height )  {
				filter->map_eq_u[y * filter->dest_u_stride + x] = 0;
				continue;
			}

			filter->map_eq_u[y * filter->dest_u_stride + x] = ny * filter->src_u_stride + nx;
		}
	}
	for (y = 0; y < filter->dest_v_height; y++) {
		for (x = 0; x < filter->dest_v_width; x++) {
			//normalized index
			long double ni = (long double)(y+1)*2/(filter->dest_v_height) -1 + Cy;
			long double nj = (long double)(x+1)*2/(filter->dest_v_width) -1 + Cx;
			int nx;
			int ny;

			long double sni = sqr(ni);
			long double snj = sqr(nj);
			long double r = sqrt( sqr(ni) + sqr(nj) );
			long double plot_coffX = 1 + k1*powl(r,2) + k2*powl(r,4) + k3*powl(r,6);
			long double plot_coffY = 1 + (k1/s)*powl(r,2) + (k2/s)*powl(r,4) + (k3/s)*powl(r,6);

			if(plot_coffY != plot_coffY)
				break;

			ni = ni * plot_coffX;
			nj = nj * plot_coffY;
			ni = ni*(1+p1*sni);
			nj = nj*(1+p2*snj);


			nx = round( (nj + 1) * (filter->dest_v_width/2)) - 1;
			ny = round( (ni + 1) * (filter->dest_v_height/2)) - 1;

			if( nx < 0 || ny < 0 || nx > filter->src_v_stride || ny > filter->dest_v_height )  {
				filter->map_eq_v[y * filter->dest_v_stride + x] = 0;
				continue;
			}

			filter->map_eq_v[y * filter->dest_v_stride + x] = ny * filter->src_v_stride + nx;
		}
	}
}
static gboolean gst_lense_correction_set_caps (GstBaseTransform *object, GstCaps * incaps, GstCaps * outcaps)
{
	GstVideoFormat out_format;
	GstLenseCorrection *filter = GST_LENSE_CORRECTION (object);
	int ret = 0;
	ret = gst_video_format_parse_caps (incaps, &filter->in_format, &filter->from_width, &filter->from_height);
	if (ret != TRUE)
	{
		g_print ("Unable to get cap of input \n");
		return FALSE;
	}

	ret = gst_video_format_parse_caps (outcaps, &out_format, &filter->to_width, &filter->to_height);
	if(ret != TRUE)
	{
		g_print ("Unable to get cap of output\n");
		return FALSE;
	}

	if (filter->in_format != out_format)
	{
		g_print ("We dont conver format\n");
		return FALSE;
	}
	set_lense_eq_map(filter);
	g_print ("Here is the cap of element\n");
	return TRUE;
}
/* GstBaseTransform vmethod implementations */
static GstFlowReturn gst_lense_correction_transform(GstBaseTransform *object, GstBuffer *inbuf,
                                 GstBuffer *outbuf)
{
	int frame_rate,value;
	const guint8 *src = GST_BUFFER_DATA (inbuf);
	guint8* dest = GST_BUFFER_DATA (outbuf);
	GstLenseCorrection *filter = GST_LENSE_CORRECTION (object);
	gint x, y;
	guint8 const *s;
	guint8 *d;
	s = src + filter->src_y_offset;
	d = dest + filter->dest_y_offset;

	for (y = 0; y < filter->dest_y_height; y++) {
		for (x = 0; x < filter->dest_y_width; x++) {
			d[y * filter->dest_y_stride + x] = s[filter->map_eq_y[y * filter->dest_y_stride + x]];
		}
	}

	s = src + filter->src_u_offset;
	d = dest + filter->dest_u_offset;
	for (y = 0; y < filter->dest_u_height; y++) {
		for (x = 0; x < filter->dest_u_width; x++) {
			d[y * filter->dest_u_stride + x] = s[filter->map_eq_u[y * filter->dest_u_stride + x]];;
		}
	}

	s = src + filter->src_v_offset;
	d = dest + filter->dest_v_offset;
	for (y = 0; y < filter->dest_v_height; y++) {
		for (x = 0; x < filter->dest_v_width; x++) {
			d[y * filter->dest_v_stride + x] = s[filter->map_eq_v[y * filter->dest_v_stride + x]];;
		}
	}
	if(filter->frame_count%FRAME_OFFSET == 1)
	{
		filter->prev=(int)time(NULL);
	}
	else if(filter->frame_count%FRAME_OFFSET == 0)
	{
		filter->current=(int)time(NULL);
		value=filter->current-filter->prev;
		frame_rate=FRAME_OFFSET/value;
		g_print("fps %d\r",frame_rate);
	}
	filter->frame_count++;
	return GST_FLOW_OK;
}

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean lensecorrection_init (GstPlugin * plugin)
{
	/* initialize gst controller library */
	gst_controller_init(NULL, NULL);

	return gst_element_register (plugin, "lensecorrection", GST_RANK_NONE,
	GST_TYPE_LENSE_CORRECTION);
}

/* gstreamer looks for this structure to register plugins
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "lensecorrection",
    "lense Distortion Corrector",
    lensecorrection_init,
    VERSION,
    "LGPL",
    "GStreamer",
    "http://openfeather.com/"
)

/* initialize the plugin's class */
static void
gst_lense_correction_class_init (GstLenseCorrectionClass * klass)
{
	GObjectClass *gobject_class;
	GstBaseTransformClass *lense_class = (GstBaseTransformClass *) klass;

	gobject_class = (GObjectClass *) klass;
	gobject_class->set_property = gst_lense_correction_set_property;
	gobject_class->get_property = gst_lense_correction_get_property;
	lense_class->set_caps = GST_DEBUG_FUNCPTR (gst_lense_correction_set_caps);
/*
	g_object_class_install_property (gobject_class, PROP_METHOD,
		g_param_spec_enum ("method", "method", "method",
		GST_TYPE_VIDEO_FLIP_METHOD, PROP_METHOD_DEFAULT,
		GST_PARAM_CONTROLLABLE | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
*/
	//GST_BASE_TRANSFORM_CLASS (klass)->transform_ip =  GST_DEBUG_FUNCPTR (gst_lense_correction_transform_ip);
	GST_BASE_TRANSFORM_CLASS (klass)->transform = GST_DEBUG_FUNCPTR (gst_lense_correction_transform);
}
