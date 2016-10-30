#ifndef CFOG_DATA_H
#define CFOG_DATA_H

enum CFogType {
  CFOG_TYPE_EXP,
  CFOG_TYPE_EXP2,
  CFOG_TYPE_LINEAR
};

struct CFogData {
  CFogType type;
  double   density;
  double   start, end;
  double   index;
  CRGBA    color;

  CFogData() :
   type   (CFOG_TYPE_EXP),
   density(1),
   start  (0),
   end    (1),
   index  (0),
   color  (0,0,0,0) {
  }
};

#endif
