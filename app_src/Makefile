CPP_CC = g++

AR = ar
RM = rm

CDEBUG = -g

INC_DIR = ../app_include
OBJ_DIR = ../obj
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(LIB_DIR)/libCGLApp.a

CPP_SRC = \
CGLApp.cpp \
CGLControl2D.cpp \
CGLControl3D.cpp \
CGLDinosaur.cpp \
CGLDispAttr.cpp \
CGLIntSpinBox.cpp \
CGLLabel.cpp \
CGLLight.cpp \
CGLMaterial.cpp \
CGLMenu.cpp \
CGLPushButton.cpp \
CGLTexture.cpp \
CGLTorus.cpp \
CGLUtil.cpp \
CGLWidget.cpp \
CGLWindow.cpp \

CPP_OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(CPP_SRC))

CFLAGS = \
-std=c++14 \
$(CDEBUG) \
-I$(INC_DIR) \
-I. \
-I../../CGeometry3D/include \
-I../../CRenderer/gl_include \
-I../../CRenderer/include \
-I../../CImageLib/include \
-I../../CFont/include \
-I../../CRGBName/include \
-I../../CMath/include \
-I../../CFile/include \
-I../../CConfig/include \
-I../../CTimer/include \
-I../../CArgs/include \
-I../../CStrUtil/include \
-I../../COS/include \
-I../../CUtil/include \

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(LIB_DIR)/libCGLApp.a

.SUFFIXES: .cpp

$(CPP_OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CPP_CC) -c $< -o $(OBJ_DIR)/$*.o $(CFLAGS)

$(LIB_DIR)/libCGLApp.a: $(CPP_OBJS)
	$(AR) crv $(LIB_DIR)/libCGLApp.a $(CPP_OBJS)
