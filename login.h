#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include "virtualbox.h"

class Login : public QDialog
{
    Q_OBJECT
public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();
    QString GetPassword() const;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
    void loginSuccess(QString &role);

private slots:
    void showVirtualKeyboard();
    void handleKeyPress(const QString &key);
    bool validateLogin(const QString &role, const QString &password);
    void onOkButtonClicked();
private:
    QLineEdit *passwordEdit;
    VirtualBox *virtualKeyboard;
    QComboBox *roleComboBox;
};
#endif // LOGIN_H
