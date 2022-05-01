CDEBUG = -g

SHADER_DIR = /home/colinw/dev/progs/gl/src/CGLNewModel/shaders

PROG = CGLNewModel

NO_WARN_CAST = 1

CPP_SRC = \
CGLNewModel.cpp \

EXTRA_CFLAGS = \
-DSHADER_DIR="$(SHADER_DIR)" \
-I$(INC_DIR)/CImportModel \
-I$(INC_DIR)/CGeometry3D \
-I$(INC_DIR)/CMaterial \
-I$(INC_DIR)/CGLShader \
-I$(INC_DIR)/CImageLib \
-I$(INC_DIR)/CFont \
-I$(INC_DIR)/CFile \
-I$(INC_DIR)/CFileType \
-I$(INC_DIR)/CMath \
-I$(INC_DIR)/CStrUtil \
-I$(INC_DIR)/CUtil \
-I$(INC_DIR)/COS \

LIBS = \
-L./lib \
-lCImportModel -lCGeometry3D \
-lCGLShader -lCImageLib -lCFile -lCFileParse -lCFileUtil -lCTempFile \
-lCMath -lCStrUtil -lCPrintF -lCRegExp -lCRGBUtil -lCUtil -lCOS \
-lglad -lglfw3 -lassimp -lGL -lGLU \
-lX11 -lpthread -lXrandr -lXi -ldl \
-ljpeg -lpng -ltre

include $(MAKE_DIR)/bin.mk
