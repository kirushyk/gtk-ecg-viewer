public class MainWindow: Gtk.ApplicationWindow
{

	public MainWindow(Gtk.Application application, string? path)
	{
 		Object(application: application, title: "ECG Viewer");
#if !HAVE_GTK4
		window_position = Gtk.WindowPosition.CENTER;
#endif
		set_default_size(800, 600);
		set_icon_name("utilities-system-monitor");

		var box = new Gtk.Box(Gtk.Orientation.VERTICAL, 0);
		
#if !HAVE_GTK4
		var menu = new MainMenu();
		menu.quit_item_activated.connect(() =>
		{
			this.destroy();
		});
		// application.set_menubar(menu);
		box.pack_start(menu, false, true, 0);
#endif

		var monitor = new ECGMonitor();
		var scrollbar = new Gtk.Scrollbar(Gtk.Orientation.HORIZONTAL, null);
		if (path != null) {
			monitor.load(path);
#if !HAVE_GTK4
			scrollbar.set_range(0, monitor.get_max_shift());
#endif
		}
#if HAVE_GTK4
		box.append(monitor);
#else
		box.pack_start(monitor, true, true, 0);
#endif

#if !HAVE_GTK4
		menu.open_file_item_activated.connect((path) => 
		{
			monitor.load(path);
			scrollbar.set_range(0, monitor.get_max_shift());
		});
		menu.limb_leads_item_activated.connect(monitor.show_limb_leads);
		menu.augment_limb_leads_item_activated.connect(monitor.show_augment_limb_leads);
		menu.chest_leads_item_activated.connect(monitor.show_chest_leads);
#endif

#if !HAVE_GTK4
		scrollbar.value_changed.connect(() =>
		{
			monitor.set_ecg_shift((int)scrollbar.get_value());
        });
#endif

#if HAVE_GTK4
		box.append(scrollbar);
#else
		box.pack_start(scrollbar, true, true, 0);
#endif

#if !HAVE_GTK4
		this.key_press_event.connect((source, key) => 
		{
			int new_shift = monitor.get_ecg_shift();

			switch (key.keyval)
			{
			case Gdk.Key.Left:
			case Gdk.Key.leftarrow:
				new_shift -= 200;
				break;
			case Gdk.Key.Right:
			case Gdk.Key.rightarrow:
				new_shift += 200;
				break;
			case Gdk.Key.space:
				new_shift++;
				break;
			default:
				break;
			}
			
			monitor.set_ecg_shift(new_shift);
			scrollbar.set_value(new_shift);
			
			return true;
		});
#endif

#if HAVE_GTK4
		set_child(box);
#else
		add(box);
#endif
	}

}

