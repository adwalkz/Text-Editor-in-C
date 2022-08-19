#include<gtk/gtk.h>

//_________________________________________________________________________________________________

GtkWidget *window;
GtkWidget *workArea;

GtkWidget *nb;
GtkWidget *textArea[100];
GtkWidget *scrlBox[100];

GtkTextBuffer *buf;
GtkTextIter head, tail;

GtkWidget *hbar;
GtkWidget *bOpen, *bNew, *bClose, *bSave;

gint width, height;
int ntab = 0;
int p = -1;
char *fname = "*UnsavedFile";

//_________________________________________________________________________________________________

/* This helping function 
of the EDITOR will 
resize widget childs*/
void resize()
{
	gtk_window_get_size(GTK_WINDOW(window), &width, &height);
	gtk_widget_set_size_request(hbar, width-1,10);
	
	gtk_widget_set_size_request(scrlBox[ntab], width-3, height-80);
	
	gtk_widget_queue_draw(window);
	gtk_widget_show_all(window);
}

/* This helping function 
of the EDITOR will 
add new tab*/
void gtk_add_page_new()
{
	if(p++ == -1)
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(nb), FALSE);
	else
	{
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(nb), TRUE);
		gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(nb), scrlBox[ntab], fname);
	}

	textArea[ntab] = gtk_text_view_new();

	buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textArea[ntab]));
	//g_signal_connect(buf, "changed", G_CALLBACK(__switch__), bSave);
	gtk_text_buffer_set_text(buf, "", gtk_text_buffer_get_char_count(buf));

	scrlBox[ntab] = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrlBox[ntab]), textArea[ntab]);

	gtk_notebook_append_page(GTK_NOTEBOOK(nb), scrlBox[ntab], NULL);
	//gtk_notebook_set_action_widget(GTK_NOTEBOOK(nb), bClose,GTK_PACK_END);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(nb), scrlBox[ntab], TRUE);
	gtk_widget_grab_focus(textArea[ntab]);
	gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(nb), scrlBox[ntab], "*UnsavedFile");
}

/* This function
of the EDITOR will 
save the user's text*/
void __save__(GtkWidget *widget, gpointer window)
{
	GtkWidget *FileDialog;
	GtkFileChooser *ch;
	
	FileDialog = gtk_file_chooser_dialog_new ("Save A New File", GTK_WINDOW(window), 				GTK_FILE_CHOOSER_ACTION_SAVE, ("_Cancel"), GTK_RESPONSE_CANCEL, ("_Save"), 				GTK_RESPONSE_ACCEPT, NULL);
	
	ch = GTK_FILE_CHOOSER(FileDialog);
	
	gtk_file_chooser_set_do_overwrite_confirmation(ch, TRUE);
			
	gint res = gtk_dialog_run(GTK_DIALOG(FileDialog));
	
	if(res == GTK_RESPONSE_ACCEPT)
	{
		fname = gtk_file_chooser_get_filename(ch);
		const char *Text;
		char name[] = "";

		if(p == 0)
			gtk_window_set_title(GTK_WINDOW(window), fname);
		else
		{
			/*int i;
			for(i=0; fname[i]!='\0'; i++);
			for(i-=1; i>-1; i--)
				if(fname[i] == '/')
					break;
			int j = i+1;
			i = 0;
			for(; fname[j]!='\0'; j++)
			{
				strncat(&name, &fname[j], 1);
				i+=1;
			}
			name[i] = '\0';*/
			
			gtk_window_set_title(GTK_WINDOW(window), "ADitor: TextEditor");
			gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(nb), scrlBox[ntab], fname);
		}

		FILE *fp = fopen(fname, "w");
		
		if(fp == NULL)
			printf("NOT FOUND\n");
		else
		{		
			gtk_text_buffer_get_bounds(buf, &head, &tail);
			
			Text = gtk_text_buffer_get_text(buf, &head, &tail, FALSE);
			
			fprintf(fp, "%s",Text);
		}

		fclose(fp);
	}
	gtk_widget_destroy(FileDialog);
	//gtk_widget_set_sensitive(bClose, TRUE);
}

/* This function 
of the EDITOR will 
add new tab*/
void __add__(GtkWidget *widget, gpointer window)
{	
	p = gtk_notebook_get_current_page(GTK_NOTEBOOK(nb));
	if(p == -1)
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(nb), FALSE);
	else
	{
		gtk_window_set_title(GTK_WINDOW(window), "ADitor: TextEditor");
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(nb), TRUE);
	}
		
	gtk_add_page_new();
	gtk_widget_show_all(window);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(nb), p);
	gtk_widget_grab_focus(textArea[ntab]);
	fname = "*UnsavedFile";
}

