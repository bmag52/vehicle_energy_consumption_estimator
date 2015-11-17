# Virtual Machine

#### Requires:
* [Vagrant](https://www.vagrantup.com/downloads.html)
* [Virtual Box](https://www.virtualbox.org/wiki/Downloads)
* [VirtualBox  Extension Pack](http://download.virtualbox.org/virtualbox/5.0.10/Oracle_VM_VirtualBox_Extension_Pack-5.0.10-104061.vbox-extpack)

#### Info:
The directory contains a `Vagrantfile` which is the build script, and a `package.box` which is the virtual machine image. Vagrant combines these to create and launch an up-to-date virtual machine and launches it in Virtual Box.

#### Installation:
* Make sure you install the required software above
* Clone the repo: `git clone https://github.com/UWEcoCAR/UW-Infotainment.git`
* Download the `package.box` (large file) [here](https://drive.google.com/open?id=0B-HGy6dgp_EHMGtzTW1rOGdJYVE). Save it in the `VM/` subdirectory of this repo.
* `cd` to the `VM/` directory
* Install the box: `vagrant box add infotainment package.box` 
* Run `vagrant up` (this will install the latest packages for the dev environment). Vagrant will now start Virtual Box, SSH in and automatically install packages. Hopefully everything updates installs correctly.
* The `package.box` file can now be deleted
* The VM is perminantly in the Virtual Box's default directory

#### Usage:
* Run `vagrant up` in the `VM/` directory to open the vm
* `vagrant reload`can reload a running vm
* the `--provision` argument will rerun the install scripts in the above commands

#### Update:
* Update Vm: `git pull` this repo for the latest `Vagrantfile`
* Run `vagrant up --provision` in the `Vagrantfile` directory

#### Uninstall:
* Uninstall Vm: `vagrant destroy` in the `VM/` directory. You will loose you virtual machine, but can reinstall fresh.
