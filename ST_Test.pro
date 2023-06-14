TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += object_parallel_to_source #ignore same file names which are in different directories


#include system <pthread> lib
#QMAKE_CXXFLAGS += -std=c++0x -pthread
QMAKE_CXXFLAGS += -std=c++11 -pthread -libi2c

#DEFINES += STMVL53L5CX_KERNEL
DEFINES += LAW_RAM

INCLUDEPATH += ./platform \
               ./vl53L5CX-driver/inc \
               ./vl6180x-driver/inc \
/opt/fsl-imx-xwayland/4.14-sumo/sysroots/x86_64-pokysdk-linux/usr/lib/aarch64-poky-linux/gcc/aarch64-poky-linux/7.3.0/include \
/opt/fsl-imx-xwayland/4.14-sumo/sysroots/aarch64-poky-linux/usr/include \
/opt/fsl-imx-xwayland/4.14-sumo/sysroots/aarch64-poky-linux/usr/include/c++/7.3.0 \
/opt/fsl-imx-xwayland/4.14-sumo/sysroots/aarch64-poky-linux/usr/lib
/opt/fsl-imx-xwayland/4.14-sumo/sysroots/aarch64-poky-linux/usr/include/linux \


LIBS += -L:/opt/fsl-imx-xwayland/4.14-sumo/sysroots/aarch64-poky-linux/usr/lib

SOURCES += main.c \
    platform/platform.c \
    vl53L5CX-driver/src/vl53l5cx_api.c \
    vl53L5CX-driver/src/vl53l5cx_plugin_detection_thresholds.c \
    vl53L5CX-driver/src/vl53l5cx_plugin_motion_indicator.c \
    vl53L5CX-driver/src/vl53l5cx_plugin_xtalk.c \
    vl6180x-driver/src/vl6180x_api.c \
    vl6180x-driver/src/vl6180x_platform.c \
    vl6180x-driver/src/vl6180x_i2c.c

HEADERS += \
    platform/platform.h \
    platform/types.h \
    vl53L5CX-driver/inc/vl53l5cx_api.h \
    vl53L5CX-driver/inc/vl53l5cx_buffers.h \
    vl53L5CX-driver/inc/vl53l5cx_plugin_detection_thresholds.h \
    vl53L5CX-driver/inc/vl53l5cx_plugin_motion_indicator.h \
    vl53L5CX-driver/inc/vl53l5cx_plugin_xtalk.h \
    vl6180x-driver/inc/vl6180x_api.h \
    vl6180x-driver/inc/vl6180x_cfg.h \
    vl6180x-driver/inc/vl6180x_def.h \
    vl6180x-driver/inc/vl6180x_i2c.h \
    vl6180x-driver/inc/vl6180x_platform.h \
    vl6180x-driver/inc/vl6180x_types.h

target.path = /home/root/ # exemple: target.path = /home/root/demo
INSTALLS += target
