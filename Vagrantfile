# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure(2) do |config|
  config.vm.box = "DriverPredictionGensetControl"
  config.ssh.username = "vagrant"
  config.ssh.password = "vagrant"
  config.vm.provider "virtualbox" do |vb|
    # Display the VirtualBox GUI when booting the machine
    vb.gui = false
    # Customize the amount of memory on the VM:
    # vb.memory = "1024"
  end

  # Bash Script
  #  config.vm.provision :shell, :path => "script.sh" # runs script file
  config.vm.provision "shell", inline: <<-SHELL
    sudo apt-get update

    # install programs
    echo "---------------INSTALLING PACKAGES---------------"
    declare -a program=(
      "python"
      "python-pip"
      "python-numpy" )
    for i in "${program[@]}"
      do
         echo "Installing $i ..."
         apt-get -y install "$i"
      done

    # upgrade and clean up
    sudo apt-get upgrade -y
    # sudo apt-get dist-upgrade -y
    sudo apt-get autoremove -y
    sudo apt-get autoclean -y
  SHELL
end
