#include "login.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QEvent>
#include <QObject>
#include <QDebug>

Login::Login(QDialog *parent) : QDialog(parent)
{
    setWindowTitle("登录");
    setFixedSize(400, 250);

    QLabel *passwordLabel = new QLabel("输入密码", this);
    passwordLabel->setFixedWidth(80);
    passwordLabel->setStyleSheet("font-size: 16px;");

    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFixedSize(250, 40);
    passwordEdit->installEventFilter(this);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(passwordLabel);
    inputLayout->addWidget(passwordEdit);
    inputLayout->addStretch();

    QPushButton *okButton = new QPushButton("确定", this);
    okButton->setFixedSize(80, 30);

    QPushButton *cancelButton = new QPushButton("取消", this);
    cancelButton->setFixedSize(80, 30);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addSpacing(10);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(inputLayout);
    mainLayout->addSpacing(30);
    mainLayout->addLayout(buttonLayout);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    setLayout(mainLayout);

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    keyboard = nullptr;
    createVirtualKeyboard();
}

QString Login::GetPassword() const
{
    return passwordEdit->text();
}

bool Login::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == passwordEdit && event->type() == QEvent::MouseButtonPress) {
        if (keyboard) {
            keyboard->show(); // 第66行
            keyboard->move(this->pos().x(), this->pos().y() + this->height());
        }
        return true;
    }
    return QDialog::eventFilter(obj, event);
}

void Login::createVirtualKeyboard()
{
    keyboard = new VirtualKeyboard(this);
    keyboard->hide();
}
