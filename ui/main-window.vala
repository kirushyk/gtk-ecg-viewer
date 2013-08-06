public class MainWindow: Gtk.ApplicationWindow
{

	public MainWindow(Gtk.Application application)
	{
 		Object (application: application, title: "ECG Viewer");
		border_width = 0;
		window_position = Gtk.WindowPosition.CENTER;
		set_default_size(800, 600);

		var box = new Gtk.Box(Gtk.Orientation.VERTICAL, 0);
		
		var menu = new MainMenu();
		menu.quit_item_activated.connect(() =>
		{
			this.destroy();
		});
		//application.set_menubar(menu);
		box.pack_start(menu, false, true, 0);

		var monitor = new ECGMonitor();

		var scrollbar = new Gtk.Scrollbar(Gtk.Orientation.HORIZONTAL, null);

		menu.open_file_item_activated.connect((path) => 
		{
			monitor.load(path);
			scrollbar.set_range(0, monitor.get_ecg_size());
		});
		menu.limb_leads_item_activated.connect(monitor.show_limb_leads);
		menu.augment_limb_leads_item_activated.connect(monitor.show_augment_limb_leads);
		menu.chest_leads_item_activated.connect(monitor.show_chest_leads);
		box.pack_start(monitor, true, true, 0);

		scrollbar.value_changed.connect(() =>
		{
			monitor.set_ecg_shift((int)scrollbar.get_value());
        	});
		box.pack_start(scrollbar, false, true, 0);
		this.key_press_event.connect(() =>
		{
			int new_shift = monitor.get_ecg_shift();
			new_shift++;
			monitor.set_ecg_shift(new_shift);
			return true;
		});

		add(box);
	}

}
