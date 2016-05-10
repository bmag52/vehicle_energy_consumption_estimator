# Developement Environment Setup Introduction
* 2 development environments are available
	* linux with eclipse setup
		* relies on vagrant VM provisioning
		* intended for embedded developement
		* known issues
			* cannot build opencv due to space limited space initialized by vagrant / virtualbox
	* osx also with eclipse setup
		* requires some additional packages


#### Vagrant VM Requirements
* [Vagrant](https://www.vagrantup.com/downloads.html)
* [Virtual Box](https://www.virtualbox.org/wiki/Downloads)
* [VirtualBox  Extension Pack](http://download.virtualbox.org/virtualbox/5.0.10/Oracle_VM_VirtualBox_Extension_Pack-5.0.10-104061.vbox-extpack)

#### Vagrant VM Info:
The directory contains a `Vagrantfile` which is the build script, and a `package.box` which is the virtual machine image. Vagrant combines these to create and launch an up-to-date virtual machine and launches it in Virtual Box. The cross-compiler for the S32V board support package (BSP) is linked to the eclipse tool chain in the VM.

#### Installation for Vagrant VM:
* Download the `S32V_BSP.zip` [here](https://drive.google.com/open?id=0Bxacre40weBuZEZDdEswTVRIZms) and extract it to the `VM/` subdirectory of the repo. 
* Download the `package.box` (large file) [here](https://drive.google.com/open?id=0B-HGy6dgp_EHMGtzTW1rOGdJYVE). Save it in the `VM/` subdirectory of this repo.
* Install the box: `vagrant box add predictive_thermo_controller VM/package.box` 
* Run `vagrant up` (this will install the latest packages for the dev environment). Vagrant will now start Virtual Box, SSH in and automatically install packages. Hopefully everything updates installs correctly.
* The `package.box` file can now be deleted
* The VM is perminantly in the Virtual Box's default directory

#### Usage for Vagrant VM:
* Run `vagrant up` in the top level directory to open the vm
* `vagrant reload` can reload a running vm
* `vagrant provision` will rerun the install scripts in the above commands and update all packages
* `vagrant halt` shutsdown vm outside of vm
* `vagrant ssh` opens shell
* `exit` once ssh'ed into vm
* shared folder between host and VM in VM is `/vagrant`

#### Building OpenCV for Vagrant VM: 
* Once VM is provisioned and running, open terminal.
* Follow instructions [here](http://docs.opencv.org/2.4/doc/tutorials/introduction/linux_install/linux_install.html)

#### New Colors in Eclipse for Vagrant VM:
* Open eclipse
* Help -> Install New Software -> add 
* Copy and paste this link into URL window (http://eclipse-color-theme.github.com/update). No name needed
* Select check-box and click through to installation

#### Update for Vagrant VM:
* Update Vm: `git pull` this repo for the latest `Vagrantfile`
* Run `vagrant up --provision` in the `Vagrantfile` directory

#### Uninstall Vagrant VM:
* Uninstall Vm: `vagrant destroy` in the `VM/` directory. You will loose you virtual machine, but can reinstall fresh.

#### Adding pertinent drivers for OSX developement:
* install eclipse 64-bit for osx [here](http://www.eclipse.org/downloads/packages/eclipse-ide-cc-developers/galileosr2)
* install brew [here](http://brew.sh/)
* in terminal make the following calls 
	* brew install eigen
	* brew install boost
	* brew install opencv
	* brew install g++
	* brew install gdb
* follow instructions [here](http://stackoverflow.com/questions/13913818/how-to-get-a-codesigned-gdb-on-osx) allow usage of debugger



