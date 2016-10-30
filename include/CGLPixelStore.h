class CGLPixelStore {
 private:
  uint unpack_alignment_;
  uint unpack_row_length_;
  uint pack_alignment_;
  uint pack_row_length_;

 public:
  CGLPixelStore() :
   unpack_alignment_(1), unpack_row_length_(0),
   pack_alignment_  (1), pack_row_length_  (0) {
  }

  ACCESSOR(UnpackAlignment, uint, unpack_alignment )
  ACCESSOR(UnpackRowLength, uint, unpack_row_length)
  ACCESSOR(PackAlignment  , uint, pack_alignment   )
  ACCESSOR(PackRowLength  , uint, pack_row_length  )
};
