# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure(2) do |config|
	config.vm.box = "predictive_thermo_controller"
	config.ssh.username = "vagrant"
	config.ssh.password = "vagrant"
	config.vm.provider "virtualbox" do |vb|

	# Display the VirtualBox GUI when booting the machine
	vb.gui = true

	# Customize the amount of memory on the VM:
	vb.memory = "8180"
end

	# Bash Script
	# config.vm.provision :shell, :path => "script.sh" # runs script file
	config.vm.provision "shell", inline: <<-SHELL
	sudo apt-get update

	# install programs
	echo "***************************************************"
	echo "--------------- INSTALLING PACKAGES ---------------"
	echo "***************************************************"
	echo ""
	declare -a program=(
		"python"
		"python-pip"
		"python-numpy"
		"build-essential"
		"openssh-server"
		"htop"
		"eclipse"
		"libeigen3-dev"
		"eclipse-cdt"
		"libboost-all-dev"
		"git"
		"gdb"
		"unzip"
		"wkhtmltopdf"
		"subversion"
		"libqt4-webkit"
		"libqt4-dev"
		"cutycapt"
		"libpng12-dev"
		"libpng++-dev"
		"libpng3"
		"libpnglite-dev"
		"libpngwriter0-dev"
		"zlib1g-dbg"
		"zlib1g-dev"
		"pngtools"
		"libjpeg8-dev"
		"libjpeg8-dbg"
		"libjpeg-progs"
		"ffmpeg"
		"libgstreamer0.xy-0-dbg"
		"libgstreamer0.xy-dev"
		"libxine-dev"
		"libunicap2-dev"
		"swig"
		"libdc1394-utils"
		"cmake")

	for i in "${program[@]}"
	do
		echo "----------------- Installing $i ... -----------------"
		sudo apt-get -y install "$i" --upgrade
	done	

	echo "------------- house keeping ------------- "
	# upgrade and clean up
	sudo apt-get upgrade -y
	# sudo apt-get dist-upgrade -y
	sudo apt-get autoremove -y
	sudo apt-get autoclean -y
	SHELL

	end
