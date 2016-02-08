# Virtual Machine

#### Requirements
* [Vagrant](https://www.vagrantup.com/downloads.html)
* [Virtual Box](https://www.virtualbox.org/wiki/Downloads)
* [VirtualBox  Extension Pack](http://download.virtualbox.org/virtualbox/5.0.10/Oracle_VM_VirtualBox_Extension_Pack-5.0.10-104061.vbox-extpack)

#### Info:
The directory contains a `Vagrantfile` which is the build script, and a `package.box` which is the virtual machine image. Vagrant combines these to create and launch an up-to-date virtual machine and launches it in Virtual Box. A board support package (BSP) is used to compile code for the S32V development board

#### Installation:
* Download the `S32V_BSP.zip` [here](https://drive.google.com/a/uw.edu/file/d/0Bxacre40weBuM2RXWWxNUFJpWnc/view) and extract it to the `VM/` subdirectory of the repo. 
* Download the `package.box` (large file) [here](https://drive.google.com/open?id=0B-HGy6dgp_EHMGtzTW1rOGdJYVE). Save it in the `VM/` subdirectory of this repo.
* Install the box: `vagrant box add DriverPredictionGensetControl VM/package.box` 
* Run `vagrant up` (this will install the latest packages for the dev environment). Vagrant will now start Virtual Box, SSH in and automatically install packages. Hopefully everything updates installs correctly.
* The `package.box` file can now be deleted
* The VM is perminantly in the Virtual Box's default directory

#### Usage:
* Run `vagrant up` in the `VM/` directory to open the vm
* `vagrant reload` can reload a running vm
* `vagrant provision` will rerun the install scripts in the above commands and update all packages
* 'vagrant halt' shutsdown vm outside of vm
* 'vagrant ssh' opens shell
* 'exit' once ssh'ed into vm
* shared folder between host and VM in VM is /vagrant

#### Update:
* Update Vm: `git pull` this repo for the latest `Vagrantfile`
* Run `vagrant up --provision` in the `Vagrantfile` directory

#### Uninstall:
* Uninstall Vm: `vagrant destroy` in the `VM/` directory. You will loose you virtual machine, but can reinstall fresh.
