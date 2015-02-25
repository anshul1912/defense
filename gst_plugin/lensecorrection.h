/*
 * Copyright (C) 2015 Anshul Maheshwari er.anshul.maheshwari@gmail.com
 */

#ifndef __GST_LENSE_CORRECTION_H__
#define __GST_LENSE_CORRECTION_H__

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>

G_BEGIN_DECLS

#define GST_TYPE_LENSE_CORRECTION \
  (gst_lense_correction_get_type())
#define GST_LENSE_CORRECTION(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_LENSE_CORRECTION,GstLenseCorrection))
#define GST_LENSE_CORRECTION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_LENSE_CORRECTION,GstLenseCorrectionClass))
#define GST_IS_LENSE_CORRECTION(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_LENSE_CORRECTION))
#define GST_IS_LENSE_CORRECTION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_LENSE_CORRECTION))

typedef struct _GstLenseCorrection      GstLenseCorrection;
typedef struct _GstLenseCorrectionClass GstLenseCorrectionClass;

struct _GstLenseCorrection {
	GstBaseTransform element;
	gint from_width, from_height;
	gint to_width, to_height;
	GstVideoFormat in_format;
	gboolean silent;
	gint src_y_stride, src_u_stride, src_v_stride;
	gint src_y_offset, src_u_offset, src_v_offset;
	gint src_y_height, src_u_height, src_v_height;
	gint src_y_width, src_u_width, src_v_width;
	gint dest_y_stride, dest_u_stride, dest_v_stride;
	gint dest_y_offset, dest_u_offset, dest_v_offset;
	gint dest_y_height, dest_u_height, dest_v_height;
	gint dest_y_width, dest_u_width, dest_v_width;
	int *map_eq_y;
	int *map_eq_u;
	int *map_eq_v;
	int frame_count,prev,current;

};

struct _GstLenseCorrectionClass {
  GstBaseTransformClass parent_class;
};

GType gst_lense_correction_get_type (void);

G_END_DECLS

#endif /* __GST_LENSE_CORRECTION_H__ */
