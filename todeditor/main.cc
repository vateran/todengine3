#include "todeditor.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QSurfaceFormat>

class TodEditorApplication : public QApplication
{
public:
    TodEditorApplication(int argc, char* argv[]):QApplication(argc, argv) {}
    bool notify(QObject* receiver, QEvent* event) override
    {
        bool done = true;
        try
        {
            done = QApplication::notify(receiver, event);
        }
        catch (const tod::Exception& e)
        {
            QMessageBox::critical(tod::editor::TodEditor::instance(),
                "Error", QString::fromLocal8Bit(e.what()));
            return false;
        }
        catch (...)
        {
            return false;
        }
        return done;
    }
};

int main(int argc, char *argv[])
{
    //Opengl 설정
    QSurfaceFormat fmt { QSurfaceFormat::defaultFormat() };
    fmt.setVersion(4, 0);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setStencilBufferSize(8);
    fmt.setDepthBufferSize(24);
    fmt.setSwapInterval(10);
    QSurfaceFormat::setDefaultFormat(fmt);

    TodEditorApplication a(argc, argv);
    a.setKeyboardInputInterval(0);
    
    //DarkStyle theme 적용
    QFile f(":/Resources/darkstyle.qss");
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&f);
    a.setStyleSheet(ts.readAll());
    
    //에디터 실행
    tod::editor::TodEditor w;
    w.showMaximized();
    return a.exec();
}
