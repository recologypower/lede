#!/bin/bash

cp -rf ./newu_patch/dts/rk356x.dtsi build_dir/target-aarch64_generic_musl/linux-rockchip_armv8/linux-5.19.1/arch/arm64/boot/dts/rockchip/ 
cp -rf ./newu_patch/phy/*.c ./build_dir/target-aarch64_generic_musl/linux-rockchip_armv8/linux-5.19.1/drivers/phy/rockchip/ 
#cp -rf ./newu_patch/rockchip/* ./build_dir/target-aarch64_generic_musl/linux-rockchip_armv8/linux-5.19.1/include/soc/rockchip/
#cp -rf ./newu_patch/stmmac/* ./build_dir/target-aarch64_generic_musl/linux-rockchip_armv8/linux-5.19.1/drivers/net/ethernet/stmicro/stmmac
cp -rf ./build_dir/toolchain-aarch64_generic_gcc-8.4.0_musl/linux-5.19.1/drivers/spi/spidev.c ./build_dir/target-aarch64_generic_musl/linux-rockchip_armv8/linux-5.19.1/drivers/spi/

make V=99 -j8
