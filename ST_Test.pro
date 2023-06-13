TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

#DEFINES += STMVL53L5CX_KERNEL
DEFINES += LAW_RAM

INCLUDEPATH += ./platform \
               ./vl53L5CX-driver/inc \
/opt/fsl-imx-xwayland/4.14-sumo/sysroots/x86_64-pokysdk-linux/usr/lib/aarch64-poky-linux/gcc/aarch64-poky-linux/7.3.0/include \
/opt/fsl-imx-xwayland/4.14-sumo/sysroots/aarch64-poky-linux/usr/include \
/opt/fsl-imx-xwayland/4.14-sumo/sysroots/aarch64-poky-linux/usr/include/c++/7.3.0 \
/opt/fsl-imx-xwayland/4.14-sumo/sysroots/aarch64-poky-linux/usr/lib
#/opt/fsl-imx-xwayland/4.14-sumo/sysroots/aarch64-poky-linux/usr/include/linux \


LIBS += -L:/opt/fsl-imx-xwayland/4.14-sumo/sysroots/aarch64-poky-linux/usr/lib

SOURCES += main.c \
    platform/platform.c \
    vl53L5CX-driver/src/vl53l5cx_api.c \
    vl53L5CX-driver/src/vl53l5cx_plugin_detection_thresholds.c \
    vl53L5CX-driver/src/vl53l5cx_plugin_motion_indicator.c \
    vl53L5CX-driver/src/vl53l5cx_plugin_xtalk.c

HEADERS += \
    platform/platform.h \
    platform/types.h \
    vl53L5CX-driver/inc/vl53l5cx_api.h \
    vl53L5CX-driver/inc/vl53l5cx_buffers.h \
    vl53L5CX-driver/inc/vl53l5cx_plugin_detection_thresholds.h \
    vl53L5CX-driver/inc/vl53l5cx_plugin_motion_indicator.h \
    vl53L5CX-driver/inc/vl53l5cx_plugin_xtalk.h

target.path = /home/root/ # exemple: target.path = /home/root/demo
INSTALLS += target
