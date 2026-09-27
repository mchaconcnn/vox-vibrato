# Vox Vibrato

A small vocal **pitch-vibrato** plug-in for macOS. The project builds:

- Audio Unit (`.component`) for Logic Pro
- VST3 (`.vst3`) for VST3 hosts
- Standalone app for quick listening and UI checks

## Controls

- **Rate:** 0–10 Hz, default 5 Hz. At 0 Hz, modulation stops.
- **Intensity:** 0–100%, default 50%. This scales the sine wave vertically.
- **Peak Shift:** 0–100 cents, default 20 cents. This is the peak/valley ceiling.

Effective pitch excursion is `Peak Shift × Intensity`. For example, 20 cents at
50% intensity produces approximately ±10 cents. The processor is deliberately
100% wet to avoid mixing a dry voice with the delayed voice and creating a
chorus-like sound.

## Build and preview

Version 0.1.2 adds the black instrument-panel editor, green oscilloscope-style
modulation preview, machined circular Peak Shift knob, and website link.
The scope shows one second horizontally; its vertical scale follows Peak Shift
and its amplitude follows Intensity. It is a modulation preview, not an audio
spectrum analyser. The footer opens https://rco.cr/vox/vibrato/ (landing page pending).
Double-click any control to restore its default, or click its value to type.

Requirements: macOS 12+, Xcode command-line tools, CMake, and internet access on
the first configure so CMake can fetch JUCE 9.0.2.

```bash
./script/build_and_run.sh
```

Run the build, DSP test, and artifact checks:

```bash
./script/build_and_run.sh --verify
```

## Install for Logic Pro

```bash
./script/install_user_plugins.sh
```

Then restart Logic Pro and insert:

`Audio FX → Audio Units → rco.cr → Vox Vibrato`

Logic uses Audio Units, not VST/VST3. The VST3 build is included for other hosts.

If Logic does not list the plug-in, open **Logic Pro → Settings → Plug-in Manager**,
search for “Vox Vibrato,” and run validation again.

## Signal use

For a corrected vocal, place Vox Vibrato after pitch cleanup and automate
**Intensity** up on sustained vowels or note endings. Start around 5 Hz,
20 cents, and 25–50% intensity, then adjust by ear.

## DSP note

The effect uses a cubic-interpolated modulated delay calibrated from cents. This
is the classic time-domain vibrato approach: it changes pitch without modulating
volume. Rates below 0.05 Hz use a bounded delay excursion to keep latency
practical; the musical 4–7 Hz vocal range is calibrated normally.

## JUCE licensing

This source fetches JUCE 9.0.2. Before distributing a compiled plug-in, choose
and comply with the applicable JUCE commercial or AGPLv3 licence. See the
official JUCE licensing terms; this repository does not grant a JUCE licence.
