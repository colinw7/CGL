CC = g++
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../../include
OBJ_DIR = .
LIB_DIR = ../../lib
BIN_DIR = ../../bin

all: $(BIN_DIR)/glut_atlantis

SRC = \
glut_atlantis.cpp \
glut_dolphin.cpp \
glut_shark.cpp \
glut_swim.cpp \
glut_whale.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

LIBS = \
-lCGL -lCXLib -lCConfig -lCImageLib -lCFont -lCTimer -lCArgs \
-lCFile -lCFileUtil -lCUtil -lCOS -lCStrUtil -lCRegExp \
-lXt -lX11 -lfreetype -lpng -ljpeg -ltre

CPPFLAGS = \
-I../../include \
-I../../../CGL/include \
-I../../../CXLib/include \
-I../../../CImageLib/include \
-I../../../CFont/include \
-I../../../CArgs/include \
-I../../../CFile/include \
-I../../../COS/include \
-I../../../CStrUtil/include \
-I../../../CMath/include \
-I../../../CUtil/include \
-I../../../CTimer/include \
-I.

LFLAGS = \
-L$(LIB_DIR) \
-L../../../CGL/lib \
-L../../../CXLib/lib \
-L../../../CImageLib/lib \
-L../../../CFont/lib \
-L../../../CArgs/lib \
-L../../../CFile/lib \
-L../../../CFileUtil/lib \
-L../../../COS/lib \
-L../../../CStrUtil/lib \
-L../../../CConfig/lib \
-L../../../CMath/lib \
-L../../../CUtil/lib \
-L../../../CTimer/lib \
-L../../../CRegExp/lib \

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(BIN_DIR)/glut_atlantis

.SUFFIXES: .cpp

.cpp.o:
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(BIN_DIR)/glut_atlantis: $(OBJS) $(LIB_DIR)/libCGL.a
	$(CC) $(LDEBUG) -o $(BIN_DIR)/glut_atlantis $(OBJS) $(LFLAGS) $(LIBS)
