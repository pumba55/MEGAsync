#include "GuestWidget.h"
#include "ui_GuestWidget.h"
#include "megaapi.h"

using namespace mega;

GuestWidget::GuestWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GuestWidget)
{
    ui->setupUi(this);

    transferMenu = NULL;
    isIdle = true;

    ui->wActiveDownload->setType(MegaTransfer::TYPE_DOWNLOAD);
    ui->wActiveDownload->hideTransfer();
    connect(ui->wActiveDownload, SIGNAL(cancel(int, int)), this, SLOT(onTransferCancel(int, int)));

    overlayIdle = new QPushButton(this);
    overlayIdle->setIcon(QIcon(QString::fromAscii("://images/non_logged_graphic.png")));
    overlayIdle->setIconSize(QSize(104, 104));
    overlayIdle->resize(ui->wContainer->minimumSize());

    overlayIdle->move(2, 20);
#ifndef __APPLE__
    overlayIdle->resize(overlayIdle->width()-4, overlayIdle->height());
#endif

    overlayPaused = new QPushButton(this);
    connect(overlayPaused, SIGNAL(clicked()), this, SIGNAL(pauseClicked()));
    overlayPaused->setIcon(QIcon(QString::fromAscii("://images/tray_paused_large_ico.png")));
    overlayPaused->setIconSize(QSize(64, 64));
    overlayPaused->setStyleSheet(QString::fromAscii("background-color: rgba(247, 247, 247, 200); "
                                              "border: none; "));
    overlayPaused->resize(ui->wContainer->minimumSize());

#ifndef __APPLE__
    overlayPaused->move(2, 0);
    overlayPaused->resize(overlayPaused->width()-4, overlayPaused->height());
#endif

    overlayPaused->hide();

    setIdleState(true);
}

ActiveTransfer *GuestWidget::getTransfer()
{
    return ui->wActiveDownload;
}

void GuestWidget::setDownloadLabel(QString dString)
{
    ui->lDownloads->setText(dString);
}

void GuestWidget::setRemainingTime(QString time)
{
    ui->lRemainingTimeD->setText(time);
}

void GuestWidget::hideDownloads()
{
    ui->wDownloadDesc->hide();
}
void GuestWidget::showDownloads()
{
    ui->wDownloadDesc->show();
}

bool GuestWidget::idleState()
{
    return isIdle;
}

void GuestWidget::setIdleState(bool state)
{
    isIdle = state;
    if (state)
    {
        ui->wActiveDownload->hide();
        ui->wDownloadDesc->hide();
        overlayIdle->setVisible(true);
        ui->wDownload->setVisible(false);
    }
    else
    {
        ui->wActiveDownload->show();
        ui->wDownloadDesc->show();
        overlayIdle->setVisible(false);
        ui->wDownload->setVisible(true);
    }
}

void GuestWidget::setPauseState(bool state)
{
    state ? overlayPaused->show() : overlayPaused->hide();
}

bool GuestWidget::pauseState()
{
    return overlayPaused->isVisible();
}
GuestWidget::~GuestWidget()
{
    delete ui;
}

void GuestWidget::onTransferCancel(int x, int y)
{
    if (transferMenu)
    {
#ifdef __APPLE__
        transferMenu->close();
        return;
#else
        transferMenu->deleteLater();
#endif
    }

    transferMenu = new QMenu();
#ifndef __APPLE__
    transferMenu->setStyleSheet(QString::fromAscii(
            "QMenu {background-color: white; border: 2px solid #B8B8B8; padding: 5px; border-radius: 5px;} "
            "QMenu::item {background-color: white; color: black;} "
            "QMenu::item:selected {background-color: rgb(242, 242, 242);}"));
#endif

    QAction *cancelAll = transferMenu->addAction(tr("Cancel all downloads"), this, SIGNAL(cancelAllDownloads()));
    QAction *cancelCurrent = transferMenu->addAction(tr("Cancel download"), this, SIGNAL(cancelCurrentDownload()));
    transferMenu->addAction(cancelCurrent);
    transferMenu->addAction(cancelAll);

#ifdef __APPLE__
    transferMenu->exec(ui->wActiveDownload->mapToGlobal(QPoint(x, y)));
    if (!this->rect().contains(this->mapFromGlobal(QCursor::pos())))
    {
        this->hide();
    }
    transferMenu->deleteLater();
    transferMenu = NULL;
#else
    transferMenu->popup(ui->wActiveDownload->mapToGlobal(QPoint(x, y)));
#endif
}

void GuestWidget::on_bLogin_clicked()
{
    emit actionButtonClicked(LOGIN_CLICKED);
}

void GuestWidget::on_bCreateAccount_clicked()
{
    emit actionButtonClicked(CREATE_ACCOUNT_CLICKED);
}
