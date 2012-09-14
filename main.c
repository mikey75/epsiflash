/*
 * This file is a part of the EpsiFlash Copyright (c) 2000-2007 Michal
 * Szwaczko, WireLabs Systems contact: mikey@wirelabs.net 
 */

#include "main.h"

int
main (int argc, char *argv[])
{

  int i, k;
  PangoFontDescription *font_desc;

  /* initialise the burner  */
  hardware_init ();

  gtk_init (&argc, &argv);

  /* main window  */
  mainwindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size (GTK_WINDOW (mainwindow), 600, 70);
  gtk_window_set_resizable (GTK_WINDOW (mainwindow), FALSE);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (mainwindow), vbox);

  /* progressbar */
  progressbar = gtk_progress_bar_new ();
  font_desc = pango_font_description_from_string ("Monospace");
  gtk_widget_modify_font (progressbar, font_desc);
  gtk_widget_show (progressbar);
  gtk_box_pack_start (GTK_BOX (vbox), progressbar, FALSE, FALSE, 0);

  msg_ready ();

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);

  /* chip label */
  chiplabel = gtk_label_new ("<b>Chip</b>");
  gtk_widget_show (chiplabel);
  gtk_box_pack_start (GTK_BOX (hbox), chiplabel, FALSE, FALSE, 0);
  gtk_label_set_use_markup (GTK_LABEL (chiplabel), TRUE);

  /* chip select combobox   */
  chipcombo = gtk_combo_box_new_text ();
  gtk_widget_show (chipcombo);
  gtk_box_pack_start (GTK_BOX (hbox), chipcombo, TRUE, TRUE, 0);

  /* buttons */
  fileopenbutton = gtk_button_new_with_mnemonic ("Load");
  gtk_widget_show (fileopenbutton);
  gtk_box_pack_start (GTK_BOX (hbox), fileopenbutton, FALSE, FALSE, 0);
  filesavebutton = gtk_button_new_with_mnemonic ("Save");
  gtk_widget_show (filesavebutton);
  gtk_box_pack_start (GTK_BOX (hbox), filesavebutton, FALSE, FALSE, 0);
  readchipbutton = gtk_button_new_with_mnemonic ("Read");
  gtk_widget_show (readchipbutton);
  gtk_box_pack_start (GTK_BOX (hbox), readchipbutton, FALSE, FALSE, 0);
  burnchipbutton = gtk_button_new_with_mnemonic ("Burn");
  gtk_widget_show (burnchipbutton);

  gtk_box_pack_start (GTK_BOX (hbox), burnchipbutton, FALSE, FALSE, 0);
  erasechipbutton = gtk_button_new_with_mnemonic ("Erase");
  gtk_widget_show (erasechipbutton);
  gtk_box_pack_start (GTK_BOX (hbox), erasechipbutton, FALSE, FALSE, 0);
  gtk_widget_show (erasechipbutton);


  viewbufferbutton = gtk_button_new_with_mnemonic ("View");
  gtk_widget_show (viewbufferbutton);
  gtk_box_pack_start (GTK_BOX (hbox), viewbufferbutton, FALSE, FALSE, 0);
  showhelpbutton = gtk_button_new_with_mnemonic ("Help");
  gtk_widget_show (showhelpbutton);
  gtk_box_pack_start (GTK_BOX (hbox), showhelpbutton, FALSE, FALSE, 0);

  /* status bar */
  statusbar = gtk_statusbar_new ();
  gtk_widget_show (statusbar);
  gtk_box_pack_start (GTK_BOX (vbox), statusbar, FALSE, FALSE, 0);

  /* signals for the UI  */
  g_signal_connect_swapped (G_OBJECT (mainwindow), "destroy",
			    GTK_SIGNAL_FUNC (quit), NULL);
  g_signal_connect_swapped (G_OBJECT (chipcombo), "changed",
			    GTK_SIGNAL_FUNC (chip_selected), NULL);
  g_signal_connect_swapped (G_OBJECT (fileopenbutton), "clicked",
			    GTK_SIGNAL_FUNC (file_open), NULL);
  g_signal_connect_swapped (G_OBJECT (filesavebutton), "clicked",
			    GTK_SIGNAL_FUNC (file_save), NULL);
  g_signal_connect_swapped (G_OBJECT (readchipbutton), "clicked",
			    GTK_SIGNAL_FUNC (read_chip), NULL);
  g_signal_connect_swapped (G_OBJECT (burnchipbutton), "clicked",
			    GTK_SIGNAL_FUNC (program_chip), NULL);
  g_signal_connect_swapped (G_OBJECT (erasechipbutton), "clicked",
			    GTK_SIGNAL_FUNC (erase_chip), NULL);
  g_signal_connect_swapped (G_OBJECT (viewbufferbutton), "clicked",
			    GTK_SIGNAL_FUNC (view_buffer), NULL);
  g_signal_connect_swapped (G_OBJECT (showhelpbutton), "clicked",
			    GTK_SIGNAL_FUNC (show_help), NULL);

  /* set up chiplist in the combobox   */
  for (i = 0; chips[i].name != NULL; i++)
    {
      gtk_combo_box_append_text (GTK_COMBO_BOX (chipcombo),
				 g_strdup_printf ("%s", chips[i].name));
    };

   /* set active probed chip and call chip_selected()  */
    probe_chip();


  /* ok, we're rollin... */
  gtk_widget_show (mainwindow);
  gtk_main ();

  return 0;
}


