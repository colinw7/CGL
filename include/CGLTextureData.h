#include <CStrUtil.h>
#include <CImageMgr.h>

struct CGLTextureKey {
  uint ind;
  uint level;

  CGLTextureKey(uint ind1, uint level1) :
   ind(ind1), level(level1) {
  }

  bool operator<(const CGLTextureKey &key) const {
    return (ind < key.ind || (ind == key.ind && level < key.level));
  }
};

class CGLTexture1Data {
 private:
  typedef std::map<CGLTextureKey,CImagePtr> TextureMap;
  typedef std::map<uint,uint>               TextureDepth;

  uint         current_ind_;
  bool         enabled_;
  TextureMap   texture_map_;
  TextureDepth texture_depth_;
  uint         wrap_s_;
  uint         mag_filter_;
  uint         min_filter_;
  CRGBA        border_color_;

 public:
  CGLTexture1Data() :
   current_ind_(0), enabled_(false),
   wrap_s_(GL_REPEAT),
   mag_filter_(GL_LINEAR), min_filter_(GL_LINEAR),
   border_color_(0,0,0,0) {
  }

  ACCESSOR(CurrentInd , uint , current_ind)
  ACCESSOR(Enabled    , bool , enabled)
  ACCESSOR(WrapS      , uint , wrap_s)
  ACCESSOR(MagFilter  , uint , mag_filter)
  ACCESSOR(MinFilter  , uint , min_filter)
  ACCESSOR(BorderColor, CRGBA, border_color)

  const CImagePtr &getTexture(const CGLTextureKey &key) {
    if (texture_map_.find(key) == texture_map_.end() || ! texture_map_[key]) {
      CImageNameSrc src("CGL/texture1/" +
                        CStrUtil::toString(key.ind) + "/" +
                        CStrUtil::toString(key.level));

      texture_map_[key] = CImageMgrInst->createImage(src);
    }

    return texture_map_[key];
  }

  void setTexture(const CGLTextureKey &key, const CImagePtr &image) {
    texture_map_[key] = image;

    if (texture_depth_.find(key.ind) == texture_depth_.end())
      texture_depth_[key.ind] = key.level;
    else
      texture_depth_[key.ind] = std::max(texture_depth_[key.ind], key.level);
  }

  uint getTextureDepth(uint ind) {
    return texture_depth_[ind];
  }
};

class CGLTexture2Data {
 private:
  struct ImageFormat {
    CImagePtr image;
    int       format;

    ImageFormat() { }

    ImageFormat(CImagePtr image1, int format1) :
     image(image1), format(format1) {
    }
  };

  typedef std::map<CGLTextureKey,ImageFormat> TextureMap;
  typedef std::map<uint,uint>                 TextureDepth;

  uint         current_ind_;
  bool         enabled_;
  TextureMap   texture_map_;
  TextureDepth texture_depth_;
  uint         wrap_s_, wrap_t_;
  uint         mag_filter_;
  uint         min_filter_;
  CRGBA        border_color_;

 public:
  CGLTexture2Data() :
   current_ind_(0), enabled_(false),
   wrap_s_(GL_REPEAT), wrap_t_(GL_REPEAT),
   mag_filter_(GL_LINEAR), min_filter_(GL_LINEAR),
   border_color_(0,0,0,0) {
  }

  ACCESSOR(CurrentInd , uint , current_ind)
  ACCESSOR(Enabled    , bool , enabled)
  ACCESSOR(WrapS      , uint , wrap_s)
  ACCESSOR(WrapT      , uint , wrap_t)
  ACCESSOR(MagFilter  , uint , mag_filter)
  ACCESSOR(MinFilter  , uint , min_filter)
  ACCESSOR(BorderColor, CRGBA, border_color)

  const CImagePtr &getTextureImage(const CGLTextureKey &key) {
    if (texture_map_.find(key) == texture_map_.end() ||
        ! texture_map_[key].image) {
      CImageNameSrc src("CGL/texture2/" +
                        CStrUtil::toString(key.ind) + "/" +
                        CStrUtil::toString(key.level));

      CImagePtr image = CImageMgrInst->createImage(src);

      texture_map_[key] = ImageFormat(image, GL_RGBA);
    }

    return texture_map_[key].image;
  }

  int getTextureFormat(const CGLTextureKey &key) {
    if (texture_map_.find(key) == texture_map_.end())
      return GL_RGBA;

    return texture_map_[key].format;
  }

  void setTexture(const CGLTextureKey &key,
                  const CImagePtr &image, int format) {
    texture_map_[key] = ImageFormat(image, format);

    if (texture_depth_.find(key.ind) == texture_depth_.end())
      texture_depth_[key.ind] = key.level;
    else
      texture_depth_[key.ind] = std::max(texture_depth_[key.ind], key.level);
  }

  uint getTextureDepth(uint ind) {
    return texture_depth_[ind];
  }
};

class CGLTexture3Data {
 public:
  typedef std::vector<CImagePtr> CImagePtrStack;

 private:
  typedef std::map<CGLTextureKey,CImagePtrStack> TextureMap;
  typedef std::map<uint,uint>                    TextureDepth;

  uint         current_ind_;
  bool         enabled_;
  TextureMap   texture_map_;
  TextureDepth texture_depth_;
  uint         wrap_s_, wrap_t_, wrap_r_;
  uint         mag_filter_;
  uint         min_filter_;
  CRGBA        border_color_;

 public:
  CGLTexture3Data() :
   current_ind_(0), enabled_(false),
   wrap_s_(GL_REPEAT), wrap_t_(GL_REPEAT), wrap_r_(GL_REPEAT),
   mag_filter_(GL_LINEAR), min_filter_(GL_LINEAR),
   border_color_(0,0,0,0) {
  }

  ACCESSOR(CurrentInd , uint , current_ind)
  ACCESSOR(Enabled    , bool , enabled)
  ACCESSOR(WrapS      , uint , wrap_s)
  ACCESSOR(WrapT      , uint , wrap_t)
  ACCESSOR(WrapR      , uint , wrap_r)
  ACCESSOR(MagFilter  , uint , mag_filter)
  ACCESSOR(MinFilter  , uint , min_filter)
  ACCESSOR(BorderColor, CRGBA, border_color)

  const CImagePtrStack &getTexture(const CGLTextureKey &key) {
    return texture_map_[key];
  }

  const CImagePtrStack &getTexture(const CGLTextureKey &key, uint depth) {
    CImagePtrStack &stack = texture_map_[key];

    stack.resize(depth);

    for (uint z = 0; z < depth; z++) {
      if (! stack[z]) {
        CImageNameSrc src("CGL/texture3/" +
                          CStrUtil::toString(key.ind) + "/" +
                          CStrUtil::toString(key.level) + "/" +
                          CStrUtil::toString(z + 1));

        stack[z] = CImageMgrInst->createImage(src);
      }
    }

    return stack;
  }

  void setTexture(const CGLTextureKey &key, const CImagePtrStack &image) {
    texture_map_[key] = image;

    if (texture_depth_.find(key.ind) == texture_depth_.end())
      texture_depth_[key.ind] = key.level;
    else
      texture_depth_[key.ind] = std::max(texture_depth_[key.ind], key.level);
  }

  uint getTextureDepth(uint ind) {
    return texture_depth_[ind];
  }
};
