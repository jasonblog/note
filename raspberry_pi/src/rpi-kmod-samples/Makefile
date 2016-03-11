MODULES += modules/kmod-basic
MODULES += modules/kmod-clarg
MODULES += modules/kmod-gpio_inpirq
MODULES += modules/kmod-gpio_inpthrd
MODULES += modules/kmod-gpio_outptask
MODULES += modules/kmod-gpio_outptimer
MODULES += modules/kmod-gpio_output
MODULES += modules/kmod-pdev
MODULES += modules/kmod-tasklet

USRSPC += usrspc/evtest

all: pack 

.PHONY: build
build: build-kernel build-modules build-usrspc

.PHONY: clean
clean: clean-kernel clean-modules clean-usrspc
	rm rpi-modtest-sysroot.tar.bz2
	rm modules.order

.PHONY: distclean
distclean: clean distclean-kernel distclean-modules

.PHONY: install
install: build install-kernel install-modules

dl-kernel:
	if ! [ -d linux-rpi-3.6.y ]; then wget https://github.com/raspberrypi/linux/archive/rpi-3.6.y.tar.gz && tar -zxvf rpi-3.6.y.tar.gz; fi
	if ! [ -s ./linux ]; then ln -s linux-rpi-3.6.y linux; for i in ./patches/*.patch; do (cd ./linux && patch -p1 < ../$$i); done; fi
	if ! [ -f ./linux/localversion ]; then echo "-modtest" > ./linux/localversion; fi

clone-kernel:
	if ! [ -d ./linux-git ]; then git clone https://github.com/raspberrypi/linux.git linux-git && cd linux-git && git checkout rpi-3.6.y; fi
	if ! [ -s ./linux ]; then ln -s linux-git linux; for i in ./patches/*.patch; do (cd ./linux && patch -p1 < ../$$i); done; fi
	if ! [ -f ./linux/localversion ]; then echo "modtest" > ./linux/localversion; fi

build-kernel: dl-kernel
	if ! [ -f ./linux/.config ]; then (cd ./linux && cp arch/arm/configs/bcmrpi_defconfig .config); fi
	(cd ./linux && make -j2)

build-modules:
	for i in $(MODULES); do (cd $$i && make); done

build-usrspc:
	for i in $(USRSPC); do (cd $$i && make); done

clean-kernel:
	(cd ./linux && make clean)

clean-modules:
	for i in $(MODULES); do (cd $$i && make clean); done

clean-usrspc:
	for i in $(USRSPC); do (cd $$i && make clean); done

distclean-kernel:
	(cd ./linux && make distclean)
	
distclean-modules:
	if [ -d ./sysroot ]; then rm -r ./sysroot; fi

install-kernel:
	(cd ./linux && make INSTALL_MOD_PATH=../sysroot modules_install)
	mkdir -p ./sysroot/boot
	cp ./linux/arch/arm/boot/Image ./sysroot/boot/kernel.img

install-modules:
	for i in $(MODULES); do (cd $$i && make modules_install); done

install-usrspc:
	for i in $(USRSPC); do (cd $$i && make install); done

pack: install
	(cd ./sysroot && tar -jcvf ../rpi-modtest-sysroot.tar.bz2 *)

