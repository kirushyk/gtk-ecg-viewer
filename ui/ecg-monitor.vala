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
		ecg.cache(first_cached_frame = 0, last_cached_frame = 400);

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

	public int get_max_shift()
	{
		int width = get_allocated_width();
		return ecg.get_frames_count() - width * 2;
	}

	public int get_ecg_shift()
	{
		return first_frame;
	}

	public void set_ecg_shift(int shift)
	{
		first_frame = shift;
		if ((first_frame + 3000 > last_cached_frame) || (first_frame < first_cached_frame))
		{
			int frames_count = ecg.get_frames_count();
			first_cached_frame = first_frame - 10000;
			last_cached_frame = first_frame + 10000;
			if (first_cached_frame < 0)
				first_cached_frame = 0;
			if (first_cached_frame > frames_count)
				first_cached_frame = frames_count;
			if (last_cached_frame < 0)
				last_cached_frame = 0;
			if (last_cached_frame > frames_count)
				last_cached_frame = frames_count;
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
			"I", "II", "III", "V1", "V2", "V3", "V4", "V5", "V6", "aVR", "aVL", "aVF", "Unknown Channel", "0"
		};

		float scale = 1.0f;
		int width = get_allocated_width();
		int height = get_allocated_height();


        	c.set_source_rgb(1.0, 1.0, 1.0);
		c.rectangle(0, 0, width, height);
		c.fill();

        	c.set_source_rgb(0.0, 0.0, 0.0);

		double magnitude_scale = 50.0f;
/*
		if (false)//  && (start == 0) || (start == 9))
		{
			c.move_to(40.0, 40.0);
			c.show_text("250 μV ✕ 100 ms");
			magnitude_scale = 100.0f;
		}
		else
*/
		{
			c.move_to(40.0, 40.0);
			c.show_text("0.5 mV ✕ 100 ms");
		}


		c.set_line_width(0.25);
		for (double x = 0.5; x < width; x += 20.0)
		{
			c.move_to(x, 0);
			c.line_to(x, height);			
		}
		c.stroke();

		for (int channel = start; channel <= end; channel++)
		{
			double y = (channel + 1 - start) * magnitude_scale * 2 + (height - (end - start + 2) * magnitude_scale * 2) * 0.5;

			c.set_line_width(0.25);
			for (int k = -2; k <= 2; k++)
			{
				c.move_to(0.0, y + k * 20.0);
				c.line_to(width, y + k * 20.0);			
			}
			c.stroke();

			c.set_line_width(1.0);
			c.move_to(40.0, y + 40.0);
			c.show_text(channels_names[channel]);
			c.move_to(200.0, y + 40.0);
			// double magnitude = ecg.get_magnitude(channel, first_frame);
			// c.show_text(@"level: $magnitude");

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

