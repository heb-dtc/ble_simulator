## BLE Peripheral Simulator

### Build

- This software is a Qt application and require version 5.10 to build and run proprely. Please check Qt website and follow the instruction to install it on your environnement. 

- Once everything is configured properly run the following commands:
`qmake && make`

### Run 

 - Running this software requires hardware capable of performing BLE. Make sure to check if your system is compliant before trying this software.

#### On Linux

- Qt uses BLueZ and the blueZ stack of your machine needs to be configured. This may vary from linux distro to distro. The following has been tested on archlinux.

- These are the commands to run:
> ```
modprobe btusb
systemctl start bluetooth.service 
sudo btmgmt -i hci0 power off 
sudo btmgmt -i hci0 le on
sudo btmgmt -i hci0 bredr off
sudo btmgmt -i hci0 connectable on
sudo btmgmt -i hci0 advertising on
sudo btmgmt -i hci0 power on
```

#### On MacOS

- It seems that simply turning the bluetooth on is enough.


