sudo yum install epel-release sshpass nmap \
sudo yum --setopt=group package types=optional \ 
groupinstall "Infiniband Support"
sudo yum install infiniband-diags perftest qperf \
numactl-devel numactl rdma-core-devel papi papi-devel \
libmemcached-devel libnuma cmake3 libibverbs-utils \
opensm libmemcached
sudo yum group install "Development Tools"
# Reboot and chckconfig
sudo reboot
chkconfig rdma on
chkconfig opensm on
# If we weren't running CentOS-HPC, it would be necessary to install the psxe runtme
#sudo rpm --import https://yum.repos.intel.com/2017/setup/RPM-GPG-KEY-intel-psxe-runtime-2017
#sudo rpm -Uhv https://yum.repos.intel.com/2017/setup/intel-psxe-runtime-2017-reposetup-1-0.noarch.rpm
#sudo yum install intel-psxe-runtime
# OR, if we have Parallel Studio XE
# sudo yum remove intel-mpi-doc intel-mpi-rt-core-174.x86_64 intel-mpi-rt-exec-050.x86_64
# sudo yum install nfs-utils
# For PSXE installation, download and tar
# If you want to use NFS, create the dirs
# On server
# mkdir ~/cloud
# sudo "~/cloud *(rw, sync, no_root_squash, no_subtree_check)" > /etc/exports
# sudo exportfs -a
# sudo service nfs-server enable
# sudo service nfs-server start
# On client, edit fstab to reflect file
