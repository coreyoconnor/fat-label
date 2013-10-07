#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <gconf/gconf-client.h>

#include <hildon/hildon.h>
#include <libhildondesktop/libhildondesktop.h>

#include "lib_bg90_fat_label.h"

#define FAT_LABEL_GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE (obj, FAT_LABEL_TYPE_HOME_PLUGIN, FatLabelPrivate))


const gchar* conf_base_dir = "/apps/blimpsgo90-fat-label/";
const gchar* conf_text_key = "text";
const gchar* conf_size_key = "size";
const gchar* conf_color_key = "color";

HD_DEFINE_PLUGIN_MODULE (FatLabel, fat_label, HD_TYPE_HOME_PLUGIN_ITEM);

typedef struct 
{
    float text_size;
    gchar* text;
    GtkWidget* drawing_area;
    double r;
    double g;
    double b;
    GConfClient* gconf;
    guint change_notify_cnxn;
} FatLabelPrivate;

static gchar* fat_label_conf_dir( GConfClient* gconf, gchar* unique_ID )
{
    return g_strdup_printf( "%s%s", conf_base_dir, unique_ID );
}

static gchar* fat_label_conf_text( GConfClient* gconf, gchar* unique_ID )
{
    gchar* conf_dir = fat_label_conf_dir( gconf, unique_ID );
    
    gchar* conf_text = g_strdup_printf( "%s/%s", conf_dir, conf_text_key );

    g_free(conf_dir);
    
    return conf_text;
}

static gboolean expose_event_callback ( GtkWidget *widget
                                      , GdkEventExpose *event
                                      , FatLabelPrivate* private_data
                                      )
{
    cairo_t* cr = gdk_cairo_create (GDK_DRAWABLE (widget->window));
    gdk_cairo_region (cr, event->region);
    cairo_clip (cr);

    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.0);
    cairo_paint (cr);

    cairo_select_font_face( cr, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD );
    cairo_set_source_rgba ( cr
                          , private_data->r
                          , private_data->g
                          , private_data->b
                          , 1.0
                          );

    cairo_move_to(cr, 0.0, HILDON_ICON_PIXEL_SIZE_FINGER + private_data->text_size);
    cairo_set_font_size(cr, private_data->text_size);
    cairo_show_text( cr, private_data->text );

    cairo_destroy (cr);

    return FALSE;
}

static void fat_label_request_size( FatLabel* fat_label )
{
    cairo_t* cr = gdk_cairo_create (GDK_DRAWABLE ( GTK_WIDGET( fat_label )->window ));
    FatLabelPrivate* private_data = FAT_LABEL_GET_PRIVATE(fat_label);
    cairo_text_extents_t text_extents;

    cairo_select_font_face( cr, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD );
    cairo_set_font_size(cr, private_data->text_size);
    cairo_text_extents( cr
                      , private_data->text
                      , &text_extents
                      );

    double w = text_extents.width + HILDON_ICON_PIXEL_SIZE_FINGER;
    double h = 2.0*HILDON_ICON_PIXEL_SIZE_FINGER + text_extents.height + 5.0;

    gtk_widget_set_size_request( private_data->drawing_area 
                               , w
                               , h 
                               );

    cairo_destroy( cr );
}

static void fat_label_get_gdk_color( FatLabel* fat_label, GdkColor* c )
{
    FatLabelPrivate* private_data = FAT_LABEL_GET_PRIVATE(fat_label);
    c->red = (guint16) ( private_data->r * G_MAXUSHORT );
    c->green = (guint16) ( private_data->g * G_MAXUSHORT );
    c->blue = (guint16) ( private_data->b * G_MAXUSHORT );
}

static void fat_label_set_gdk_color( FatLabel* fat_label, GdkColor* c )
{
    FatLabelPrivate* private_data = FAT_LABEL_GET_PRIVATE(fat_label);
    private_data->r = (double)c->red / (double)G_MAXUSHORT;
    private_data->g = (double)c->green / (double)G_MAXUSHORT;
    private_data->b = (double)c->blue / (double)G_MAXUSHORT;
}

static void show_settings ( FatLabel* fat_label, FatLabelPrivate* private_data )
{
    GtkDialog* dialog = GTK_DIALOG( gtk_dialog_new_with_buttons( "FatLabel Settings"
                                                               , NULL
                                                               , GTK_DIALOG_MODAL
                                                               , GTK_STOCK_OK
                                                               , GTK_RESPONSE_OK
                                                               , NULL
                                                               )
                                  );
    GtkWidget* content_area = gtk_dialog_get_content_area( dialog );
    GtkEntry* label_text = GTK_ENTRY( hildon_entry_new(HILDON_SIZE_AUTO) );
    gtk_entry_set_text( label_text
                      , private_data->text 
                      );
    
    gtk_container_add( GTK_CONTAINER( content_area ), GTK_WIDGET( label_text ) );

    GdkColor c;
    fat_label_get_gdk_color( fat_label, &c );
    HildonColorButton* color_button = HILDON_COLOR_BUTTON( hildon_color_button_new_with_color(&c) );
    gtk_container_add( GTK_CONTAINER( content_area ), GTK_WIDGET(color_button) );

    gtk_widget_show_all ( content_area );

    gint response = gtk_dialog_run( dialog );
    if ( response == GTK_RESPONSE_OK )
    {
        hildon_color_button_get_color( color_button, &c );
        fat_label_set_gdk_color( fat_label, &c );

        gchar* new_text = gtk_editable_get_chars( GTK_EDITABLE( label_text )
                                                , 0
                                                , -1
                                                );
        
        gchar* unique_ID = hd_plugin_item_get_plugin_id( HD_PLUGIN_ITEM( fat_label ) );
        gchar* text_path = fat_label_conf_text( private_data->gconf, unique_ID );
        
        gconf_client_set_string( private_data->gconf
                               , text_path
                               , new_text
                               , NULL
                               );

        g_free( text_path );
        g_free( new_text );
        g_free( unique_ID );
    }
    gtk_widget_destroy( GTK_WIDGET( dialog ) );
}

