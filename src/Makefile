CPP_CC = g++

AR = ar
RM = rm

CDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(LIB_DIR)/libCGL.a

CPP_SRC = \
CGLAccumBuffer.cpp \
CGLColorBuffer.cpp \
CGL.cpp \
CGLDisplayList.cpp \
CGL_gl.cpp \
CGL_glu.cpp \
CGL_glut.cpp \
CGL_glx.cpp \
CGLMenu.cpp \
CGLNurbs.cpp \
CGLQuadric.cpp \
CGLTesselator.cpp \
CGLWind.cpp \
\
CXMenu.cpp \
\
CMenuButton.cpp \
CMenu.cpp \
CMenuFormatter.cpp \
CMenuItem.cpp \
CMenuSeparator.cpp \
CMenuTitle.cpp \

CPP_OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(CPP_SRC))

CFLAGS = \
-std=c++14 \
$(CDEBUG) \
-I$(INC_DIR) \
-I. \
-I../../CXLib/include \
-I../../CGeometry3D/include \
-I../../CRenderer/xinclude \
-I../../CRenderer/include \
-I../../CImageLib/include \
-I../../CFont/include \
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
	$(RM) -f $(LIB_DIR)/libCGL.a

.SUFFIXES: .cpp

$(CPP_OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CPP_CC) -c $< -o $(OBJ_DIR)/$*.o $(CFLAGS)

$(LIB_DIR)/libCGL.a: $(CPP_OBJS)
	$(AR) crv $(LIB_DIR)/libCGL.a $(CPP_OBJS)
