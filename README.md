# PowerTunerCLI

![](src/Resources/Windows/icon.ico)

Command-line PowerTuner client.

Output is in JSON format, so you can easily script the cli client.

For a GUI desktop client, see [PowerTunerClient](https://github.com/PowerTuner/PowerTunerClient)

For a gamepad first client and couch gaming, see [PowerTunerConsole](https://github.com/PowerTuner/PowerTunerConsole)

## Usage

To save your daemons, use the command:

```
$ PowerTunerCLI set add-daemons <daemon-name1> <address1> <port1> ...
```

You can now use _\<daemon-name\>_ in place of _address;port_ to specify the daemon to connect to.

Example commands:

```
$ PowerTunerCLI set add-daemons myDaemon 10.10.1.40 56000
$ PowerTunerCLI get device-info myDaemon
```

List available settings for each category with the command:
```
$ PowerTunerCLI help <category>_settings
```

## Custom range limits for settings

Some settings like TDP or GPU clock, use a json database with min/max ranges.

Built-in ranges are safe, not exceeding the default/recommended values for the device.

These range limits will be enforced when you apply settings.

To create your own ranges override, see instructions [here](https://github.com/PowerTuner/PowerTunerClient#custom-input-ranges-for-settings).

Your overrides will replace built-in ranges on settings apply.

## Build

clone this repo:
```
git clone --recursive https://github.com/PowerTuner/PowerTunerCLI.git
```

Build options:

```text
WITH_INTEL
enable building of client UI for Intel CPU settings, default ON

WITH_AMD
enable building of client UI for AMD CPU settings, default ON
```

### Linux

requirements:

- gcc or clang
- cmake
- qt6-base

```
$ cd PowerTunerCLI
$ cmake -B build -DCMAKE_BUILD_TYPE=Release
$ make -C build
```

### Windows

requirements:

- Visual Studio 2022 with the component **Desktop development with C++** installed
- [Windows terminal](https://apps.microsoft.com/detail/9n0dx20hk701)
- [latest Qt](https://www.qt.io/development/download-qt-installer-oss) with **MSVC 2022 64bit** installed

Open terminal and create a new instance of **Developer Command Promp for VS 2022** or **Developer PowerShell for VS 2022**.

_Tab completition feature is only available in PowerShell_

```
$ cd PowerTunerCLI
$ cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="<path to Qt>\msvc2022_64\lib\cmake" -G "Ninja"
$ ninja -C build
```

Replace _\<path to Qt\>_ with your Qt root path, default is **C:\Qt\\<Qt version\>**

---

```text
PowerTunerCLI v1.0 - GPLv3 kylon

Usage: PowerTunerCLI <mode> <options>

  help
      Show help.

      Options:
        linux_settings
            List linux settings for device-settings option.

        linux_amd_settings
            List linux AMD settings for device-settings option.

        windows_settings
            List windows settings for device-settings option.

        amd_cpu_settings
            List AMD CPU settings for device-settings option.

        intel_cpu_settings
            List Intel CPU settings for device-settings option.

        fan_control_settings
            List fan control settings for device-settings option.

  get
      Request and print data.

  set
      Set options.


PowerTunerCLI <mode> help, for more help
```