void
msg_ready (void)
{

  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), 0);
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progressbar), "Ready ...");
}

void
msg_error (void)
{

  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), 0);
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progressbar),
			     "ERROR ERROR ERROR");

}

void
quit (void)
{
  if (buffer != NULL)
    g_free (buffer);
  power_down ();
  gtk_main_quit ();
  exit (1);
}

void
view_buffer (void)
{

  viewerline_t *l;
  unsigned long int i;
  unsigned long int a;
  PangoFontDescription *font_desc;


  bufferwindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (bufferwindow), "Buffer");
  gtk_window_set_default_size (GTK_WINDOW (bufferwindow), 680, 600);
  gtk_widget_show (bufferwindow);

  bufferscrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (bufferscrolledwindow);
  gtk_container_add (GTK_CONTAINER (bufferwindow), bufferscrolledwindow);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW
				       (bufferscrolledwindow), GTK_SHADOW_IN);

  bufferclist = gtk_clist_new (3);

  font_desc = pango_font_description_from_string ("Monospace");
  gtk_widget_modify_font (bufferclist, font_desc);
  pango_font_description_free (font_desc);


  gtk_widget_show (bufferclist);
  gtk_container_add (GTK_CONTAINER (bufferscrolledwindow), bufferclist);
  gtk_clist_set_column_width (GTK_CLIST (bufferclist), 0, 106);
  gtk_clist_set_column_width (GTK_CLIST (bufferclist), 1, 356);
  gtk_clist_set_column_width (GTK_CLIST (bufferclist), 2, 80);
  gtk_clist_column_titles_show (GTK_CLIST (bufferclist));

  adrlabel = gtk_label_new ("Address");
  gtk_widget_show (adrlabel);
  gtk_clist_set_column_widget (GTK_CLIST (bufferclist), 0, adrlabel);
  hexlabel = gtk_label_new ("Hex");
  gtk_widget_show (hexlabel);
  gtk_clist_set_column_widget (GTK_CLIST (bufferclist), 1, hexlabel);
  asciilabel = gtk_label_new ("ASCII");
  gtk_widget_show (asciilabel);
  gtk_clist_set_column_widget (GTK_CLIST (bufferclist), 2, asciilabel);

  l = g_malloc ((buffersize / 16) * sizeof (viewerline_t));

  a = 0;

  gtk_clist_clear (GTK_CLIST (bufferclist));
  gtk_clist_freeze (GTK_CLIST (bufferclist));

  for (i = 0; i < buffersize / 16; i++)
    {

      l[i].adres = g_strdup_printf ("%08lX", a);
      l[i].hexy =
	g_strdup_printf
	("%02X %02X %02X %02X %02X %02X %02X %02X   %02X %02X %02X %02X %02X %02X %02X %02X",
	 buffer[a], buffer[a + 1], buffer[a + 2], buffer[a + 3],
	 buffer[a + 4], buffer[a + 5], buffer[a + 6], buffer[a + 7],
	 buffer[a + 8], buffer[a + 9], buffer[a + 10], buffer[a + 11],
	 buffer[a + 12], buffer[a + 13], buffer[a + 14], buffer[a + 15]);

      l[i].ascii = g_strdup_printf ("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				    !isprint (buffer[a]) ? 0x2e :
				    buffer[a],
				    !isprint (buffer[a + 1]) ? 0x2e :
				    buffer[a + 1],
				    !isprint (buffer[a + 2]) ? 0x2e :
				    buffer[a + 2],
				    !isprint (buffer[a + 3]) ? 0x2e :
				    buffer[a + 3],
				    !isprint (buffer[a + 4]) ? 0x2e :
				    buffer[a + 4],
				    !isprint (buffer[a + 5]) ? 0x2e :
				    buffer[a + 5],
				    !isprint (buffer[a + 6]) ? 0x2e :
				    buffer[a + 6],
				    !isprint (buffer[a + 7]) ? 0x2e :
				    buffer[a + 7],
				    !isprint (buffer[a + 8]) ? 0x2e :
				    buffer[a + 8],
				    !isprint (buffer[a + 9]) ? 0x2e :
				    buffer[a + 9],
				    !isprint (buffer[a + 10]) ? 0x2e :
				    buffer[a + 10],
				    !isprint (buffer[a + 11]) ? 0x2e :
				    buffer[a + 11],
				    !isprint (buffer[a + 12]) ? 0x2e :
				    buffer[a + 12],
				    !isprint (buffer[a + 13]) ? 0x2e :
				    buffer[a + 13],
				    !isprint (buffer[a + 14]) ? 0x2e :
				    buffer[a + 14],
				    !isprint (buffer[a + 15]) ? 0x2e :
				    buffer[a + 15]);

      gtk_clist_insert (GTK_CLIST (bufferclist), i, (gpointer) & l[i]);

      a = a + 16;
    }
  gtk_clist_thaw (GTK_CLIST (bufferclist));

}

