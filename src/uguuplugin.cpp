#include <KPluginFactory>
#include <purpose/job.h>
#include <purpose/pluginbase.h>
#include <KNotification>
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
#include <QGuiApplication>
#include <QClipboard>

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
        qDebug() << "Uguu: start() called";
        const QJsonArray urls = data().value(QLatin1String("urls")).toArray();
        qDebug() << "Uguu: data() contains the following: " << data();
        if (urls.isEmpty()) {
            qCritical() << "Uguu: No file was provided.";
            setError(1);
            setErrorText(QStringLiteral("No file was provided."));
            emitResult();
            return;
        }
        for (qsizetype i = 0; i < urls.size(); ++i) {
            qInfo() << "Uguu: file url " << i << ":" << urls.at(i).toString();
        }

        if (urls.size() != 1) {
            qCritical() << "Uguu: Uguu upload currently supports one file at a time.";
            setError(1);
            setErrorText(QStringLiteral("Uguu upload currently supports one file at a time."));
            emitResult();
            return;
        }
        const QString input = urls.first().toString();

        QUrl url(input);

        QString filePath;

        if (url.isLocalFile()) {
        filePath = url.toLocalFile();
    } else {

        filePath = input;
        }
        qInfo() << "Uguu: local screenshot:" << filePath;
        uploadFile(filePath);
    }

   void uploadFile(const QString &filePath)
{
    qDebug() << "Uguu: uploadFile() called";

    auto *multiPart =
        new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart imagePart;

    auto *screenshot = new QFile(filePath);

    if (!screenshot->open(QIODevice::ReadOnly)) {
        qCritical() << "Uguu: could not open screenshot."
                   << screenshot->errorString();

        delete screenshot;
        delete multiPart;

        setError(1);
        setErrorText(QStringLiteral("Could not open screenshot."));
        emitResult();
        return;
    }

    imagePart.setHeader(
        QNetworkRequest::ContentDispositionHeader,
        QVariant(
            QStringLiteral(
                "form-data; name=\"files[]\"; filename=\"%1\""
            ).arg(QFileInfo(filePath).fileName())
        )
    );

    imagePart.setBodyDevice(screenshot);

    screenshot->setParent(multiPart);
    multiPart->append(imagePart);

    QNetworkRequest request(
        QUrl(QStringLiteral("https://uguu.se/upload"))
    );

    auto *manager =
        new QNetworkAccessManager(this);

    QNetworkReply *reply =
        manager->post(request, multiPart);

    multiPart->setParent(reply);

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        &UguuShareJob::fileUploaded
    );
}
   
void fileUploaded()
{
    qDebug() << "Uguu: fileUploaded() started";

    auto *reply = qobject_cast<QNetworkReply *>(sender());

    if (!reply) {
        qCritical() << "Uguu: Invalid network reply.";
        setError(1);
        setErrorText(QStringLiteral("Invalid network reply."));
        emitResult();
        return;
    }

    const QByteArray responseData = reply->readAll();

    qInfo() << "Uguu response:" << responseData;

    if (reply->error() != QNetworkReply::NoError) {
        qCritical() << "Uguu: " << reply->errorString();
        setError(1);
        setErrorText(reply->errorString());
        QNetworkAccessManager *manager = reply->manager();
        reply->deleteLater();
        if(manager)
        {
            manager->deleteLater();
        }
        emitResult();
        return;
    }

    QJsonParseError parseError;

    const QJsonDocument document =
        QJsonDocument::fromJson(responseData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qCritical() << QStringLiteral("Uguu: Could not parse Uguu response: %1").arg(parseError.errorString());
        setError(1);
        setErrorText(
            QStringLiteral("Could not parse Uguu response: %1")
                .arg(parseError.errorString())
        );

        QNetworkAccessManager *manager = reply->manager();
        reply->deleteLater();
        if(manager)
        {
            manager->deleteLater();
        }
        emitResult();
        return;
    }

    const QJsonObject root = document.object();

    const QJsonArray files =
        root.value(QStringLiteral("files")).toArray();
    const QJsonValue success = root.value(QStringLiteral("success"));
    if(!success.toBool())
    {
        const QJsonValue code = root.value(QStringLiteral("errorcode"));
        const QJsonValue desc = root.value(QStringLiteral("description"));
        setError(1);
        qCritical() << "Uguu: returned an error with the code "<< code.toString() <<"\n error description: "<<desc.toString();
        setErrorText(QStringLiteral("Uguu couldn't upload the files\n Error: ")+desc.toString());

        QNetworkAccessManager *manager = reply->manager();
        reply->deleteLater();
        if(manager)
        {
            manager->deleteLater();
        }
        emitResult();
        return;
    }
    if (files.isEmpty()) {
        setError(1);
        qCritical() << "Uguu: Uguu returned no uploaded files.";
        setErrorText(QStringLiteral("Uguu returned no uploaded files."));

        QNetworkAccessManager *manager = reply->manager();
        reply->deleteLater();
        if(manager)
        {
            manager->deleteLater();
        }
        emitResult();
        return;
    }

    const QJsonObject uploadedFile =
        files.first().toObject();

    const QString url =
        uploadedFile.value(QStringLiteral("url")).toString();

    if (url.isEmpty()) {
        setError(1);
        qCritical() << "Uguu: Uguu returned no URL.";
        setErrorText(QStringLiteral("Uguu returned no URL."));

        QNetworkAccessManager *manager = reply->manager();
        reply->deleteLater();
        if(manager)
        {
            manager->deleteLater();
        }
        emitResult();
        return;
    }

    qInfo() << "Uguu uploaded URL:" << url;

    setOutput(
        QJsonObject{
            {QStringLiteral("url"), url}
        }
    );

    QNetworkAccessManager *manager = reply->manager();
    reply->deleteLater();
    if(manager)
    {
        manager->deleteLater();
    }
    QGuiApplication::clipboard()->setText(url);

    KNotification::event(
    KNotification::Notification,
    QStringLiteral("Uguu Upload"),
    QStringLiteral(
        "Upload complete.<br>"
        "<a href=\"%1\">%1</a><br><br>"
        "The URL has been copied to the clipboard."
    ).arg(url)
    );

    emitResult();
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