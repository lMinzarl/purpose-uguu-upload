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
        qWarning() << "Uguu: start() called";
        const QJsonArray urls = data().value(QLatin1String("urls")).toArray();
        qWarning() << "Uguu: data() contains the following: " << data();
        if (urls.isEmpty()) {
            qWarning() << "no urls to share" << urls << data();
            emitResult();
            return;
        }
        for (qsizetype i = 0; i < urls.size(); ++i) {
            qWarning() << "Uguu URL" << i << ":" << urls.at(i).toString();
        }
        const QString input = urls.first().toString();

        QUrl url(input);

        QString filePath;

        if (url.isLocalFile()) {
        filePath = url.toLocalFile();
    } else {

        filePath = input;
        }
        qWarning() << "Uguu: local screenshot:" << filePath;
        uploadFile(filePath);
    }

   void uploadFile(const QString &filePath)
{
    qWarning() << "Uguu: uploadFile() called";

    auto *multiPart =
        new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart imagePart;

    auto *screenshot = new QFile(filePath);

    if (!screenshot->open(QIODevice::ReadOnly)) {
        qWarning() << "Uguu: couldn't open screenshot:"
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
        QUrl(QStringLiteral("https://uguu.se/upload.php"))
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
    qWarning() << "Uguu: fileUploaded() started";

    auto *reply = qobject_cast<QNetworkReply *>(sender());

    if (!reply) {
        setError(1);
        setErrorText(QStringLiteral("Invalid network reply."));
        emitResult();
        return;
    }

    const QByteArray responseData = reply->readAll();

    qWarning() << "Uguu response:" << responseData;

    if (reply->error() != QNetworkReply::NoError) {
        setError(1);
        setErrorText(reply->errorString());

        reply->deleteLater();
        emitResult();
        return;
    }

    QJsonParseError parseError;

    const QJsonDocument document =
        QJsonDocument::fromJson(responseData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        setError(1);
        setErrorText(
            QStringLiteral("Could not parse Uguu response: %1")
                .arg(parseError.errorString())
        );

        reply->deleteLater();
        emitResult();
        return;
    }

    const QJsonObject root = document.object();

    const QJsonArray files =
        root.value(QStringLiteral("files")).toArray();

    if (files.isEmpty()) {
        setError(1);
        setErrorText(QStringLiteral("Uguu returned no uploaded files."));

        reply->deleteLater();
        emitResult();
        return;
    }

    const QJsonObject uploadedFile =
        files.first().toObject();

    const QString url =
        uploadedFile.value(QStringLiteral("url")).toString();

    if (url.isEmpty()) {
        setError(1);
        setErrorText(QStringLiteral("Uguu returned no URL."));

        reply->deleteLater();
        emitResult();
        return;
    }

    qWarning() << "Uguu uploaded URL:" << url;

    setOutput(
        QJsonObject{
            {QStringLiteral("url"), url}
        }
    );

    reply->deleteLater();
    QGuiApplication::clipboard()->setText(url);

    KNotification::event(
    KNotification::Notification,
    QStringLiteral("Uguu Upload"),
    QStringLiteral(
        "Upload complete.<br>"
        "<a href=\"%1\">%1</a><br><br>"
        "The URL has been copied to the clipboard."
    ).arg(url),
    KNotification::Persistent
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