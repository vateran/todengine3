#include <QVBoxLayout>
#include <QTextEdit>
#include "todeditor/logoutput.h"

namespace tod::editor
{

//-----------------------------------------------------------------------------
LogOutput::LogOutput()
: DockWidget<LogOutput>
    (DockWidgetOption()
    .setName("Log Output")
    .setMinSize(QSize(100, 100))
    .setDockArea(Qt::BottomDockWidgetArea))
, logText(nullptr)
{
    auto main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(1);

    this->logText = new QTextEdit(this);
    this->logText->setReadOnly(true);
    main_layout->addWidget(this->logText);

    class TodQtLog : public tod::Log
    {
    public:
        TodQtLog(QTextEdit* log_text):logText(log_text) {}
        virtual~TodQtLog() {}
        virtual void log(const char* log) const
        {
            logText->append(log);
        }

    public:
        QTextEdit* logText;
    };
    Logger::instance()->addCustomLog(new TodQtLog(this->logText));
}


//-----------------------------------------------------------------------------
LogOutput::~LogOutput()
{
}

}