void
block_user_input (void)
{
  /*
   * block combobox selection 
   */
  g_object_set (GTK_OBJECT (chipcombo), "button-sensitivity",
		GTK_SENSITIVITY_OFF, NULL);
  /*
   * block buttons 
   */
  gtk_signal_handler_block_by_func (GTK_OBJECT (chipcombo),
				    G_CALLBACK (chip_selected), NULL);
  gtk_signal_handler_block_by_func (GTK_OBJECT (fileopenbutton),
				    G_CALLBACK (file_open), NULL);
  gtk_signal_handler_block_by_func (GTK_OBJECT (filesavebutton),
				    G_CALLBACK (file_save), NULL);
  gtk_signal_handler_block_by_func (GTK_OBJECT (readchipbutton),
				    G_CALLBACK (read_chip), NULL);
  gtk_signal_handler_block_by_func (GTK_OBJECT (erasechipbutton),
				    G_CALLBACK (erase_chip), NULL);

  gtk_signal_handler_block_by_func (GTK_OBJECT (viewbufferbutton),
				    G_CALLBACK (view_buffer), NULL);
  gtk_signal_handler_block_by_func (GTK_OBJECT (showhelpbutton),
				    G_CALLBACK (show_help), NULL);
  gtk_signal_handler_block_by_func (GTK_OBJECT (burnchipbutton),
				    G_CALLBACK (program_chip), NULL);

}

