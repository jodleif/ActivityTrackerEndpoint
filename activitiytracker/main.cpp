#include <QCoreApplication>
#include <qhttpserver.hpp>
#include <qhttpserverresponse.hpp>
#include <qhttpserverrequest.hpp>
int
main(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);
  qhttp::server::QHttpServer server(&app);
    server.listen(QHostAddress::Any,8080,
                  [](qhttp::server::QHttpRequest* req, qhttp::server::QHttpResponse* res)
    {
        res->setStatusCode(qhttp::ESTATUS_OK);
        res->end("Hello world\n");
    });
    if (!server.isListening()){
        qDebug("failed to listen");
        return -1;
    }
    return app.exec();
}
