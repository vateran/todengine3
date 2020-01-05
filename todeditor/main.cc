#include <windows.h>
#include "todeditor.h"
#include "tod/graphics/renderinterface.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QSurfaceFormat>
#include <QSettings>
#include <QStyleFactory>
#include <QFontDatabase>

namespace tod
{

//----------------------------------------------------------------------------
class TodEditorApplication : public QApplication
{
public:
    TodEditorApplication(int32 argc, char* argv[]):QApplication(argc, argv) {}
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

}

//----------------------------------------------------------------------------
#if defined (TOD_PLATFORM_MAC)
int32 main(int32 argc, char* argv[])
#else
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
#endif
{
    //Opengl 설정
    QSurfaceFormat fmt{ QSurfaceFormat::defaultFormat() };
    fmt.setVersion(4, 0);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setStencilBufferSize(8);
    fmt.setDepthBufferSize(24);
    fmt.setSwapInterval(10);
    QSurfaceFormat::setDefaultFormat(fmt);

    tod::TodEditorApplication app(0, nullptr);
    app.setKeyboardInputInterval(0);

    int id = QFontDatabase::addApplicationFont(":/Resources/Poppins-Light.ttf");
    QFont editorFont("Poppins Light", 10);
    app.setFont(editorFont);

    //DarkStyle theme 적용
    QFile f(":/Resources/darkstyle.qss");
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&f);
    app.setStyleSheet(ts.readAll());

    //에디터 실행
    tod::editor::TodEditor w;
    //w.showMaximized();
    w.show();
    return app.exec();
}