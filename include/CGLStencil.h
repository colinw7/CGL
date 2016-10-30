class CGLStencil {
 private:
  bool enabled_;

  uint func_;
  uint ref_;
  uint mask_;

  uint fail_;
  uint zfail_;
  uint zpass_;

 public:
  CGLStencil() :
   enabled_(false), func_(GL_ALWAYS), ref_(0), mask_(0xFF),
   fail_(GL_KEEP), zfail_(GL_KEEP), zpass_(GL_KEEP) {
  }

  ACCESSOR(Enabled, bool, enabled)

  ACCESSOR(Func, uint, func)
  ACCESSOR(Ref , uint, ref )
  ACCESSOR(Mask, uint, mask)

  ACCESSOR(Fail , uint, fail)
  ACCESSOR(ZFail, uint, zfail)
  ACCESSOR(ZPass, uint, zpass)

  bool testValue(uint value) const {
    switch (func_) {
      case GL_NEVER:
        return false;
      case GL_LESS:
        return (ref_ & mask_) <  (value & mask_);
      case GL_LEQUAL:
        return (ref_ & mask_) <= (value & mask_);
      case GL_GREATER:
        return (ref_ & mask_) >  (value & mask_);
      case GL_GEQUAL:
        return (ref_ & mask_) <= (value & mask_);
      case GL_EQUAL:
        return (ref_ & mask_) == (value & mask_);
      case GL_NOTEQUAL:
        return (ref_ & mask_) != (value & mask_);
      case GL_ALWAYS:
      default:
        return true;
    }
  }

  uint failValue(uint old_value) const {
    switch (fail_) {
      case GL_KEEP:
        return old_value;
      case GL_ZERO:
        return 0;
      case GL_REPLACE:
        return ref_;
      case GL_INCR:
        return (old_value < 0xFF ? old_value + 1 : 0xFF);
      case GL_DECR:
        return (old_value > 0x00 ? old_value - 1 : 0x00);
      case GL_INVERT:
        return (old_value ^ 0xFF);
      default:
        return old_value;
    }
  }

  uint zfailValue(uint old_value) const {
    switch (zfail_) {
      case GL_KEEP:
        return old_value;
      case GL_ZERO:
        return 0;
      case GL_REPLACE:
        return ref_;
      case GL_INCR:
        return (old_value < 0xFF ? old_value + 1 : 0xFF);
      case GL_DECR:
        return (old_value > 0x00 ? old_value - 1 : 0x00);
      case GL_INVERT:
        return (old_value ^ 0xFF);
      default:
        return old_value;
    }
  }

  uint zpassValue(uint old_value) const {
    switch (zpass_) {
      case GL_KEEP:
        return old_value;
      case GL_ZERO:
        return 0;
      case GL_REPLACE:
        return ref_;
      case GL_INCR:
        return (old_value < 0xFF ? old_value + 1 : 0xFF);
      case GL_DECR:
        return (old_value > 0x00 ? old_value - 1 : 0x00);
      case GL_INVERT:
        return (old_value ^ 0xFF);
      default:
        return old_value;
    }
  }
};
