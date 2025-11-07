#ifndef CGLWidget_H
#define CGLWidget_H

#define BOOST_SIGNALS_NO_DEPRECATION_WARNING 1
#define BOOST_BIND_GLOBAL_PLACEHOLDERS 1

#include <CGLWindow.h>
#include <boost/signals2/signal.hpp>
#include <boost/bind.hpp>

#define CGLWidgetMgrInst CGLWidgetMgr::getInstance()

class CGLWidgetMgr {
 private:
  int        id_;
  CGLWidget *root_;

 public:
  static CGLWidgetMgr *getInstance();

  int getId() const;

  CGLWidget *getRoot() const;

  void setRoot(CGLWidget *root);

 private:
  CGLWidgetMgr();
 ~CGLWidgetMgr();
};

class CGLWidget {
 public:
  typedef std::list<CGLWidget *> WidgetList;

 protected:
  typedef boost::signals2::signal<void ()> RegionSignalType;

  struct Region {
    uint             id;
    CIBBox2D         bbox;
    RegionSignalType signal;

    Region(const CIBBox2D &bbox1, RegionSignalType::slot_function_type slot) :
     bbox(bbox1) {
      static uint last_id;

      id = ++last_id;

      signal.connect(slot);
    }
  };

  typedef std::vector<Region *> RegionList;

  CGLWindow   *window_;
  CGLWidget   *parent_;
  WidgetList   children_;
  std::string  name_;
  CIBBox2D     bbox_;
  RegionList   regionList_;
  CRGBA        bg_, fg_;
  CFontPtr     font_;
  bool         visible_;
  bool         autoFill_;

 public:
  CGLWidget(CGLWindow *window, const char *name);

  CGLWidget(CGLWidget *parent, const char *name);

  virtual ~CGLWidget();

  CGLWindow *getWindow() const { return window_; }

  CGLWidget *getParent() const { return parent_; }

  void addChild(CGLWidget *widget);

  uint addRegion(const CIBBox2D &bbox,
                 RegionSignalType::slot_function_type slot);

  void setRegionBBox(uint id, const CIBBox2D &bbox);

  const CRGBA &getBackground() const { return fg_; }
  const CRGBA &getForeground() const { return fg_; }

  void setBackground(const CRGBA &bg);
  void setForeground(const CRGBA &fg);

  CFontPtr getFont() const;

  void setFont(CFontPtr font);

  bool getVisible() const { return visible_; }

  void setVisible(bool visible);

  bool getAutoFill() const { return autoFill_; }

  void setAutoFill(bool autoFill);

  void setGeometry(const CIBBox2D &bbox);

  uint getX() const;
  uint getY() const;

  uint getWidth () const;
  uint getHeight() const;

  CGLRenderer3D *getRenderer() const;

  void exposeEvent();

  void resizeEvent();

  void buttonPressEvent  (const CMouseEvent &);
  void buttonMotionEvent (const CMouseEvent &);
  void buttonReleaseEvent(const CMouseEvent &);

  void keyPressEvent  (const CKeyEvent &);
  void keyReleaseEvent(const CKeyEvent &);

 private:
  void initWidget();

  bool isInside(const CIPoint2D &pos);

 protected:
  virtual void draw();

  virtual void buttonPress  (const CMouseEvent &);
  virtual void buttonMotion (const CMouseEvent &);
  virtual void buttonRelease(const CMouseEvent &);

  virtual void keyPress  (const CKeyEvent &);
  virtual void keyRelease(const CKeyEvent &);
};

#endif
