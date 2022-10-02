class CGLMap1Data {
 private:
  bool gen_color_;
  bool gen_index_;
  bool gen_normal_;
  bool gen_texture1_;
  bool gen_texture2_;
  bool gen_texture3_;
  bool gen_texture4_;
  bool gen_vertex3_;
  bool gen_vertex4_;

 public:
  CGLMap1Data() :
   gen_color_   (false),
   gen_index_   (false),
   gen_normal_  (false),
   gen_texture1_(false),
   gen_texture2_(false),
   gen_texture3_(false),
   gen_texture4_(false),
   gen_vertex3_ (false),
   gen_vertex4_ (false) {
  }

  ACCESSOR(GenColor   , bool, gen_color   )
  ACCESSOR(GenIndex   , bool, gen_index   )
  ACCESSOR(GenNormal  , bool, gen_normal  )
  ACCESSOR(GenTexture1, bool, gen_texture1)
  ACCESSOR(GenTexture2, bool, gen_texture2)
  ACCESSOR(GenTexture3, bool, gen_texture3)
  ACCESSOR(GenTexture4, bool, gen_texture4)
  ACCESSOR(GenVertex3 , bool, gen_vertex3 )
  ACCESSOR(GenVertex4 , bool, gen_vertex4 )
};

class CGLMap1 {
 private:
  double              u1_, u2_;
  uint                num_uvalues_;
  std::vector<double> values_;

 public:
  CGLMap1() :
   u1_(0), u2_(0) {
  }

  ACCESSOR(U1        , double             , u1)
  ACCESSOR(U2        , double             , u2)
  ACCESSOR(NumUValues, uint               , num_uvalues)
  ACCESSOR(Values    , std::vector<double>, values)

  void calcBernstein1(double u, double *u1) const {
    double *bb = calcBernstein(u, num_uvalues_);

    *u1 = 0;

    for (uint i = 0, j = 0; j < num_uvalues_; i += 1, ++j) {
      *u1 += bb[j]*values_[i + 0];
    }
  }

  void calcBernstein2(double u, double *u1, double *u2) const {
    double *bb = calcBernstein(u, num_uvalues_);

    *u1 = 0; *u2 = 0;

    for (uint i = 0, j = 0; j < num_uvalues_; i += 2, ++j) {
      *u1 += bb[j]*values_[i + 0];
      *u2 += bb[j]*values_[i + 1];
    }
  }

  void calcBernstein3(double u, double *u1, double *u2,
                      double *u3) const {
    double *bb = calcBernstein(u, num_uvalues_);

    *u1 = 0; *u2 = 0; *u3 = 0;

    for (uint i = 0, j = 0; j < num_uvalues_; i += 3, ++j) {
      *u1 += bb[j]*values_[i + 0];
      *u2 += bb[j]*values_[i + 1];
      *u3 += bb[j]*values_[i + 2];
    }
  }

  void calcBernstein4(double u, double *u1, double *u2,
                      double *u3, double *u4) const {
    double *bb = calcBernstein(u, num_uvalues_);

    *u1 = 0; *u2 = 0; *u3 = 0; *u4 = 0;

    for (uint i = 0, j = 0; j < num_uvalues_; i += 4, ++j) {
      *u1 += bb[j]*values_[i + 0];
      *u2 += bb[j]*values_[i + 1];
      *u3 += bb[j]*values_[i + 2];
      *u4 += bb[j]*values_[i + 3];
    }
  }

  double *calcBernstein(double u, uint num_values) const {
    enum { MAX_VALUES = 32 };

    static double uu[MAX_VALUES + 1];
    static double ww[MAX_VALUES + 1];
    static double bb[MAX_VALUES + 1];

    assert(num_values <= MAX_VALUES);

    double up = (u - u1_)/(u2_ - u1_);
    double wp = 1 - up;

    uu[0             ] = 1;
    ww[num_values - 1] = 1;

    for (uint i = 1, j = num_values - 2; i < num_values; ++i, --j) {
      uu[i] = uu[i - 1]*up;
      ww[j] = ww[j + 1]*wp;
    }

    for (uint i = 0; i < num_values; ++i)
      bb[i] = double(CMathGen::binomialCoeff(num_values - 1, i))*uu[i]*ww[i];

    return bb;
  }
};

class CGLMapGrid1 {
 private:
  int    un_;
  double u1_, u2_;

 public:
  CGLMapGrid1() :
   un_(0), u1_(0), u2_(0) {
  }

  ACCESSOR(UN, int   , un)
  ACCESSOR(U1, double, u1)
  ACCESSOR(U2, double, u2)
};
