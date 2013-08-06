[CCode (lower_case_cprefix = "ecg_", cheader_filename = "ecg-parser.h")]
namespace ECG 
{    

	[CCode (cheader_filename = "ecg-parser.h", cprefix = "ECG_CHANNEL_", has_type_id = false)]
	[Flags]
	public enum Channel {
		I,
		II,
		III,
		V1,
		V2,
		V3,
		V4,
		V5,
		V6,
		AVR,
		AVL,
		AVF,
		X
	}

	[Compact]
	[CCode (free_function = "ecg_close", cname = "ECG", cprefix = "ecg_")] 
	public class ECG
	{          

		[CCode (cname = "ecg_cache")]
                public float cache(int first_frame, int last_frame);

		[CCode (cname = "ecg_clear_cache")]
                public float clear_cache();

		[CCode (cname = "ecg_get_magnitude")]
                public float get_magnitude(int ECGChannel, int frame);

		[CCode (cname = "ecg_get_frames_count")]
                public int get_frames_count();


	}

	[CCode (cname = "ecg_open")]
	public ECG open(string filename);
}

