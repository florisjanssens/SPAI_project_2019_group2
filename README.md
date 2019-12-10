#  SPAI Project 2019 - Team 2 - Deliverables

This Git contains the deliverables for the SPAI Project of the R&D Experience course (B-KUL-T47UXW). Here you can find the code for the different parts of the assignment together with usage/installation instructions.


## Team members (a-z)
- Darina Abaffyová
- Dionys Nabarro
- Floris Janssens
- Meng Shang
- Wout Slaets
- Yannick Byamungu

## Requirements and Usage of Code
### Part 1-2-3b: Single/multi-channel push-to-listen & online noise reduction
You can find part 1-2 in their respectable folder in the Git. Here you can find the installation and usage instructions. These instructions are mostly related to Qt configurations as the code itself is easy to get running once Qt is correctly configured.
#### Development environment and supported OSes
The application for these parts were made completely in Qt. Qt Creator 4.10.0 with Qt 5.13.1 was used. Qt is a cross-platform app-framework for Linux, macOS, Windows, Android, iOS, etc.
This also means the application should work on all these OSes. Because of a limited amount of available devices and limited amount of OS variability, only the following OSes were completely tested and verified to be working completely:
- Linux
- Windows (Windows 10 tested)
- Android (All versions starting from Android V6 are tested)

Of course, other supported OSes of Qt should also work but mileage may vary (Qt supports all these OSes for all the functionality used but of course Qt also has some bugs sometimes). The OSes listed above are completely tested and verified however.

#### General installation
If you are trying to run the application on a regular computer, the installation is very easy for all operating systems.
- Just download the Open Source version of Qt by clicking [here](https://www.qt.io/download-open-source).
- Follow the instructions on this link to install Qt.
- After installing, go to the .pro file inside the folder for the part you want to open. Open this .pro file with Qt Creator.
- Now you can click Build > Run
- Now you should be able to see the UI and use the application

#### Getting your installation ready for Android usage
Being able to run the code on an Android device using Qt is a bit more difficult. To do this you can follow the official tutorial inside the Qt documentation by clicking [here](https://doc.qt.io/qt-5/android-getting-started.html).
Note that there seems to be a problem while using Java JDK versions other than v8. If you get any kind of error, the first thing to do would be to use Java JDK v8.
After installing all the requirements and setting the Android NDK and SDK paths in the Qt Creator options as mentioned in the link, the code can be used on Android devices.

On your Android device you also need to enable USB debugging. First you must enable the Developer options menu on your device. This can be done by going to Settings > About and tapping the Build number of your phone seven times. Afterwards you can enable USB debugging by going to Settings > Developer options.

Now connect your Android phone using a USB cable. Make sure the project is opened on Qt and go to the Projects tab. Then you can select one of the android kits which should be visible to build & run the code on android. After selecting one of these kits, you should be able to simply click Build > Run to run the code on your Android device.

### Part 3a: Offline noise reduction
Using the code of offline noise reduction is very easy, you simply need a working installation of [MATLAB](https://nl.mathworks.com/products/matlab.html). Using the code comes down to running the .m files.
