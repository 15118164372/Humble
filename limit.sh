#!/bin/sh
#***********************************************
# File Name  : limit
# Description: 打开linux服务器限制
# Make By    :lqf/200309129@163.com
# Date Time  :2016/03/12 
#***********************************************

limit=`ulimit -n`
echo "ulimit -n:"$limit
max=`cat /proc/sys/fs/file-max`
echo "file-max:"$max

maxfile=$(expr $max - 100)
tempfile=/etc/security/limits.tmp
rm -rf $tempfile
while read line
do
    bDelete=0
    temp=`echo $line|grep "soft nofile"`
	if [ "$temp" != "" ]
	then
		bDelete=1
	fi
	temp=`echo $line|grep "hard nofile"`
	if [ "$temp" != "" ]
	then
		bDelete=1
	fi
	
	if [ "$bDelete" == "0" ]
	then
		echo $line >> $tempfile
	fi
done < /etc/security/limits.conf

echo "* soft nofile "$maxfile >> $tempfile
echo "* hard nofile "$maxfile >> $tempfile

rm -rf /etc/security/limits.conf
mv $tempfile /etc/security/limits.conf

echo "session required /lib/security/pam_limits.so" >> /etc/pam.d/login

port_range=`sysctl -a | grep range`
echo $port_range > /etc/sysctl.conf
echo "net.core.rmem_max=16777216" >> /etc/sysctl.conf
echo "net.core.wmem_max=16777216" >> /etc/sysctl.conf
echo "net.ipv4.tcp_rmem=4096 87380 16777216" >> /etc/sysctl.conf
echo "net.ipv4.tcp_wmem=4096 65536 16777216" >> /etc/sysctl.conf
echo "net.ipv4.tcp_fin_timeout = 10" >> /etc/sysctl.conf
echo "net.ipv4.tcp_tw_recycle = 1" >> /etc/sysctl.conf
echo "net.ipv4.tcp_timestamps = 1" >> /etc/sysctl.conf
echo "net.ipv4.tcp_tw_reuse = 1" >> /etc/sysctl.conf
echo "net.ipv4.tcp_window_scaling = 0" >> /etc/sysctl.conf
echo "net.ipv4.tcp_sack = 0" >> /etc/sysctl.conf
echo "net.core.netdev_max_backlog = 30000" >> /etc/sysctl.conf
echo "net.ipv4.tcp_no_metrics_save=1" >> /etc/sysctl.conf
echo "net.core.somaxconn = 262144" >> /etc/sysctl.conf
echo "net.ipv4.tcp_syncookies = 0" >> /etc/sysctl.conf
echo "net.ipv4.tcp_max_orphans = 262144" >> /etc/sysctl.conf
echo "net.ipv4.tcp_max_syn_backlog = 262144" >> /etc/sysctl.conf
echo "net.ipv4.tcp_synack_retries = 2" >> /etc/sysctl.conf
echo "net.ipv4.tcp_syn_retries = 2" >> /etc/sysctl.conf

/sbin/sysctl -p /etc/sysctl.conf
/sbin/sysctl -w net.ipv4.route.flush=1