void
unblock_user_input (void)
{
  /*
   * unblock combobox selection 
   */
  g_object_set (GTK_OBJECT (chipcombo), "button-sensitivity",
		GTK_SENSITIVITY_ON, NULL);
  /*
   * unblock buttons 
   */
  gtk_signal_handler_unblock_by_func (GTK_OBJECT (chipcombo),
				      G_CALLBACK (chip_selected), NULL);
  gtk_signal_handler_unblock_by_func (GTK_OBJECT (fileopenbutton),
				      G_CALLBACK (file_open), NULL);
  gtk_signal_handler_unblock_by_func (GTK_OBJECT (filesavebutton),
				      G_CALLBACK (file_save), NULL);
  gtk_signal_handler_unblock_by_func (GTK_OBJECT (readchipbutton),
				      G_CALLBACK (read_chip), NULL);
  gtk_signal_handler_unblock_by_func (GTK_OBJECT (erasechipbutton),
				      G_CALLBACK (erase_chip), NULL);

  gtk_signal_handler_unblock_by_func (GTK_OBJECT (viewbufferbutton),
				      G_CALLBACK (view_buffer), NULL);
  gtk_signal_handler_unblock_by_func (GTK_OBJECT (showhelpbutton),
				      G_CALLBACK (show_help), NULL);
  gtk_signal_handler_unblock_by_func (GTK_OBJECT (burnchipbutton),
				      G_CALLBACK (program_chip), NULL);


}


void
update_statusbar (void)
{

  char *s;

  s = g_strdup_printf ("Buffer:%d kB Adapter:(%s)",
		       (unsigned int) buffersize / 1024,
		       chips[chipindex].adapter);
  gtk_statusbar_push (GTK_STATUSBAR (statusbar), 1, s);
  gtk_window_set_title (GTK_WINDOW (mainwindow),
			g_strdup_printf ("WireLabs EpsiFlash v0.1 - [%s]",
					 (fileinthebuffer !=
					  NULL) ? fileinthebuffer : "none"));


}


void
chip_selected (void)
{


  GtkWidget *dialog;

  if (bufferloaded == 1)
    {
      dialog = gtk_message_dialog_new ((gpointer) mainwindow,
				       GTK_DIALOG_DESTROY_WITH_PARENT,
				       GTK_MESSAGE_QUESTION,
				       GTK_BUTTONS_YES_NO,
				       "Changing chip will invalidate the buffer! Are you sure?");
      if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_NO)
	{
	  gtk_widget_destroy (dialog);

	  /*
	   * avoid recursive chip_selected signal 
	   */
	  gtk_signal_handler_block_by_func (GTK_OBJECT (chipcombo),
					    G_CALLBACK (chip_selected),
					    chipcombo);
	  gtk_combo_box_set_active (GTK_COMBO_BOX (chipcombo), chipindex);
	  gtk_signal_handler_unblock_by_func (GTK_OBJECT (chipcombo),
					      G_CALLBACK (chip_selected),
					      chipcombo);
	  return;
	}
      gtk_widget_destroy (dialog);
    }

  chipindex = gtk_combo_box_get_active (GTK_COMBO_BOX (chipcombo));

  buffersize = chips[chipindex].size;
  pagesize = chips[chipindex].pagesize;

  if (buffer != NULL)
    g_free (buffer);
  buffer = g_malloc (buffersize);
  memset (buffer, 0xFF, buffersize);
  fileinthebuffer = NULL;
  bufferloaded = 0;
  update_statusbar ();

}




