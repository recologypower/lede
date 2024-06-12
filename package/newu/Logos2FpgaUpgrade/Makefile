#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=Logos2FpgaUpgrade
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/Logos2FpgaUpgrade
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Logos2FpgaUpgrade for Newu-TwoWayCpe
  DEPENDS:=@DISPLAY_SUPPORT
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/Logos2FpgaUpgrade/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/FpgaUpgrade $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,Logos2FpgaUpgrade))
