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

void println(std::string str)
{
    std::cout << str << '\n';
}

void
print_db_res(db::db_result res)
{
    using r = db::db_result;
    switch(res){
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

int
main(int argc, char* argv[])
{
  auto db_conn_ptr = db::open_db_connection();
  db::prepare_connection(db_conn_ptr.get());
  {
    db::make_transaction(db_conn_ptr.get(), "select * from users");
    print_db_res(db::delete_user(db_conn_ptr.get(), "jodleif@gmail.com", "password"));
    db::make_transaction(db_conn_ptr.get(), "select * from users");
    print_db_res(db::insert_user(db_conn_ptr.get(), "jodleif@gmail.com", "password123"));
    db::make_transaction(db_conn_ptr.get(), "select * from users");
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
