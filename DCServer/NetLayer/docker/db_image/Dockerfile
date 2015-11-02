FROM blackikeeagle/archlinux


RUN pacman -Syu --needed --noconfirm base-devel
RUN pacman -Syu --needed --noconfirm systemd
RUN pacman -Syu --needed --noconfirm mariadb
RUN pacman -Rcs --noconfirm systemd
RUN pacman -Scc --needed --noconfirm

ADD ./create-mysql-structure.sh /opt/create-mysql-structure.sh
RUN chmod +x /opt/create-mysql-structure.sh
RUN /opt/create-mysql-structure.sh

EXPOSE 3306

VOLUME ["/etc/mysql", "/var/lib/mysql"]

USER mysql

CMD ["/usr/bin/mysqld_safe", "--pid-file=/run/mysqld/mysqld.pid", "--bind-address=0.0.0.0", "--skip-name-resolve", "--datadir=/var/lib/mysql"]
