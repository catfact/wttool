s = Server.default;
s.waitForBoot { Routine {
	b = Buffer.read(s, "/home/emb/code/wttool/AKWF_sinharm_0001_sc.wav");
	s.sync;
	{
		VOsc.ar(b.bufnum, 220).dup * 0.25
	}.play(s);

}.play; };