void
file_open (void)
{

  GtkWidget *dialog;
  GtkWidget *msg;
  int fd;
  size_t fsize;

  dialog =
    gtk_file_chooser_dialog_new ("Open File", (gpointer) mainwindow,
				 GTK_FILE_CHOOSER_ACTION_OPEN,
				 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				 GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {

      char *filename;
      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

      fd = open (filename, O_RDONLY);

      if (fd < 0)
	{
	  msg =
	    gtk_message_dialog_new ((gpointer) mainwindow,
				    GTK_DIALOG_DESTROY_WITH_PARENT,
				    GTK_MESSAGE_ERROR,
				    GTK_BUTTONS_CLOSE,
				    "Error loading file '%s': %s",
				    filename, g_strerror (errno));
	  gtk_dialog_run (GTK_DIALOG (msg));
	  gtk_widget_destroy (msg);
	};

      if (buffer != NULL)
	g_free (buffer);
      buffer = g_malloc (buffersize);
      memset (buffer, 0xFF, buffersize);

      fsize = read (fd, buffer, buffersize);

      fileinthebuffer = g_strdup (g_basename (filename));
      update_statusbar ();
      bufferloaded = 1;
      g_free (filename);


    }
  gtk_widget_destroy (dialog);

}



void
file_save (void)
{

  GtkWidget *dialog;
  GtkWidget *msg;
  int fd;
  size_t f;

  dialog =
    gtk_file_chooser_dialog_new ("Save File", (gpointer) mainwindow,
				 GTK_FILE_CHOOSER_ACTION_SAVE,
				 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				 GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);

  gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER
						  (dialog), TRUE);
  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), ".");
  gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), "buffer.bin");

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
    {

      char *filename;
      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

      fd = open (filename, O_CREAT | O_WRONLY | O_TRUNC, S_IWUSR | S_IRUSR);

      if (fd < 0)
	{
	  msg =
	    gtk_message_dialog_new ((gpointer) mainwindow,
				    GTK_DIALOG_DESTROY_WITH_PARENT,
				    GTK_MESSAGE_ERROR,
				    GTK_BUTTONS_CLOSE,
				    "Error saving file '%s': %s",
				    filename, g_strerror (errno));
	  gtk_dialog_run (GTK_DIALOG (msg));
	  gtk_widget_destroy (msg);
	};

      f = write (fd, buffer, buffersize);
      g_free (filename);
    }
  gtk_widget_destroy (dialog);
}


void
show_progress (long unsigned int adr, const char *mode)
{

  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar),
				 (adr / (float) buffersize));
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (progressbar),
			     g_strdup_printf ("[%s] Address: 0x%08lX",
					      mode, adr));
  while (g_main_context_iteration (NULL, FALSE));
}

void
show_help (void)
{

  helpwindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (helpwindow), "Jumper help");

  gtk_widget_show (helpwindow);

  jumperimage = gtk_image_new_from_file ("zworki2.png");
  gtk_widget_show (jumperimage);

  gtk_container_add (GTK_CONTAINER (helpwindow), jumperimage);

}

void
read_chip (void)
{
  int r;

  block_user_input ();
  power_up ();
  r = chips[chipindex].read_func (buffersize, pagesize);
  bufferloaded = 1;
  power_down ();
  unblock_user_input ();
  msg_ready ();

}

void
erase_chip (void)
{

  int r, b;

  block_user_input ();
  power_up ();
  r = chips[chipindex].erase_func (buffersize, pagesize);
  b = generic_blank_check (buffersize, pagesize);
  power_down ();
  unblock_user_input ();
  if (r < 0 || b < 0)
    msg_error ();
  else
    msg_ready ();


}

void
program_chip (void)
{

  int r, b, v;
  block_user_input ();
  power_up ();
  chips[chipindex].erase_func (buffersize, pagesize);
  b = generic_blank_check (buffersize, pagesize);
  r = chips[chipindex].burn_func (buffersize, pagesize);
  v = generic_verify (buffersize, pagesize);
  power_down ();
  unblock_user_input ();
  if (r < 0 || b < 0 || v < 0)
    msg_error ();
  else
    msg_ready ();

}


int
probe_chip (void)
{

  int i;
  power_up ();
  for (i = 0; chips[i].name != NULL; i++)
    {
      chips[i].probe_func();
      if ((chips[i].id1 == id1) && (chips[i].id2 == id2))
	{
          gtk_combo_box_set_active (GTK_COMBO_BOX (chipcombo), i);
          chip_selected ();
	  power_down ();
	  return 1;
	};
    };
  gtk_combo_box_set_active (GTK_COMBO_BOX (chipcombo), i-1);
  power_down ();
  return -1;
}
