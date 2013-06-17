#ifndef LOGVIEW_H
#define LOGVIEW_H

#include "QtWidgets/QGraphicsView"
#include "QtWidgets/QGraphicsItem"
#include "XEventLogger"
#include "XDebugLogger.h"
#include "XUnorderedMap"
#include "QtCore/QPersistentModelIndex"
#include "XShared"

class QGraphicsScene;
class QAbstractItemModel;

class EventItem;
class ThreadItem;
class MomentItem;
class DurationItem;
class InfoItem;
class TimelineItem;
class ThreadsItem;

class LogView : public QGraphicsView
  {
  Q_OBJECT

public:
  typedef Eks::DebugLogger::DebugLocationWithData Location;

  LogView(QObject *model);

  const Eks::Time &start() const;

  void selectEvent(EventItem *item, const QPointF &scenePos);
  float xOffset() const { return _offset; }
  float timeToX(const Eks::Time &t) const;
  float timeToXNoOffset(const Eks::Time &t) const;
  Eks::Time timeFromX(float x, bool offset) const;

protected:
  void timerEvent(QTimerEvent *) X_OVERRIDE;

signals:
  void timeConversionChanged();

private:
  void addDuration(
      const xsize id,
      const Eks::Time &t,
      const QString &disp,
      const xuint64 thr,
      const Location *l);
  void addMoment(
      const Eks::Time &t,
      const QString &disp,
      const xuint64 thr,
      const Location *l);
  void updateEnd(
      const xsize id,
      const xuint64 thr,
      const Eks::Time &t);

  void wheelEvent(QWheelEvent *event) X_OVERRIDE;
  void mouseMoveEvent(QMouseEvent *event) X_OVERRIDE;
  void mousePressEvent(QMouseEvent *event) X_OVERRIDE;
  void mouseReleaseEvent(QMouseEvent *event) X_OVERRIDE;

  typedef QPair<xuint64, xsize> OpenEvent;

  struct ActiveDuration
    {
    ActiveDuration(ThreadItem *t=0, DurationItem *d=0) : thread(t), duration(d) { }
    ThreadItem *thread;
    DurationItem *duration;
    };


  Eks::UnorderedMap<OpenEvent, ActiveDuration> _openEvents;
  QGraphicsScene _scene;

  TimelineItem *_timelineRoot;

  EventItem *_selected;
  InfoItem *_info;

  float _scale;
  float _offset;

  float _lastDragX;
  bool _dragging;

  Eks::Time _min;
  Eks::Time _max;
  };

class TimelineItem : public QGraphicsObject
  {
  Q_OBJECT

public:
  TimelineItem(LogView *log);

  QRectF boundingRect() const X_OVERRIDE;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) X_OVERRIDE;

  ThreadsItem *threads();
  void setCurrentTime(const Eks::Time &);

public slots:
  void timeConversionChanged();
  void layoutThreads();

protected:
  void timerEvent(QTimerEvent *event) X_OVERRIDE;

private:
  ThreadsItem *_threads;
  LogView *_log;
  bool _pendingLayoutThread;
  };

class EventContainer
  {
public:
  typedef Eks::Vector<Eks::SharedPointer<DurationItem>> DurationVector;
  typedef Eks::Vector<Eks::SharedPointer<MomentItem>> MomentVector;

XProperties:
  XProperty(Eks::Time, start, setStart);
  XProperty(Eks::Time, end, setEnd);

  XROProperty(xsize, maxChildStackSize);
  XROByRefProperty(DurationVector, durationChildren);
  XROByRefProperty(MomentVector, momentChildren);

public:
  void addMoment(MomentItem *item);
  void addDuration(DurationItem *item);
  };

class EventItem : public Eks::detail::SharedData
  {
XProperties:
  XProperty(const LogView::Location *, location, setLocation);
  XByRefProperty(QString, display, setDisplay);
  XProperty(bool, isSelected, setSelected);

public:
  EventItem();

  virtual QString formattedTime(const LogView *thr) = 0;
  virtual void paint(const ThreadItem *t, QPainter *p, const Eks::Time &begin, const Eks::Time &end) = 0;

  Eks::Time relativeTime(const LogView *thr, const Eks::Time &t) const;
  };

class ThreadItem : public QGraphicsObject
  {
  Q_OBJECT

XProperties:
  XROProperty(LogView *, log);
  XROByRefProperty(Eks::Time, currentTime);

public:
  ThreadItem(LogView *l, QGraphicsItem *parent);

  float timeToX(const Eks::Time &t) const;

  void setCurrentTime(const Eks::Time &t);

  MomentItem *addMoment(const Eks::Time &t);
  DurationItem *addDuration(const Eks::Time &start);

  void endDuration(DurationItem *e, const Eks::Time &time);

  void selectEvent(EventItem *item, const QPointF &pos);

  QRectF boundingRect() const X_OVERRIDE;

  void paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *) X_OVERRIDE;

public slots:
  void timeConversionChanged();

private:
  Eks::Vector <EventContainer *> _containers;
  Eks::Vector <DurationItem *> _openDurations;
  };

#endif // LOGVIEW_H
