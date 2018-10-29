# .bashrc

# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi

# Uncomment the following line if you don't like systemctl's auto-paging feature:
# export SYSTEMD_PAGER=

# User specific aliases and functions
export PATH=/home/masoud/cloud/intel/compilers_and_libraries_2018/linux/mpi/intel64/bin:$PATH
source /home/masoud/cloud/intel/bin/compilervars.sh intel64
source /home/masoud/cloud/intel/bin/ifortvars.sh intel64
source /home/masoud/cloud/intel/impi/2018.1.163/intel64/bin/mpivars.sh intel64
export I_MPI_FABRICS=shm:dapl
export I_MPI_DAPL_PROVIDER=ofa-v2-ib0
export I_MPI_DYNAMIC_CONNECTION=0
