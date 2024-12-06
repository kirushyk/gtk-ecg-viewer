public class App: Gtk.Application
{

	public App()
	{
		Object(application_id: "org.gnome.ecg-viewer", flags: ApplicationFlags.HANDLES_OPEN);
	}

	protected override void activate()
	{
		Gtk.Window window = new MainWindow(this, null);
#if HAVE_GTK4
			window.show();
#else
			window.show_all();
#endif
		add_window(window);
	}

	protected override void open(GLib.File[] files, string unused)
	{
		foreach (var filename in files) {
			Gtk.Window window = new MainWindow(this, filename.get_path());
#if HAVE_GTK4
			window.show();
#else
			window.show_all();
#endif
			add_window(window);
		}
	}

	public static int main(string[] args)
	{
		App app = new App();
		return app.run(args);
	}

}

