class CGLMap2Data {
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
  CGLMap2Data() :
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

class CGLMap2 {
 private:
  double              u1_, u2_, v1_, v2_;
  uint                num_uvalues_, num_vvalues_;
  std::vector<double> values_;

 public:
  CGLMap2() :
   u1_(0), u2_(0), v1_(0), v2_(0) {
  }

  ACCESSOR(U1        , double             , u1);
  ACCESSOR(U2        , double             , u2);
  ACCESSOR(V1        , double             , v1);
  ACCESSOR(V2        , double             , v2);
  ACCESSOR(NumUValues, uint               , num_uvalues);
  ACCESSOR(NumVValues, uint               , num_vvalues);
  ACCESSOR(Values    , std::vector<double>, values)

  void calcBernstein1(double u, double v, double *u1) const {
    double *bu, *bv;

    calcBernstein(u, v, num_uvalues_, num_vvalues_, &bu, &bv);

    *u1 = 0;

    for (uint vv = 0, i = 0; vv < num_vvalues_; ++vv) {
      for (uint uu = 0; uu < num_uvalues_; ++uu, i += 1) {
        *u1 += bu[uu]*bv[vv]*values_[i + 0];
      }
    }
  }

  void calcBernstein2(double u, double v, double *u1, double *u2) const {
    double *bu, *bv;

    calcBernstein(u, v, num_uvalues_, num_vvalues_, &bu, &bv);

    *u1 = 0; *u2 = 0;

    for (uint vv = 0, i = 0; vv < num_vvalues_; ++vv) {
      for (uint uu = 0; uu < num_uvalues_; ++uu, i += 2) {
        *u1 += bu[uu]*bv[vv]*values_[i + 0];
        *u2 += bu[uu]*bv[vv]*values_[i + 1];
      }
    }
  }

  void calcBernstein3(double u, double v, double *u1, double *u2, double *u3) const {
    double *bu, *bv;

    calcBernstein(u, v, num_uvalues_, num_vvalues_, &bu, &bv);

    *u1 = 0; *u2 = 0; *u3 = 0;

    for (uint vv = 0, i = 0; vv < num_vvalues_; ++vv) {
      for (uint uu = 0; uu < num_uvalues_; ++uu, i += 3) {
        *u1 += bu[uu]*bv[vv]*values_[i + 0];
        *u2 += bu[uu]*bv[vv]*values_[i + 1];
        *u3 += bu[uu]*bv[vv]*values_[i + 2];
      }
    }
  }

  void calcBernstein4(double u, double v, double *u1, double *u2, double *u3, double *u4) const {
    double *bu, *bv;

    calcBernstein(u, v, num_uvalues_, num_vvalues_, &bu, &bv);

    *u1 = 0; *u2 = 0; *u3 = 0; *u4 = 0;

    for (uint vv = 0, i = 0; vv < num_vvalues_; ++vv) {
      for (uint uu = 0; uu < num_uvalues_; ++uu, i += 4) {
        *u1 += bu[uu]*bv[vv]*values_[i + 0];
        *u2 += bu[uu]*bv[vv]*values_[i + 1];
        *u3 += bu[uu]*bv[vv]*values_[i + 2];
        *u4 += bu[uu]*bv[vv]*values_[i + 3];
      }
    }
  }

  void calcBernstein(double u, double v, uint num_uvalues, uint num_vvalues,
                     double **bu1, double **bv1) const {
    enum { MAX_VALUES = 32 };

    static double uu[MAX_VALUES + 1];
    static double vv[MAX_VALUES + 1];
    static double ww[MAX_VALUES + 1];
    static double xx[MAX_VALUES + 1];
    static double bu[MAX_VALUES + 1];
    static double bv[MAX_VALUES + 1];

    assert(num_uvalues <= MAX_VALUES && num_vvalues <= MAX_VALUES);

    double up = (u - u1_)/(u2_ - u1_);
    double vp = (v - v1_)/(v2_ - v1_);
    double wp = 1 - up;
    double xp = 1 - vp;

    uu[0              ] = 1;
    ww[num_uvalues - 1] = 1;

    for (uint i = 1, j = num_uvalues - 2; i < num_uvalues; ++i, --j) {
      uu[i] = uu[i - 1]*up;
      ww[j] = ww[j + 1]*wp;
    }

    for (uint i = 0; i < num_uvalues; ++i)
      bu[i] = CMathGen::binomialCoeff(num_uvalues - 1, i)*uu[i]*ww[i];

    *bu1 = bu;

    vv[0              ] = 1;
    xx[num_vvalues - 1] = 1;

    for (uint i = 1, j = num_vvalues - 2; i < num_vvalues; ++i, --j) {
      vv[i] = vv[i - 1]*vp;
      xx[j] = xx[j + 1]*xp;
    }

    for (uint i = 0; i < num_vvalues; ++i)
      bv[i] = CMathGen::binomialCoeff(num_vvalues - 1, i)*vv[i]*xx[i];

    *bv1 = bv;
  }
};

class CGLMapGrid2 {
 private:
  int    un_, vn_;
  double u1_, u2_, v1_, v2_;

 public:
  CGLMapGrid2() :
   un_(0), vn_(0), u1_(0), u2_(0), v1_(0), v2_(0) {
  }

  ACCESSOR(UN, int   , un);
  ACCESSOR(U1, double, u1);
  ACCESSOR(U2, double, u2);
  ACCESSOR(VN, int   , vn);
  ACCESSOR(V1, double, v1);
  ACCESSOR(V2, double, v2);
};