static void fat_label_init (FatLabel* fat_label)
{
    FAT_LABEL_GET_PRIVATE(fat_label)->text_size = 40.0;
    FAT_LABEL_GET_PRIVATE(fat_label)->text = (gchar*) strdup("gconf failure?");
    FAT_LABEL_GET_PRIVATE(fat_label)->r = 1.0;
    FAT_LABEL_GET_PRIVATE(fat_label)->g = 1.0;
    FAT_LABEL_GET_PRIVATE(fat_label)->b = 1.0;

    hd_home_plugin_item_set_settings(&fat_label->parent, TRUE);
    g_signal_connect ( &fat_label->parent
                     , "show-settings"
                     , G_CALLBACK (show_settings)
                     , (gpointer) FAT_LABEL_GET_PRIVATE(fat_label)
                     );

    GtkWidget *drawing_area = gtk_drawing_area_new ();
    FAT_LABEL_GET_PRIVATE(fat_label)->drawing_area = drawing_area;

    g_signal_connect ( G_OBJECT (drawing_area)
                     , "expose_event"
                     , G_CALLBACK (expose_event_callback)
                     , (gpointer) FAT_LABEL_GET_PRIVATE(fat_label)
                     );

    gtk_widget_show_all (drawing_area);
    gtk_container_add (GTK_CONTAINER (fat_label), drawing_area);
}

static void fat_label_conf_changed( GConfClient* gconf
                                  , guint cnxn_id
                                  , GConfEntry* entry
                                  , FatLabel* fat_label
                                  )
{
    printf("fat_label_conf_changed: %s\n", entry->key);

    g_free( FAT_LABEL_GET_PRIVATE(fat_label)->text );
    FAT_LABEL_GET_PRIVATE(fat_label)->text = g_strdup( gconf_value_get_string(entry->value) );

    fat_label_request_size( fat_label );
}

static void fat_label_widget_realize (GtkWidget* fat_label)
{
    GdkScreen *screen = gtk_widget_get_screen (fat_label);
    gtk_widget_set_colormap (fat_label, gdk_screen_get_rgba_colormap (screen));
    gtk_widget_set_app_paintable (fat_label, TRUE);

    GTK_WIDGET_CLASS (fat_label_parent_class)->realize (fat_label);

    gchar* unique_ID = hd_plugin_item_get_plugin_id( HD_PLUGIN_ITEM( fat_label ) );
    printf("fat-label-ID: %s\n", unique_ID);

    FatLabelPrivate* private_data = FAT_LABEL_GET_PRIVATE(fat_label);
    private_data->gconf = gconf_client_get_default();

    gchar* conf_dir = fat_label_conf_dir( private_data->gconf, unique_ID );
    gconf_client_add_dir( private_data->gconf, conf_dir, GCONF_CLIENT_PRELOAD_NONE, NULL );

    // register for change notifications. *all changes* go through gconf.
    private_data->change_notify_cnxn = gconf_client_notify_add( private_data->gconf
                                                              , conf_dir
                                                              , ( GConfClientNotifyFunc ) &fat_label_conf_changed
                                                              , ( gpointer) FAT_LABEL( fat_label )
                                                              , NULL
                                                              , NULL
                                                              );
    g_free( conf_dir );

    // Now load the settings from gconf.
    // If not already configured the default values will be used.
    gchar* text_path = fat_label_conf_text( private_data->gconf, unique_ID );
    gchar* text = gconf_client_get_string( private_data->gconf, text_path, NULL );

    g_free( text_path );

    if( NULL == text )
        text = g_strdup( "Change me..." );

    g_free( private_data->text );
    private_data->text = text;

    g_free(unique_ID);
    fat_label_request_size( FAT_LABEL(fat_label) );
}

static void fat_label_finalize( GObject* fat_label )
{
    FatLabelPrivate* private_data = FAT_LABEL_GET_PRIVATE(fat_label);

    gconf_client_notify_remove( private_data->gconf, private_data->change_notify_cnxn );
    g_free( private_data->text );
    g_object_unref( G_OBJECT( private_data->gconf ) );

    G_OBJECT_CLASS(fat_label_parent_class)->finalize ( fat_label );
}

static void fat_label_class_init (FatLabelClass * klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  widget_class->realize = &fat_label_widget_realize;
  G_OBJECT_CLASS(klass)->finalize = &fat_label_finalize;

  g_type_class_add_private (klass, sizeof (FatLabelPrivate));
}

static void fat_label_class_finalize (FatLabelClass * klass)
{
}

