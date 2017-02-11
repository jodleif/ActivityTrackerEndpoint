#include "globals.h"
#include "postgres.h"
#include "rest.h"
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

void
println(std::string str)
{
  std::cout << str << '\n';
}

void
print_db_res(db::db_result res)
{
  using r = db::db_result;
  switch (res) {
    case r::OK:
      println("OK");
      break;
    case r::FAILURE:
      println("FAILURE");
      break;
    case r::USER_DOES_NOT_EXIST:
      println("USER DOES NOT EXIST");
      break;
    case r::USER_EXISTS:
      println("USER EXISTS");
      break;
    case r::WRONG_PASSWORD:
      println("WRONG PASSWORD");
      break;
  }
}

auto print_debug_info = [](auto* req, auto* res) {
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
  /**auto db_conn_ptr = db::open_db_connection();
  db::prepare_connection(db_conn_ptr.get());
  {
    db::make_transaction(db_conn_ptr.get(), "select * from users");
    print_db_res(db::delete_user(db_conn_ptr.get(), "jodleif@gmail.com",
  "password123"));
    db::make_transaction(db_conn_ptr.get(), "select * from users");
    print_db_res(db::insert_user(db_conn_ptr.get(), "jodleif@gmail.com",
  "password123"));
    db::make_transaction(db_conn_ptr.get(), "select * from users");
  }*/
  QCoreApplication app(argc, argv);
  qhttp::server::QHttpServer server(&app);
  server.listen(QHostAddress::Any, config::HTTP_PORT, [](qhttp::server::QHttpRequest* req,
                                            qhttp::server::QHttpResponse* res) {
    req->collectData(config::MAX_DATA);
    // Control flow
    auto url = req->url().toString().toStdString();

    // Make response -- fill buffer
    QByteArray result_buffer = rest::process_request(url);
    req->onEnd([req, res, result_buffer]() {
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
