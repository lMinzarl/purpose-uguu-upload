# Purpose Uguu Upload

A KDE Purpose plugin for uploading screenshots and images to [Uguu](https://uguu.se/) directly from applications that support the KDE Purpose sharing framework.

The plugin was created as a simple alternative to the Imgur upload integration, allowing screenshots to be uploaded without opening a browser or manually selecting files.

After a successful upload, the resulting Uguu URL is:

* Copied automatically to the clipboard.
* Displayed in a desktop notification.

## Requirements

The plugin requires Qt 6 and KDE Frameworks 6.

Build dependencies include:

* CMake
* A C++ compiler
* Qt 6 Core
* Qt 6 Gui
* Qt 6 Network
* KDE Frameworks 6 CoreAddons
* KDE Frameworks 6 Purpose
* KDE Frameworks 6 Notifications

Exact package names vary between Linux distributions.

## Building

Clone the repository:

```bash
git clone https://github.com/lMinzarl/purpose-uguu-upload.git
cd purpose-uguu-upload
```

Configure the project:

```bash
cmake -S . -B build
```

Build it:

```bash
cmake --build build
```

## Installing

Install the compiled plugin:

```bash
sudo cmake --install build
```

Depending on your distribution and CMake configuration, the Purpose plugin may be installed under `/usr/local`.

After installation, restart the application you want to use the plugin from.

In some cases, logging out and back in may be necessary for the desktop environment to detect newly installed plugins.

## Usage

### Spectacle

1. Take a screenshot with Spectacle.
2. Open the **Export** or **Share** menu.
3. Select **Upload to Uguu**.
4. Wait for the upload to complete.
5. The resulting URL will automatically be copied to your clipboard.

A desktop notification will also display the uploaded URL.

### Dolphin

The plugin can also be used through Dolphin's sharing functionality for supported files.

Select an image and use the available Purpose sharing actions to upload it to Uguu.

## Privacy

This plugin uploads files to the third-party file hosting service Uguu.

Files uploaded using this plugin leave your computer and are sent to Uguu's servers. Do not upload files containing sensitive or confidential information unless you understand and accept the implications of using the service.

Uguu controls file retention, availability, rate limits, and other server-side behavior. This project is not affiliated with or operated by Uguu.

See the Uguu website for current information about the service:

https://uguu.se/

## Limitations

* The plugin currently focuses on image/screenshot uploads.
* Upload availability depends on the Uguu service.
* Uguu may impose file-size, rate-limit, retention, or other restrictions.
* Behavior may differ between applications depending on how they integrate with KDE Purpose.
* The plugin has primarily been tested with Spectacle and Dolphin on KDE Plasma 6.
* Multi-file uploads are not currently supported.

## Uninstalling

If you still have the build directory, the installed files can be identified from CMake's install manifest:

```bash
cat build/install_manifest.txt
```

The plugin files listed there can then be removed from the system.

Be careful when manually removing files from system directories.

## Development

The plugin is implemented in C++ using Qt 6 and KDE Frameworks 6.

The main components used are:

* **KDE Purpose** — integration with KDE sharing/export actions.
* **Qt Network** — communication with the Uguu API.
* **Qt GUI** — clipboard integration.
* **KNotifications** — desktop upload notifications.

Contributions, bug reports, testing on additional distributions, and improvements are welcome.

## Compatibility

The plugin is intended for KDE Plasma 6 environments using KDE Frameworks 6.

It has been tested with KDE applications including Spectacle and Dolphin.

Testing reports from other KDE applications and Linux distributions are welcome.

## License

This project is licensed under the GNU General Public License v2.0 or later (`GPL-2.0-or-later`).

See the `LICENSE` file for the full license text.

## Acknowledgements

* The KDE community for the Purpose framework.
* Uguu for providing the temporary file hosting service and upload API.

## Disclaimer

This project is an independent community project and is not affiliated with, endorsed by, or maintained by KDE or Uguu.

