#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QLineEdit>
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QObject> // 添加 QObject 支持 findChild

class VirtualKeyboard : public QWidget
{
    Q_OBJECT
public:
    VirtualKeyboard(QWidget *parent = nullptr)
    {
        setFixedSize(400, 200);
        QGridLayout *layout = new QGridLayout(this);

        const char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        int row = 0, col = 0;
        for (int i = 0; i < 26; ++i) {
            QPushButton *button = new QPushButton(QString(letters[i]), this);
            button->setFixedSize(35, 35);
            layout->addWidget(button, row, col);
            connect(button, &QPushButton::clicked, this, [this, button]() { // 确保捕获 this
                QLineEdit *edit = this->parent()->findChild<QLineEdit*>(); // 显式使用 this
                if (edit) edit->setText(edit->text() + button->text());
            });
            col++;
            if (col >= 10) {
                col = 0;
                row++;
            }
        }
        setLayout(layout);
    }
};

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QDialog *parent = nullptr);
    QString GetPassword() const;

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    QLineEdit *passwordEdit;
    VirtualKeyboard *keyboard;
    void createVirtualKeyboard();
};

#endif // LOGIN_H
