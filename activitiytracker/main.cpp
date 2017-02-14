#include "globals.h"
#include "postgres.h"
#include "rest.h"
#include <QCoreApplication>
#include <QHash>
#include <iostream>
#include <qhttpserver.hpp>
#include <qhttpserverrequest.hpp>
#include <qhttpserverresponse.hpp>

auto print_debug_info = [](auto* req) {
  qDebug("\n--> %s : %s", qhttp::Stringify::toString(req->method()),
         qPrintable(req->url().toString().toUtf8()));

  auto h = req->headers();
  qDebug("[Headers (%d)]", h.size());
  auto str = req->methodString().toStdString();
  qDebug("Method-string %s", str.c_str());
  h.forEach([](auto iter) {
    qDebug(" %s : %s", iter.key().constData(), iter.value().constData());
  });
};

int
main(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);
  qhttp::server::QHttpServer server(&app);
  server.listen(
    QHostAddress::LocalHost, config::HTTP_PORT,
    [](qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res) {
      req->collectData(config::MAX_DATA);

      print_debug_info(req);
      req->onEnd([req, res]() {
        auto url = req->url().toString().toStdString();
        const auto& request_body = req->collectedData();

        // Create response based on route and post body
        QByteArray result_buffer = rest::process_request(url, request_body);
        res->setStatusCode(qhttp::ESTATUS_OK);
        res->addHeader("connection", "close");
        res->addHeaderValue("content-length", result_buffer.size());

        res->end(result_buffer);
      });
    });
  if (!server.isListening()) {
    qDebug("failed to listen");
    return -1;
  }
  return app.exec();
}