/* This function 
of the EDITOR will 
open the user's file*/
void __open__(GtkWidget *widget, gpointer window)
{
	GtkWidget *FileDialog;
	GtkFileChooser *ch;

	FileDialog = gtk_file_chooser_dialog_new ("Open Existing File", GTK_WINDOW(window), 				GTK_FILE_CHOOSER_ACTION_OPEN, ("_Cancel"), GTK_RESPONSE_CANCEL, ("_Open"), 				GTK_RESPONSE_ACCEPT, NULL);

	gint res = gtk_dialog_run(GTK_DIALOG(FileDialog));
	
	if(res == GTK_RESPONSE_ACCEPT)
	{
		ch = GTK_FILE_CHOOSER(FileDialog);
		fname = gtk_file_chooser_get_filename(ch);

		if(p == 0)
			gtk_window_set_title(GTK_WINDOW(window), fname);
		else
		{
			gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(nb), scrlBox[ntab], fname);
			gtk_window_set_title(GTK_WINDOW(window), "ADitor: TextEditor");
		}
		
		FILE *fp = fopen(fname, "r");
		
		if(fp == NULL)
			printf("NOT FOUND\n");
		else
		{
			fseek(fp,0,SEEK_END);
			int size = ftell(fp);
			fseek(fp,0,SEEK_SET);
			char *t = malloc(size);
			int ret = fread(t, 1, size, fp);
			gtk_text_buffer_set_text(buf, t, ret);
			free(t);
		}
		fclose(fp);
	}
	gtk_widget_destroy(FileDialog);
	//gtk_widget_set_sensitive(bClose, TRUE);
}

/* This function 
of the EDITOR will 
close the user's file*/
void __close__(GtkWidget *widget, gpointer window)
{
	gtk_window_set_title(GTK_WINDOW(window), "ADitor: TextEditor");
	if(gtk_text_buffer_get_char_count(buf)>0)
		gtk_text_buffer_set_text(buf, "", -1);
	fname = "*UnsavedFile";
	p = gtk_notebook_get_current_page(GTK_NOTEBOOK(nb));
	if(p > 0)
		gtk_notebook_remove_page(GTK_NOTEBOOK(nb), p);
	if(p == 0)
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(nb), FALSE);
	/*gtk_widget_set_sensitive(bSave, FALSE);
	gtk_widget_set_sensitive(bClose, FALSE);
	gtk_widget_set_sensitive(bOpen, TRUE);*/
}

//_________________________________________________________________________________________________

int main()
{
	gtk_init(NULL, NULL);
	GtkCssProvider *css;
//-------------------------------------------------------------------------------------------------
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Aditor: TextEditor");
	gtk_window_set_default_size(GTK_WINDOW (window), 600, 350);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(window), "configure-event", G_CALLBACK(resize), NULL);

	workArea = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window), workArea);
//-------------------------------------------------------------------------------------------------
	hbar = gtk_header_bar_new();
	gtk_fixed_put(GTK_FIXED(workArea), GTK_WIDGET(hbar), 0, 0);

	bOpen = gtk_button_new_with_label("Open");
	gtk_header_bar_pack_start(GTK_HEADER_BAR(hbar), bOpen);
	g_signal_connect(G_OBJECT(bOpen), "clicked", G_CALLBACK(__open__), window);

	bNew = gtk_button_new_with_label("+");
	gtk_header_bar_pack_start(GTK_HEADER_BAR(hbar), bNew);
	g_signal_connect(G_OBJECT(bNew), "clicked", G_CALLBACK(__add__), window);

	bClose = gtk_button_new_with_label("x");
	gtk_header_bar_pack_end(GTK_HEADER_BAR(hbar), bClose);
	g_signal_connect(G_OBJECT(bClose), "clicked", G_CALLBACK(__close__), window);

	bSave = gtk_button_new_with_label("Save");
	gtk_header_bar_pack_end(GTK_HEADER_BAR(hbar), bSave);
	g_signal_connect(G_OBJECT(bSave), "clicked", G_CALLBACK(__save__), window);
//-------------------------------------------------------------------------------------------------
	nb = gtk_notebook_new();
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(nb), GTK_POS_BOTTOM);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(nb), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(nb));
	gtk_fixed_put(GTK_FIXED(workArea), GTK_WIDGET(nb), 0, 40);
	gtk_add_page_new();
//-------------------------------------------------------------------------------------------------	
	css = gtk_css_provider_new();
	//gtk_css_provider_load_from_path(css, "style.css", NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css), 									GTK_STYLE_PROVIDER_PRIORITY_USER);
//-------------------------------------------------------------------------------------------------
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
