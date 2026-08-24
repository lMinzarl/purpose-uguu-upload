#include <KPluginFactory>
#include <purpose/job.h>
#include <purpose/pluginbase.h>
#include <KNotification>
#include <KIO/StoredTransferJob>
#include <KJob>
#include <QJsonArray>
#include <QJsonValue>
#include <QUrl>
#include <QDebug>
#include <QHttpMultiPart>
#include <QFile>
#include <QIODevice>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFileInfo>

class UguuShareJob : public Purpose::Job
{
    Q_OBJECT

public:
    explicit UguuShareJob(QObject *parent = nullptr)
        : Purpose::Job(parent)
    {
    }

    void start() override
    {
        qWarning() << "Uguu: start() called";
        const QJsonArray urls = data().value(QLatin1String("urls")).toArray();
        if (urls.isEmpty()) {
            qWarning() << "no urls to share" << urls << data();
            emitResult();
            return;
        }
    for (const QJsonValue &val : urls) {
        QString u = val.toString();
        KIO::StoredTransferJob *job = KIO::storedGet(QUrl(u));
        
        connect(job, &KJob::result, this, &UguuShareJob::fileFetched);

       
    
        }
    }

    void fileFetched(KJob *j)
{
    qWarning() << "Uguu: fileFetched() called";


    KIO::StoredTransferJob *job = qobject_cast<KIO::StoredTransferJob *>(j);
    qWarning() << "Uguu: job error:" << job->error() << job->errorText()<< job->url().toString();
    
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
    QFile *screenshot = new QFile(job->url().toLocalFile());
    screenshot->open(QIODevice::ReadOnly);
    imagePart.setHeader(
    QNetworkRequest::ContentDispositionHeader,
    QVariant(
        QStringLiteral(
            "form-data; name=\"files[]\"; filename=\"%1\""
        ).arg(QFileInfo(screenshot->fileName()).fileName())
    )
);
    

    imagePart.setBodyDevice(screenshot);
    screenshot->setParent(multiPart); // need to invistigate
    
    multiPart->append(imagePart);

    QUrl server("https://uguu.se/upload");
    QNetworkRequest request(server);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkReply *reply = manager->post(request, multiPart);
    
    multiPart->setParent(reply); // need to invistigate
    
    connect(reply, &QNetworkReply::finished, this, &UguuShareJob::fileUploaded);
    
}
   
void fileUploaded()
{
    qWarning() << "Uguu: fileUploaded() started";
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    
    QVariant statusCodeVariant = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    qWarning() << "Uguu: file uploaded with the following status code :" << statusCodeVariant;
}

};

class UguuPlugin : public Purpose::PluginBase
{
    Q_OBJECT

public:
    using PluginBase::PluginBase;

    Purpose::Job *createJob() const override
    {
        return new UguuShareJob(nullptr);
    }
};

K_PLUGIN_CLASS_WITH_JSON(UguuPlugin, "uguuplugin.json")

#include "uguuplugin.moc"