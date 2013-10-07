#ifndef BG90_FAT_LABEL_WIDGET_H
#define BG90_FAT_LABEL_WIDGET_H

#include <libhildondesktop/libhildondesktop.h>

G_BEGIN_DECLS

/* Common struct types declarations */
typedef struct _FatLabel FatLabel;
typedef struct _FatLabelClass FatLabelClass;

struct FatLabelPrivate;

/* Common macros */
#define FAT_LABEL_TYPE_HOME_PLUGIN            (fat_label_get_type ())
#define FAT_LABEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), FAT_LABEL_TYPE_HOME_PLUGIN, FatLabel))
#define FAT_LABEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  FAT_LABEL_TYPE_HOME_PLUGIN, FatLabelClass))
#define FAT_LABEL_IS_HOME_PLUGIN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), FAT_LABEL_TYPE_HOME_PLUGIN))
#define FAT_LABEL_IS_HOME_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  FAT_LABEL_TYPE_HOME_PLUGIN))
#define FAT_LABEL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  FAT_LABEL_TYPE_HOME_PLUGIN, FatLabelClass))

/* Instance struct */
struct _FatLabel
{
	HDHomePluginItem parent;
};

/* Class struct */
struct _FatLabelClass
{
	HDHomePluginItemClass parent_class;
};

GType  fat_label_get_type  (void);

G_END_DECLS

#endif // BG90_FAT_LABEL_WIDGET_H
