#ifndef GAMENOTATIONWIDGET_H
#define GAMENOTATIONWIDGET_H

#include <QAction>
#include <QWidget>

#include "nag.h"
#include "editaction.h"
#include "gamex.h"

class ChessBrowser;
class Output;


class GameNotationWidget : public QWidget
{
    Q_OBJECT

public:
    GameNotationWidget(QWidget* parent = nullptr);
    ~GameNotationWidget();

    QString getHtml() const;
    QString getText() const;

    QString generateText(const GameX& game, bool trainingMode);
    void reload(const GameX& game, bool trainingMode);

    QMap<Nag, QAction*> nagActions() const;

public slots:
    /** Store current configuration. */
    void saveConfig();
    /** Restore current configuration. */
    void slotReconfigure();
    /** Scroll to show given mode. */
    void showMove(int id);

signals:
    void anchorClicked(const QUrl& url);
    void actionRequested(const EditAction& action);
    void queryActiveGame(const GameX** game);
    void signalMergeGame(GameId gameIndex, QString source);

private:
    void configureFont();

    ChessBrowser *m_browser;
    Output* m_output;
};

#endif
