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

  gboolean silent;
};

struct _GstLenseCorrectionClass {
  GstBaseTransformClass parent_class;
};

GType gst_lense_correction_get_type (void);

G_END_DECLS

#endif /* __GST_LENSE_CORRECTION_H__ */
