struct CGLSelectBuffer {
  typedef std::vector<uint> NameList;

  struct Hit {
    uint     min_z, max_z;
    NameList name_list;
  };

  typedef std::vector<Hit> HitList;

  int      size;
  uint    *buffer;
  int      pos;
  HitList  hit_list;
  uint     num_hits;

  CGLSelectBuffer() :
    size(0), buffer(NULL), pos(0), num_hits(0) {
  }

  CGLSelectBuffer(int size1, uint *buffer1) :
    size(size1), buffer(buffer1), pos(0) {
  }

  void addHit(uint min_z1, uint max_z1, const NameList &names) {
    Hit hit;

    hit.min_z     = min_z1;
    hit.max_z     = max_z1;
    hit.name_list = names;

    hit_list.push_back(hit);
  }

  void setData() {
    num_hits = hit_list.size();

    HitList::iterator p1 = hit_list.begin();
    HitList::iterator p2 = hit_list.end  ();

    for ( ; p1 != p2; ++p1) {
      uint num_names = (*p1).name_list.size();

      append(&num_names, 1);

      append(&(*p1).min_z, 1);
      append(&(*p1).max_z, 1);

      NameList::iterator pn1 = (*p1).name_list.begin();
      NameList::iterator pn2 = (*p1).name_list.end  ();

      for ( ; pn1 != pn2; ++pn1)
        append(&(*pn1), 1);
    }
  }

  void reset() {
    pos = 0;

    hit_list.clear();

    num_hits = 0;
  }

  void append(uint *data, int num) {
    for (int i = 0; i < num; ++i)
      if (buffer && pos < size)
        buffer[pos++] = data[i];
  }
};
