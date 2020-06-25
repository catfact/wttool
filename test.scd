
~test_wav = { arg path;
b = Buffer.read(s, path);
	s.sync;
	{
		Osc.ar(b.bufnum, 220).dup
		* EnvGen.kr(Env.asr(1.0, 4.0, 2.0), doneAction:2)
		* 0.25
	}.play(s);


s = Server.default;
s.waitForBoot { Routine {
		~test_wav.value("/home/emb/code/wttool/AKWF_sinharm_0001_sc.wav");

}.play; };