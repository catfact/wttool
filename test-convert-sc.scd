// read a soundfile from disk, convert/resample to wavetable, load into a Buffer
~load_wavetable = { arg path;

	var soundFile, numFrames;
	var signal, waveTable, buffer;

	soundFile = SoundFile.openRead(path);
	numFrames = soundFile.numFrames;
	signal = Signal.newClear(numFrames);

	soundFile.readData(signal);

	if (numFrames.isPowerOfTwo.not, {
		var newNumFrames = numFrames.nextPowerOfTwo;
		var resampled =  signal.resamp1(newNumFrames);
		postln("last value (in/out): " ++ signal[numFrames-1] ++ " / " ++ resampled[newNumFrames-1]);
		signal = resampled.as(Signal);
		numFrames = newNumFrames ;
	});

	waveTable = signal.asWavetable;
};

~wt = ~load_wavetable.value("~/code/wttool/test-input/AKWF_eguitar_0001.wav".standardizePath);
~tsf = SoundFile.openRead("~/code/wttool/test-output/AKWF_eguitar_0001-1024-SC.wav".standardizePath);

~twt= Signal.newClear(~tsf.numFrames);
~tsf.readData(~twt);

~err = ~twt - ~wt.as(Signal);
"max error: ".post;
(~err.abs.maxItem+1).ampdb.post;
" dB".postln;

{ ~err.plot }.defer;


/////////////
//////////

/*
////... our error from just resampling is small, as expected
n = 1024;
~sf = SoundFile.openRead("~/code/wttool/test-input/AKWF_eguitar_0001.wav".standardizePath);
z = Signal.newClear(~sf.numFrames);
~sf.readData(z);
a = z.resamp1(n);

~sf = SoundFile.openRead("~/code/wttool/test-output/AKWF_eguitar_0001-1024.wav".standardizePath);
b = Signal.newClear(n);
~sf.readData(b);

e = (a-b);

{ e.plot; }.defer;
*/