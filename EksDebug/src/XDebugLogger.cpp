#if 0
#include "XDebugLogger.h"
#include "QDataStream"
#include "QDebug"
#include "QtGui/QStandardItemModel"
#include "Containers/XVector.h"
#include "QtWidgets/QApplication"
#include "QThread"

namespace Eks
{

QDataStream &operator<<(QDataStream &s, const DebugLogger::LogEntry &l)
  {
  return s << l.time << (xuint64)l.thread << l.level << l.entry;
  }

QDataStream &operator>>(QDataStream &s, DebugLogger::LogEntry &l)
  {
  xuint64 thr;
  s >> l.time >> thr >> l.level >> l.entry;
  l.thread = (void*)thr;
  return s;
  }

QDataStream &operator<<(QDataStream &s, const ThreadEventLogger::EventItem &l)
  {
  return s << l.time << (const xuint8&)l.type << l.location << l.id;
  }

QDataStream &operator>>(QDataStream &s, ThreadEventLogger::EventItem &l)
  {
  s >> l.time >> (xuint8&)l.type >> l.location >> l.id;

  return s;
  }

QDataStream &operator<<(QDataStream &s, const EventLogger::LocationReference &l)
  {
  return s << l.id << l.data << l.file << l.function << l.line;
  }

QDataStream &operator>>(QDataStream &s, DebugLogger::DebugLocationWithData &l)
  {
  auto readStr = [&s]()
    {
    quint32 len;
    s >> len;

    char *data = (char *)alloca(len);
    s.readRawData(data, len);

    if(len > 1)
      {
      return QString::fromUtf8(data, len - 1);
      }

    return QString();
    };

  QString file, function, data;
  decltype(EventLogger::LocationReference::line) line;
  EventLocation::ID id;
  s >> id;
  l.setData(readStr());
  l.setFile(readStr());
  l.setFunction(readStr());
  s >> line;

  l.setId(id);
  l.setLine(line);

  return s;
  }

QDataStream &operator<<(QDataStream &s, const DebugLogger::LocationList &l)
  {
  return s << *l.embeddedLocations;
  }

QDataStream &operator>>(QDataStream &s, DebugLogger::LocationList &l)
  {
  return s >> l.allocatedLocations;
  }


QDataStream &operator<<(QDataStream &s, const DebugLogger::EventList &l)
  {
  return s << (xuint64)l.thread << *l.events;
  }

QDataStream &operator>>(QDataStream &s, DebugLogger::EventList &l)
  {
  xuint64 thr;
  s >> thr >> l.inPlaceEvents;
  l.events = &l.inPlaceEvents;
  l.thread = (void*)thr;
  return s;
  }

DebugLogger *g_logger;
QtMessageHandler g_oldHandler;
void handler(QtMsgType t, const QMessageLogContext &c, const QString &m)
  {
  static bool inHandler = false;
  if(g_oldHandler)
    {
    g_oldHandler(t, c, m);
    }

  if(inHandler)
    {
    return;
    }
  inHandler = true;

  DebugLogger::LogEntry e;
  e.level = t;
  e.entry = m;
  e.time = Time::now();
  e.thread = QThread::currentThread();

  xAssert(g_logger)
  g_logger->emitLogMessage(e);

  inHandler = false;
}

X_IMPLEMENT_DEBUG_INTERFACE(DebugLogger)

DebugLogger::DebugLogger(DebugManager *, bool client)
  {
  // we need an instance to flush on close.
  xAssert(QApplication::instance());

  static Reciever recv[] =
    {
    recieveFunction<LogEntry, DebugLogger, &DebugLogger::onLogMessage>(),
    recieveFunction<EventList, DebugLogger, &DebugLogger::onEventList>(),
    recieveFunction<LocationList, DebugLogger, &DebugLogger::onCodeLocations>()
    };

  setRecievers(recv, X_ARRAY_COUNT(recv));

  if(client)
    {
    xAssert(!g_logger);
    g_logger = this;
    g_oldHandler = qInstallMessageHandler(handler);

#if X_EVENT_LOGGING_ENABLED
    Core::eventLogger()->setEventWatcher(this);
#endif
    }
  else
    {
    _server = Eks::Core::defaultAllocator()->createUnique<ServerData>();
    _server->model = createDataModel<DebugLoggerData>();
    }

#if X_EVENT_LOGGING_ENABLED
  startTimer(25);
#endif
  }

DebugLogger::~DebugLogger()
  {
  Core::eventLogger()->syncCachedEvents();

  if(g_logger)
    {
    qInstallMessageHandler(g_oldHandler);

    g_logger = 0;
    g_oldHandler = 0;

#if X_EVENT_LOGGING_ENABLED
    Core::eventLogger()->setEventWatcher(0);
#endif
    }
  }

void DebugLogger::emitLogMessage(const LogEntry &e)
  {
  sendData(e);
  }

void DebugLogger::timerEvent(QTimerEvent*)
  {
#if X_EVENT_LOGGING_ENABLED
    Core::eventLogger()->syncCachedEvents();
#endif
  }

void DebugLogger::onEvents(const QThread *thread, const ThreadEventLogger::EventVector &events)
  {
  EventList l;
  l.thread = thread;
  l.events = &events;

  sendData(l);
  }

void DebugLogger::onLocations(const EventLogger::EventLocationVector &locations)
  {
  LocationList l = { &locations, Eks::Vector<DebugLocationWithData, 1024>() };
  sendData(l);
  }

void DebugLogger::onLogMessage(const LogEntry &e)
  {
  if(_server)
    {
    static const QString statuses[] =
      {
      "Debug",
      "Warning",
      "Critical",
      "Fatal",
      "System"
      };

    Q_EMIT _server->model->eventCreated(
          e.time,
          ThreadEventLogger::EventType::Moment,
          (xuint64)e.thread,
          statuses[e.level] + ":\n" + e.entry,
          std::numeric_limits<xsize>::max(),
          nullptr);
    }
  }

void DebugLogger::onEventList(const EventList &list)
  {
  if (_server)
    {
    xForeach(const auto &evt, *list.events)
      {
      if(evt.type == ThreadEventLogger::EventType::Begin ||
         evt.type == ThreadEventLogger::EventType::Moment)
        {
        auto location = findLocation(evt.location);

        QString display = QStringLiteral("No Data");
        if(location)
          {
          if(!location->data().isEmpty())
            {
            display = location->data();
            }
          else
            {
            display = location->function();
            }
          }

        Q_EMIT _server->model->eventCreated(
              evt.time,
              evt.type,
              (xuint64)list.thread,
              display,
              evt.id,
              location);
        }
      else if(evt.type == ThreadEventLogger::EventType::End)
        {
        Q_EMIT _server->model->eventEndUpdated(evt.id, (xuint64)list.thread, evt.time);
        }
      }
    }
  }

void DebugLogger::onCodeLocations(const LocationList &e)
  {
  xAssert(_server);

  xForeach(const auto &ev, e.allocatedLocations)
    {
    auto id = (xsize)ev.id();
    _server->_locations.resize(xMax(_server->_locations.size(), id+1));

    auto &location = _server->_locations[id];
    location.setFile(ev.file());
    location.setLine(ev.line());
    location.setFunction(ev.function());
    location.setData(ev.data());
    }
  }

const DebugLogger::DebugLocationWithData *DebugLogger::findLocation(Eks::EventLocation::ID id)
  {
  if(id >= _server->_locations.size())
    {
    return nullptr;
    }

  return &_server->_locations[id];
  }

}

#endif
