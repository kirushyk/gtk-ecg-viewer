public class ECGMonitor: Gtk.DrawingArea
{

	protected ECG.ECG ecg = null;
	protected int first_frame = 0;
	protected int first_cached_frame = 0;
	protected int last_cached_frame = 0;

	protected int start = 0;
	protected int end = 11;

	public void show_limb_leads()
	{
		start = 0;
		end = 2;
		update();
	}

	public void show_augment_limb_leads()
	{
		start = 9;
		end = 12;
		update();
	}

	public void show_chest_leads()
	{
		start = 3;
		end = 8;
		update();
	}

	public void load(string path)
	{
		start = 0;
		end = 2;
		ecg = ECG.open(path);
		ecg.cache(first_cached_frame = 0, last_cached_frame = 2048);

		first_frame = 0;
		update();
	}

	public ECGMonitor()
	{
		add_events(Gdk.EventMask.BUTTON_PRESS_MASK |
		           Gdk.EventMask.BUTTON_RELEASE_MASK |
		           Gdk.EventMask.POINTER_MOTION_MASK);
		set_size_request(200, 225);

		// Timeout.add(1000, update);
	}

	public int get_ecg_size()
	{
		return ecg.get_frames_count();
	}

	public void set_ecg_shift(int shift)
	{
		first_frame = shift;
		if ((first_frame > last_cached_frame) || (first_frame < first_cached_frame))
		{
			int frames_count = ecg.get_frames_count();
			first_cached_frame = first_frame - 10000;
			if (first_cached_frame < 0)
				first_cached_frame = 0;
			if (first_cached_frame > frames_count);
				first_cached_frame = frames_count;
			if (last_cached_frame < 0)
				last_cached_frame = 0;
			if (last_cached_frame > frames_count);
				last_cached_frame = frames_count;
			last_cached_frame = first_frame + 10000;
			ecg.cache(first_cached_frame, last_cached_frame);
		}
		update();
	}

	public override bool draw(Cairo.Context c)
	{
		if (ecg == null)
			return true;

		string channels_names[14] = 
		{
			"I", "II", "III", "V1", "V2", "V3", "V4", "V5", "V6", "aVR", "aVL", "aVF", "P", "0"
		};

		float scale = 1.0f;
		int width = get_allocated_width();
		int height = get_allocated_height();


        	c.set_source_rgb(1.0, 1.0, 1.0);
		c.rectangle(0, 0, width, height);
		c.fill();

        	c.set_source_rgb(0.0, 0.0, 0.0);

		c.move_to(40, 40);
		c.show_text("Grid: 2 lines / mV, 1 line / 100 ms");

		c.set_line_width(0.25);
		for (double x = 0; x < width; x += 20.0)
		{
			c.move_to(x, 0);
			c.line_to(x, height);			
		}
		c.stroke();

		c.set_line_width(1.0);

		for (int channel = start; channel <= end; channel++)
		{
			double magnitude_scale = 150.0f;
			if ((start == 0) || (start == 9))
				magnitude_scale = 300.0f;

			double y = (channel + 1 - start) * magnitude_scale * 0.5 + (height - (end - start + 2) * magnitude_scale * 0.5) * 0.5;

			c.set_line_width(0.25);
			c.move_to(0, y);
			c.line_to(width, y);	
			c.stroke();

			c.set_line_width(1.0);
			c.move_to(40, y + 40);
			c.show_text(channels_names[channel]);

			c.move_to(0, y);
			for (int i = 0; i < (int)(width * 2.0 / scale); i++)
			{
				c.line_to(i * scale * 0.5, y - ecg.get_magnitude(channel, i + first_frame) * magnitude_scale);
			}
			c.stroke();
		}
		c.stroke();
		return true;
	}

	public override bool button_press_event(Gdk.EventButton event)
	{
		return false;
	}

	public override bool button_release_event(Gdk.EventButton event)
	{
		return false;
	}

	public override bool motion_notify_event(Gdk.EventMotion event)
	{
		return false;
	}

	private void redraw_canvas()
	{
		var window = get_window();
		if (window == null)
		{
			return;
		}

		var region = window.get_clip_region();
		window.invalidate_region(region, true);
		window.process_updates(true);
	}

	private bool update()
	{
		redraw_canvas();
		return true;
	}

}

