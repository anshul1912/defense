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

GST_DEBUG_CATEGORY_STATIC (gst_lense_correction_debug);
#define GST_CAT_DEFAULT gst_lense_correction_debug

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

static gboolean gst_lense_correction_set_caps (GstBaseTransform *object, GstCaps * incaps, GstCaps * outcaps)
{
	GstVideoFormat in_format, out_format;
	GstLenseCorrection *filter = GST_LENSE_CORRECTION (object);
	int ret = 0;
	ret = gst_video_format_parse_caps (incaps, &in_format, &filter->from_width, &filter->from_height);
	if (ret != 0)
	{
		g_print ("Unable to get cap of input %d\n", ret);
		return FALSE;
	}

	ret = gst_video_format_parse_caps (outcaps, &out_format, &filter->to_width, &filter->to_height);
	if(ret != 0)
	{
		g_print ("Unable to get cap of output\n");
		return FALSE;
	}

	if (in_format != out_format)
	{
		g_print ("We dont conver format\n");
		return FALSE;
	}
	g_print ("Here is the cap of element\n");
	return TRUE;
}
/* GstBaseTransform vmethod implementations */
static GstFlowReturn gst_lense_correction_transform(GstBaseTransform *trans, GstBuffer *inbuf,
                                 GstBuffer *outbuf)
{
	const guint8 *src = GST_BUFFER_DATA (inbuf);
	guint8* dest = GST_BUFFER_DATA (outbuf);

	g_print ("Got In and out\n");
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

