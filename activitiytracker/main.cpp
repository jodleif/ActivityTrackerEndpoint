#include "postgres.h"
#include <QCoreApplication>
#include <QHash>
#include <iostream>
#include <qhttpserver.hpp>
#include <qhttpserverrequest.hpp>
#include <qhttpserverresponse.hpp>
void
printstuff(const QByteArray& arr)
{
  for (const auto& b : arr)
    std::cout << b;
  std::cout << std::endl;
}

int
main(int argc, char* argv[])
{
  auto ptr = db::open_db_connection();
  {
    db::make_transaction(ptr.get(), "select * from activity");
    db::make_transaction(ptr.get(), "select * from users");
  }
  QCoreApplication app(argc, argv);
  qhttp::server::QHttpServer server(&app);
  server.listen(QHostAddress::Any, 8080, [](qhttp::server::QHttpRequest* req,
                                            qhttp::server::QHttpResponse* res) {
    req->collectData();
    printstuff(req->collectedData());
    auto x = req->headers();
    for (auto& ele : x) {
      printstuff(ele);
    }
    req->onEnd([req, res]() {
      res->setStatusCode(qhttp::ESTATUS_OK);
      res->addHeader("connection", "close");
      res->end();
    });
  });
  if (!server.isListening()) {
    qDebug("failed to listen");
    return -1;
  }
  return app.exec();
}
